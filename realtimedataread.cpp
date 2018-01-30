#include "realtimedataread.h"
#include <QDebug>
#include <iostream>
#include <QDate>
#include <QThread>
#include "toolfunc.h"
#include "WAPIWrapperCpp.h"
using namespace std;

RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView, QObject* parent):
    m_programInfoTableView(programInfoTableView), m_updateTime(3000), m_realtimeDatabase(NULL),
    m_login(false),
    QObject(parent)
{

}

RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView,  QString dirName,
                                   int updateTime, QObject* parent):
    m_programInfoTableView(programInfoTableView),
    m_updateTime(updateTime), m_strategyFileDir(dirName), m_realtimeDatabase(NULL),
    m_strategyModel(NULL), m_excel(NULL),
    m_login(false),
    QObject(parent)
{
    initTimer();
    initDatabase();
}

void RealTimeDataRead::initDatabase() {
    m_realtimeDatabase = new RealTimeDatabase("0", "Test");
}

void RealTimeDataRead::loginWind() {
    int errcode = CWAPIWrapperCpp::start();
    if (0 == errcode) {
        cout << "login successfully" << endl;
        emit loginWindSucc();
        m_login = true;
    }else {
        emit loginWindFailed(errcode);
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        cout << buffer << endl;
        std::wcout.imbue(std::locale("chs"));
        std::wcout << buffer << std::endl;
    }
}

void RealTimeDataRead::getPreData(QList<QString> secodeList) {
    int errcode;
//    qDebug() << "RealTimeDataRead::getOldStrategySpread ";
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecode(secodeList);
        LPCWSTR indicators = TEXT("pre_close");
        LPCWSTR options = TEXT("Days=Alldays");
        LPCWSTR beginDate = transSecode(QDate::currentDate().toString("yyyy-MM-dd"));
        bool bOutputMsg = false;
        errcode = CWAPIWrapperCpp::wsd(wd, windcodes, indicators, beginDate, beginDate, options);
        if (bOutputMsg) qDebug() << "wsd errcode: " << errcode;
        if (errcode == 0) {
            QMap<QString, QStringList> result;
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            int colnum = fieldslen + 1;
            if (bOutputMsg) cout  << "WindCodes    ";
            for (int i =1;i < colnum; ++i)
            {
                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
                if (bOutputMsg) cout << outfields.toStdString() << "    ";
            }
            if (bOutputMsg) cout << endl;
            for (int i = 0; i < codelen; ++i)
            {
                QStringList singleCodeData;
                QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
                if (bOutputMsg) cout << codes.toStdString() << "    ";
                for (int j = 0; j < fieldslen; ++j)
                {
                    VARIANT var;
                    wd.GetDataItem(0,i,j,var);
                    QString temp = variantToQString(&var);
                    singleCodeData.append(temp);
                    if (bOutputMsg) cout << temp.toStdString() << "    ";
                }
                result.insert(codes, singleCodeData);
            }
            emit sendPreData(result);
        } else {

        }
        delete[] beginDate;
    } else {
        errcode = -1;
    }

}

void RealTimeDataRead::startWsq(QStringList secodeList, int reqID) {
//    qDebug() << "Thread: " << QThread::currentThreadId() <<  " wsq...... ";
    int errcode;
    if (m_login) {
        updateProgramInfo(m_programInfoTableView, "订阅实时数据");
//        qDebug() <<"secodeList: " << secodeList;
//        LPCWSTR windcodes = TEXT("600000.SH,000001.SZ,000002.SZ");
        LPCWSTR windcodes = transSecode(secodeList);
        wcout << "wcout windcodes: " << windcodes << endl;
        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_latest,rt_pre_close,rt_amt");
        LPCWSTR options = TEXT("");
        ULONGLONG reqid = unsigned long long (reqID);
        errcode = CWAPIWrapperCpp::wsq(reqid, windcodes, indicators, wsqCallBack, options, TRUE);
        qDebug() << "wsq errcode: " << errcode;
        if (errcode == 0) {
            emit startWsqSucc();
        } else {
            emit startWsqFailed(errcode, reqID);
        }
        delete[] windcodes;
    } else {
        errcode = -1;
        qDebug() << "Login first!";
    }
}

