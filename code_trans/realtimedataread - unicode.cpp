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

RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView,
                                   QTableView* errorMsgTableView,
                                   QObject* parent):
    m_programInfoTableView(programInfoTableView), m_errorMsgTableView(errorMsgTableView),

    QObject(parent)
{
    initCommonData();
    initTimer();
}

void RealTimeDataRead::initCommonData() {
    m_isTooEarly = false;
    m_isRestTime = false;
    m_login = false;
    m_wsqSnapshootDataNumb = 500;
    m_updateTime = 3000;
}

void RealTimeDataRead::setSecodeList() {
    m_secodeList = wsqRealTimeSecodeList();
    QList<QString> indexCodeList = getIndexCode("wind");
    m_secodeList += indexCodeList;
    emit setSecodeList_signal(m_secodeList);
}

void RealTimeDataRead::loginWind_slot() {
    int errcode = CWAPIWrapperCpp::start();
    if (0 == errcode) {
        m_login = true;
        setSecodeList();
        setPreCloseData();

        updateProgramInfo(m_programInfoTableView, "登陆万得成功");
        emit loginWindSucc_signal();
    }else {      
        updateProgramInfo(m_errorMsgTableView, QString("登录万得失败, 错误代码: %1").arg(errcode));
        emit loginWindFailed_signal();
        //        WCHAR buffer[128];
        //        int bufferSize = 128;
        //        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        //        cout << buffer << endl;
        //        std::wcout.imbue(std::locale("chs"));
        //        std::wcout << buffer << std::endl;
    }
}

void RealTimeDataRead::initTimer() {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(setRealTimeData_slot()));
}

void RealTimeDataRead::startTimer() {
    qDebug() << "starttimer: " << m_updateTime;
    m_timer.start(m_updateTime);
}

void RealTimeDataRead::stopTimer() {
    if (m_timer.isActive()) {
        qDebug() << "RealTimeDataRead::stopTimer()";
        m_timer.stop();
    }
}

void RealTimeDataRead::setPreCloseData() {
    QMap<QString, QStringList> result = wsqPreCloseData();
    emit writePreCloseData_signal(result);
}

void RealTimeDataRead::setRealTimeData_slot() {
    QMap<QString, QStringList> result = getSnapshootData();
    if (result.size() != 0) {
        emit writeRealTimeData_signal(result);
    }

    if (isTradingOver(QTime::currentTime())) {
        return;
    }

    if (isTradingTime(QTime::currentTime())) {
        QMap<QString, QStringList> result = getSnapshootData();
        if (result.size() != 0) {
            emit writeRealTimeData_signal(result);
            emit stopReadRealTimeData_signal();
        }

    } else if(!isTradingStart()){
        if (!m_isTooEarly) {
            m_isTooEarly = true;
            updateProgramInfo(m_programInfoTableView, QString("时间太早"));
            emit startReadRealTimeData_signal();
        }
    } else {
        if (!m_isRestTime) {
            updateProgramInfo(m_programInfoTableView, QString("午休时间"));
            m_isRestTime = true;
            emit startReadRealTimeData_signal();
        }
    }
}

void RealTimeDataRead::writeComplete_slot() {
    setRealTimeData_slot();
}

QMap<QString, QStringList> RealTimeDataRead::getSnapshootData() {
    QMap<QString, QStringList> result;
    m_secodeList = getSubList(m_secodeList, 0, 1000);
    for(int i = 0; i < m_secodeList.size(); i+= m_wsqSnapshootDataNumb) {
        QList<QString> subSecodeList = getSubList(m_secodeList, i, i + m_wsqSnapshootDataNumb);
        result.unite(wsqSnapshootData(subSecodeList));
    }
    if (result.size() ==  m_secodeList.size()) {
        qDebug() << "getSnapshootData Successfully, result.size: " << result.size();
    }
    return result;
}

