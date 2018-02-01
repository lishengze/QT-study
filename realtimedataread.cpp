#include <QDebug>
#include <iostream>
#include <QDate>
#include <QThread>
#include <windows.h>
#include <QTime>
#include "toolfunc.h"
#include "WAPIWrapperCpp.h"
#include "realtimedataread.h"
using namespace std;

RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView, QObject* parent):
    m_programInfoTableView(programInfoTableView), m_updateTime(3000), m_realtimeDatabase(NULL),
    m_login(false),
    QObject(parent)
{

}

RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView, QTableView* errorMsgTableView, QString dirName,
                                   QList<QString> secodeList,Excel* excel,
                                   int updateTime, QObject* parent):
    m_programInfoTableView(programInfoTableView), m_errorMsgTableView(errorMsgTableView),
    m_strategyFileDir(dirName), m_secodeList(secodeList), m_realtimeDatabase(NULL),
    m_strategyModel(NULL), m_excel(excel), m_updateTime(updateTime), m_realtimeDataNumb(0),
    m_login(false),
    QObject(parent)
{
//    initComEnv();
    initTimer();
    initDatabase();
}

void RealTimeDataRead::initComEnv() {
    HRESULT r = OleInitialize(0);
    if (r != S_OK && r != S_FALSE) {
        qWarning("Qt:初始化Ole失败(error %x)",(unsigned int)r);
    }
}

void RealTimeDataRead::initDatabase() {
//    m_realtimeDatabase = new RealTimeDatabase("0", "Test");
    m_realtimeDatabase = new RealTimeDatabase("0", "MarketData_RealTime");
//    m_secodeList.clear();
//    m_secodeList << "000001.SZ" << "000002.SZ";
//    QDate date = m_realtimeDatabase->getDatabaseDataDate();
//    qDebug() << "date: " << date;
    QString clearMsg;
    if (!m_realtimeDatabase->checkdataTime()) {
       clearMsg = m_realtimeDatabase->clear();
        if (clearMsg != "SUCCESS") {
            updateProgramInfo(m_errorMsgTableView, QString("清空Database 失败, 错误消息为: %1").arg(clearMsg));
        } else {
            updateProgramInfo(m_programInfoTableView, QString("完成Database清空工作"));
        }
    } else {
        updateProgramInfo(m_programInfoTableView, QString("实时Datebase已经存在"));
    }

//    clearMsg = m_realtimeDatabase->clear();
//    if (clearMsg != "SUCCESS") {
//        updateProgramInfo(m_errorMsgTableView, QString("清空Database 失败, 错误消息为: %1").arg(clearMsg));
//    } else {
//        updateProgramInfo(m_programInfoTableView, QString("完成Database清空工作"));
//    }

    QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);
    if (compTableMsg != "SUCCESS") {
        updateProgramInfo(m_errorMsgTableView, QString("补完Database 失败, 错误消息为: %1").arg(compTableMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("根据策略股票代码补全Database"));
    }
}

void RealTimeDataRead::clearDatabase() {
    QString clearMsg = m_realtimeDatabase->clear();
    if (clearMsg != "SUCCESS") {
        updateProgramInfo(m_errorMsgTableView, QString("清空Database 失败, 错误消息为: %1").arg(clearMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("完成Database清空工作"));
    }

    QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);
    if (compTableMsg != "SUCCESS") {
        updateProgramInfo(m_errorMsgTableView, QString("补完Database 失败, 错误消息为: %1").arg(compTableMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("根据策略股票代码补全Database"));
    }
}

void RealTimeDataRead::setPreCloseData() {
    QString tableName = "PreCloseData";
    QList<QString> tableList = m_realtimeDatabase->getTableList(m_realtimeDatabase->getDatabaseName());
    if (tableList.indexOf(tableName) < 0 ) {
        m_realtimeDatabase->createPreCloseTable(tableName);
        QMap<QString, QStringList> precloseData = wsqPreCloseData();
        QList<QString> secodeList = precloseData.keys();
        for (int i = 0; i < secodeList.size(); ++i) {
            QString secode = secodeList[i];
            m_realtimeDatabase->insertPreCloseData(tableName, precloseData[secode]);
        }
        updateProgramInfo(m_programInfoTableView, QString("取得昨收数据"));
    }
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
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(getRealTimeData()));
}

void RealTimeDataRead::startTimer() {
    qDebug() << "starttimer: " << m_updateTime;
    setPreCloseData();
    m_timer.start(m_updateTime);
}

void RealTimeDataRead::stopTimer() {
    if (m_timer.isActive()) {
        qDebug() << "RealTimeDataRead::stopTimer()";
        m_timer.stop();
    }
}