void RealTimeDataRead::cancelWsqRequest(int reqID) {
    ULONGLONG reqid = unsigned long long (reqID);
    CWAPIWrapperCpp::cancelRequest(reqid);
    qDebug() << "cancelWsqRequest ";
}

void RealTimeDataRead::cancelAllWsqRequest() {
    CWAPIWrapperCpp::cancelAllRequest();
    qDebug() << "cancelAllWsqRequest ";
}

void RealTimeDataRead::initTimer() {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(wsqSnapshootData()));
}

void RealTimeDataRead::startTimer() {
    qDebug() << "starttimer: " << m_updateTime;
    m_timer.start(m_updateTime);
}

void RealTimeDataRead::stopTimer() {
    m_timer.stop();
}

void RealTimeDataRead::setSecodeList() {
    m_secodeList << "000001.SZ" << "600000.SH";
    m_realtimeDatabase->completeTable(m_secodeList);
    m_strategyModel = new StrategyModel(m_strategyFileDir);
    m_secodeList = m_strategyModel->getStrategyFileList();
}

void RealTimeDataRead::wsqSnapshootData() {
    setSecodeList();
    bool bOutputMsg = false;
    QMap<QString, QStringList> result;
    QTime curTime = QTime::currentTime();
    QTime amStopTime = QTime(11, 30, 10);
    QTime pmStartTime = QTime(13, 0, 0);
    if (curTime >= amStopTime && pmStartTime >= curTime) return;
    int errcode;
    if (true) {
        WindData wd;
        LPCWSTR windcodes = transSecodeB(m_secodeList);
//        LPCWSTR windcodes = TEXT("000001.SZ");
        if (bOutputMsg) wcout << "windcodes: " << windcodes << endl;
        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_latest,rt_pre_close,rt_amt");
        LPCWSTR options = TEXT("");
        errcode = CWAPIWrapperCpp::wsq(wd, windcodes, indicators, options);
        if (bOutputMsg) qDebug() << "startWsqOneTime errcode: " << errcode;
        if (errcode == 0) {
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            int colnum = fieldslen + 1;
            QString datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            if (bOutputMsg)  cout  << "WindCodes    ";
            for (int i =1;i < colnum; ++i)
            {
                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
                if (bOutputMsg) cout << outfields.toStdString() << "    ";
            }
            if (bOutputMsg) cout << endl;
            for (int i = 0; i < codelen; ++i)
            {
                QStringList singleCodeData;
                QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
                if (bOutputMsg)  cout << codes.toStdString() << "    ";
                for (int j = 0; j < fieldslen; ++j)
                {
                    VARIANT var;
                    wd.GetDataItem(0,i,j,var);
                    QString temp = variantToQString(&var);
                    singleCodeData.append(temp);
                    if (bOutputMsg) cout << temp.toStdString() << "    ";
                }
                singleCodeData.append(datetime);
                if (bOutputMsg) cout << endl;
                result.insert(codes, singleCodeData);
            }
        } else {
//            emit startWsqFailed(errcode, reqID);
        }
        delete[] windcodes;
    } else {
        errcode = -1;
        qDebug() << "Login first!";
    }
    qDebug() << "result: " << result;
    insertSnapshootData(result);
    return ;
}

void RealTimeDataRead::insertSnapshootData(QMap<QString, QStringList> result) {
    QList<QString> secodeList = result.keys();
    for (int i = 0; i < secodeList.size(); ++i) {
        QString secode = secodeList[i];
        m_realtimeDatabase->insertData(secode, result[secode]);
    }
}

RealTimeDataRead::~RealTimeDataRead() {

    if (NULL != m_strategyModel) {
        delete m_strategyModel;
        m_strategyModel = NULL;
    }

    if (NULL != m_excel) {
        delete m_excel;
        m_excel = NULL;
    }

    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
}