QMap<QString, QStringList> RealTimeDataRead::wsqSnapshootData(QList<QString> secodeList) {
    bool bOutputMsg = false;
    QMap<QString, QStringList> result;
    int errcode;
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecodeB(secodeList);
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

QList<QString> RealTimeDataRead::wsqRealTimeSecodeList() {
    int errcode;
    QList<QString> result;
    if (m_login) {
        bool bOutputMsg = true;
//        QString indexCode = "000906.SH";
//        QString conditionStr = QString("date=%1;windcode=%2").arg(QDate::currentDate().toString("yyyy-MM-dd")).arg(indexCode);
//        LPCWSTR conditions = transSecode(conditionStr);
//        LPCWSTR options = TEXT("indexconstituent");

        QString conditionStr = QString("date=%1;sectorid=a001010100000000").arg(QDate::currentDate().toString("yyyy-MM-dd"));
        LPCWSTR conditions = transSecode(conditionStr);
        LPCWSTR options = TEXT("sectorconstituent");

        WindData wd;
        errcode = CWAPIWrapperCpp::wset(wd, options, conditions);
        errcode = CWAPIWrapperCpp::wset(wd, L"indexconstituent",L"date=2018-03-09;windcode=000906.SH");


        qDebug() << "conditionStr: " << conditionStr;
//        qDebug() << "RealTimeDataRead::wsqRealTimeSecodeList ";

        if (bOutputMsg) qDebug() << "wsd errcode: " << errcode;
        if (errcode == 0) {
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            qDebug() << "codeLen: " <<codelen << ", filedslen: " << fieldslen;

            for (int i = 0; i < codelen; ++i)
            {
                VARIANT var;
                wd.GetDataItem(0,i,1,var);
                QString temp = variantToQString(&var);
                result.append(temp);
            }
        } else {
            updateProgramInfo(m_errorMsgTableView, QString("获取股票代码列表失败, 错误代码: %1").arg(errcode));
        }
        delete[] conditions;
    } else {
         updateProgramInfo(m_errorMsgTableView, QString("获取前收数据失败, 还没登录wind"));
    }
    return result;
}

RealTimeDataRead::~RealTimeDataRead() {
}

//void RealTimeDataRead::setSecodeList(QList<QString> secodeList) {
//    m_secodeList = secodeList;
//    m_realtimeDatabase->completeTable(m_secodeList);
//    m_realtimeDatabase->dropTable("PreCloseData");
//    qDebug() << "RealTimeDataRead::setSecodeList: 更新股票代码";
//    updateProgramInfo(m_programInfoTableView, QString("更新股票代码"));

//    setPreCloseData();
//}

//void RealTimeDataRead::checkDatabase() {
//    QString clearMsg;
//    if (!m_realtimeDatabase->checkdataTime()) {
//       clearMsg = m_realtimeDatabase->clear();
//        if (clearMsg != "SUCCESS") {
//            updateProgramInfo(m_errorMsgTableView, QString("清空Database 失败, 错误消息为: %1").arg(clearMsg));
//        } else {
//            updateProgramInfo(m_programInfoTableView, QString("完成Database清空工作"));
//        }
//    } else {
//        updateProgramInfo(m_programInfoTableView, QString("实时Datebase已经存在"));
//    }

//    QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);

//    if (compTableMsg != "SUCCESS") {
//        updateProgramInfo(m_errorMsgTableView, QString("补完Database 失败, 错误消息为: %1").arg(compTableMsg));
//    } else {
//        updateProgramInfo(m_programInfoTableView, QString("根据策略股票代码补全Database"));
//    }
//}

//void RealTimeDataRead::clearDatabase() {
//    QString clearMsg = m_realtimeDatabase->clear();
//    if (clearMsg != "SUCCESS") {
//        updateProgramInfo(m_errorMsgTableView, QString("清空Database 失败, 错误消息为: %1").arg(clearMsg));
//    } else {
//        updateProgramInfo(m_programInfoTableView, QString("完成Database清空工作"));
//    }

//    QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);
//    if (compTableMsg != "SUCCESS") {
//        updateProgramInfo(m_errorMsgTableView, QString("补完Database 失败, 错误消息为: %1").arg(compTableMsg));
//    } else {
//        updateProgramInfo(m_programInfoTableView, QString("根据策略股票代码补全Database"));
//    }
//}

//void RealTimeDataRead::setPreCloseData() {
//    QString tableName = "PreCloseData";
//    QList<QString> tableList = m_realtimeDatabase->getTableList(m_realtimeDatabase->getDatabaseName());
//    if (tableList.indexOf(tableName) < 0 ) {
//        m_realtimeDatabase->createPreCloseTable(tableName);
//        QMap<QString, QStringList> precloseData = wsqPreCloseData();
//        QList<QString> secodeList = precloseData.keys();
//        for (int i = 0; i < secodeList.size(); ++i) {
//            QString secode = secodeList[i];
//            m_realtimeDatabase->insertPreCloseData(tableName, precloseData[secode]);
//        }
//        updateProgramInfo(m_programInfoTableView, QString("取得昨收数据"));
//    }
//}

//void RealTimeDataRead::getPreCloseData(QList<QString> secodeList) {
//    int errcode;
//    if (m_login) {
//        WindData wd;
//        LPCWSTR windcodes = transSecode(secodeList);
//        LPCWSTR indicators = TEXT("pre_close");
//        LPCWSTR options = TEXT("Days=Alldays");
//        LPCWSTR beginDate = transSecode(QDate::currentDate().toString("yyyy-MM-dd"));
//        bool bOutputMsg = false;
//        errcode = CWAPIWrapperCpp::wsd(wd, windcodes, indicators, beginDate, beginDate, options);
//        if (bOutputMsg) qDebug() << "wsd errcode: " << errcode;
//        if (errcode == 0) {
//            QMap<QString, QStringList> result;
//            int codelen = wd.GetCodesLength();
//            int fieldslen = wd.GetFieldsLength();
//            int colnum = fieldslen + 1;
//            if (bOutputMsg) cout  << "WindCodes    ";
//            for (int i =1;i < colnum; ++i)
//            {
//                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
//                if (bOutputMsg) cout << outfields.toStdString() << "    ";
//            }
//            if (bOutputMsg) cout << endl;
//            for (int i = 0; i < codelen; ++i)
//            {
//                QStringList singleCodeData;
//                QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
//                if (bOutputMsg) cout << codes.toStdString() << "    ";
//                for (int j = 0; j < fieldslen; ++j)
//                {
//                    VARIANT var;
//                    wd.GetDataItem(0,i,j,var);
//                    QString temp = variantToQString(&var);
//                    singleCodeData.append(temp);
//                    if (bOutputMsg) cout << temp.toStdString() << "    ";
//                }
//                result.insert(codes, singleCodeData);
//            }

//            emit writePreCloseData_signal(result);

//        } else {

//        }
//        delete[] beginDate;
//    } else {
//        errcode = -1;
//    }
//}