void RealTimeDataRead::setSecodeList() {
//    m_secodeList << "000001.SZ" << "600000.SH";

    m_strategyModel = new StrategyModel(m_strategyFileDir);
    QList<QString> new_strategyFileList = m_strategyModel->getStrategyFileList();
//    qDebug() << "new_strategyFileList: " << new_strategyFileList;
    bool isNewFileIn = false;
    for (int i = 0; i < new_strategyFileList.size(); ++i) {
        QString file = new_strategyFileList[i];
        if (m_strategyFileList.indexOf(file) < 0) {
            m_strategyFileList.append(file);
            QList<QString> appendSecodeList = m_excel->getSecodeFromeExcel(file);
            for (int j = 0; j < appendSecodeList.size(); ++j) {
                m_secodeList.append(completeSecode(appendSecodeList[i], "wind"));
            }
            isNewFileIn = true;
        }
    }
    qDebug() << "m_secodeList: " << m_secodeList;
    if (true == isNewFileIn) {
        m_realtimeDatabase->completeTable(m_secodeList);
    }
}

void RealTimeDataRead::getRealTimeData() {
    if (isTradingOver(QTime::currentTime())) {
        updateProgramInfo(m_programInfoTableView, QString("交易已经结束"));
        emit sendStopTimerSignal();
        qDebug() << "emit sendStopTimerSignal();";
        return;
    }

    if (isTradingTime(QTime::currentTime())) {
        QMap<QString, QStringList> result = wsqSnapshootData();
        insertSnapshootData(result);
    } else if(!isTradingStart()){
        updateProgramInfo(m_programInfoTableView, QString("时间太早"));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("午休时间"));
    }

//    QMap<QString, QStringList> result = wsqSnapshootData();
//    insertSnapshootData(result);
}

QMap<QString, QStringList> RealTimeDataRead::wsqSnapshootData() {
    bool bOutputMsg = false;
    QMap<QString, QStringList> result;
    QTime curTime = QTime::currentTime();
    QTime amStopTime = QTime(11, 30, 10);
    QTime pmStartTime = QTime(13, 0, 0);
    if (curTime >= amStopTime && pmStartTime >= curTime) return result;
    int errcode;
    if (m_login) {
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
                singleCodeData.prepend(codes);
                if (bOutputMsg) cout << endl;
                result.insert(codes, singleCodeData);
            }
        } else {
             updateProgramInfo(m_errorMsgTableView, QString("获取实时数据失败, 错误代码: %1").arg(errcode));
        }
        delete[] windcodes;
    } else {
        updateProgramInfo(m_errorMsgTableView, QString("获取实时数据失败, 还没登录wind"));
    }
    return result;
}

QMap<QString, QStringList> RealTimeDataRead::wsqPreCloseData() {
    int errcode;
    QMap<QString, QStringList> result;
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecodeB(m_secodeList);
        LPCWSTR indicators = TEXT("pre_close");
        LPCWSTR options = TEXT("Days=Alldays");
        LPCWSTR beginDate = transSecode(QDate::currentDate().toString("yyyy-MM-dd"));
        bool bOutputMsg = false;
        errcode = CWAPIWrapperCpp::wsd(wd, windcodes, indicators, beginDate, beginDate, options);
        if (bOutputMsg) qDebug() << "wsd errcode: " << errcode;
        if (errcode == 0) {
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
                singleCodeData.prepend(codes);
                result.insert(codes, singleCodeData);
            }
        } else {
            updateProgramInfo(m_errorMsgTableView, QString("获取前收数据失败, 错误代码为: %1").arg(errcode));
        }
        delete[] beginDate;
    } else {
         updateProgramInfo(m_errorMsgTableView, QString("获取前收数据失败, 还没登录wind"));
    }
    return result;
}

void RealTimeDataRead::insertSnapshootData(QMap<QString, QStringList> result) {
    QList<QString> secodeList = result.keys();
    for (int i = 0; i < secodeList.size(); ++i) {
        QString secode = secodeList[i];
        QString msg = m_realtimeDatabase->insertData(secode, result[secode]);
        if (msg != "SUCCESS") {
            updateProgramInfo(m_errorMsgTableView, QString("插入 %1 实时数据失败, 失败消息为: %2").arg(secode).arg(msg));
        }
    }
    if (secodeList.size() != 0) {
        updateProgramInfo(m_programInfoTableView,
                          QString("插入 %1 实时数据, 数据总数: %2").arg(secodeList.size()).arg(++m_realtimeDataNumb));
    }

}

RealTimeDataRead::~RealTimeDataRead() {
//    OleUninitialize();

    if (NULL != m_strategyModel) {
        delete m_strategyModel;
        m_strategyModel = NULL;
    }



    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
}
