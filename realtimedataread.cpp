#include <QDebug>
#include <iostream>
#include <QDate>
#include <QThread>
#include <windows.h>
#include <QTime>
#include <algorithm>
#include <QFileInfo>
#include <database.h>
#include <QCoreApplication>

using namespace std;
#include "WAPIWrapperCpp.h"
#include "toolfunc.h"
#include "realtimedataread.h"
#include "wind_func.h"
#include "secode_func.h"
#include "excel_func.h"
#include "time_func.h"
#include "widget_func.h"
#include "process_data_func.h"
#include "io_func.h"


RealTimeDataRead::RealTimeDataRead(QTableView* programInfoTableView,
                                   QTableView* errorMsgTableView,
                                   QString dbConnID, QString dbHost,
                                   QObject* parent):
    m_programInfoTableView(programInfoTableView), m_errorMsgTableView(errorMsgTableView),
    m_dbConnID(dbConnID), m_dbHost(dbHost),
    QObject(parent)
{

    initCommonData();
    initDatabase();
    initTimer();
    initMonitorTimer();

//    testSecodeList();
}
void RealTimeDataRead::initCommonData() {
    m_currProccessState = false;
    m_isTooEarly = false;
    m_isRestTime = false;
    m_login = false;
    m_strategyFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/strategy/");
    m_wsqSnapshootDataNumb = 500;

    m_futureIndexCode = "000300";
    m_secodeList_IndexCode = "000906";
    m_usefulReadRate = 0.5;

    m_minReadWaitTime = 3500;
    m_signalWriteTime = 2.5;

    m_isDev = false;
}

void RealTimeDataRead::initDatabase() {
    m_dbName = "Market_Info";
    m_realtimeDatabase = new RealTimeDatabase(m_dbConnID, m_dbName, m_dbHost);
}

void RealTimeDataRead::resetDatabase() {
    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
    m_realtimeDatabase = new RealTimeDatabase(m_dbConnID, m_dbName, m_dbHost);
}

void RealTimeDataRead::initTimer() {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(setRealTimeData_slot()));
}

void RealTimeDataRead::testSecodeList() {
    m_secodeList = getSecodeList();
    m_futureList = getFutureList();
//    qDebug() << "m_secodeList: " << m_secodeList;
//    qDebug() << "m_futureList: " << m_futureList;
    updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("m_secodeList.size: %1, m_futureList.size: %2")
                                                .arg(m_secodeList.size()).arg(m_futureList.size()));
}

void RealTimeDataRead::setUpdateTime() {
    m_updateTime = max(m_minReadWaitTime, int(m_signalWriteTime * m_secodeList.size()));
}

void RealTimeDataRead::startTimer() {
    if (!m_timer.isActive()) {
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("开启读取定时器，读取时间频率为: %1 毫秒")
                                                  .arg(m_updateTime));
        m_timer.start(m_updateTime);
    }
}

void RealTimeDataRead::stopTimer() {
    if (m_timer.isActive()) {
        m_timer.stop();
    }
}

void RealTimeDataRead::initMonitorTimer() {
    m_monitorExceptionWaitTime = 10000;
    connect(&m_monitorExceptionTimer, SIGNAL(timeout()),
            this, SLOT(monitorException_slot()));

    updateProgramInfo(m_programInfoTableView,
                      QString::fromLocal8Bit("读取异常监听时间为: %1 毫秒")
                      .arg(m_monitorExceptionWaitTime));
}

void RealTimeDataRead::startMonitorTimer() {
    m_currProccessState = false;
    m_monitorExceptionTimer.start(m_monitorExceptionWaitTime);
    qDebug() << "\n";
    qDebug() << QString::fromLocal8Bit("开启了读取监听器, 剩余时间: %1")
                .arg(m_monitorExceptionTimer.remainingTime());
}

void RealTimeDataRead::stopMonitorTimer() {
    m_currProccessState = true;
    if (m_monitorExceptionTimer.isActive()) {
        m_monitorExceptionTimer.stop();
        qDebug() << QString::fromLocal8Bit("关闭了读取监听器, 剩余时间: %1")
                    .arg(m_monitorExceptionTimer.remainingTime());
    }
}

void RealTimeDataRead::monitorException_slot() {
     qDebug() << "Read::monitorException_slot, Time: " << QTime::currentTime();
    if (false == m_currProccessState) {
        setRealTimeData_slot();
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("此次读取失败, 已经超时."));
    } else {
       updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("此次读取成功"));
    }
}

void RealTimeDataRead::loginWind_slot() 
{
    int errcode = 0;

    if (!m_isDev)
    {
        errcode = CWAPIWrapperCpp::start();
    }
    
    if (0 == errcode) {

        m_login = true;
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("登陆万得成功"));

        m_secodeList = getSecodeList();
        m_futureList = getFutureList();
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("股票代码数: %1, 期货合约数: %2")
                                                    .arg(m_secodeList.size()).arg(m_futureList.size()));
        setUpdateTime();
        emit setTableList_signal(m_secodeList, m_futureList);

        emit loginWindSucc_signal();
    }else {
        WCHAR buffer[128];
        int bufferSize = 128;
        CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
        QString errorMsg = QString::fromWCharArray(buffer);
        updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("登录万得失败, 错误信息为: %1").arg(errorMsg));
        emit loginWindFailed_signal();
    }
}

QList<QString> RealTimeDataRead::getExcelSecodeList() 
{
    QList<QFileInfo> strategyFileInfoList = getExcelFileInfo(m_strategyFileDir);
    QList<QString> result;
    for (QFileInfo currFileInfo : strategyFileInfoList) 
    {
        QList<QString> secodeList = readExcelSecodeList(m_strategyFileDir + currFileInfo.fileName(), "Sheet1", 1, "wind");
        for (QString secode : secodeList) 
        {
            if (result.indexOf(secode) < 0) 
            {
                result.append(secode);
            }
        }
    }

    // printList(result, "excelFileCodeList");

/*
    QString indexFileName = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("./info.xlsx");
    QList<QString> indexCodeList = readExcelSecodeList(indexFileName, "indexList");
    for (QString indexCode : indexCodeList)
    {
        if (result.indexOf(getCompleteIndexCode(indexCode, "wind")) < 0)
        {
            result.append(getCompleteIndexCode(indexCode, "wind"));
        }
    }
*/    
    return result;
}

QList<QString> RealTimeDataRead::getSecodeList() 
{    
    QString tableName = getCompleteIndexCode(m_secodeList_IndexCode) + "_SecodeList";
    QList<QString> result;
    result = m_realtimeDatabase->getSecodeList(tableName);
    for (int i = 0; i < result.size(); ++i) 
    {
        result[i] = completeSecode(result[i], "wind");
    }

    if (result.indexOf("000303.SH") > 0)
    {
        qDebug() << "000303.SH is IN 800";
    }

    updateProgramInfo(m_programInfoTableView, 
                    QString::fromLocal8Bit("%1股票代码数: %2").arg(m_secodeList_IndexCode).arg(result.size()));

    QList<QString> excelList = getExcelSecodeList();

    updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("所有组合股票代码数: %1").arg(excelList.size()));

    QList<QString> excelOnlyCodeList;

    for (QString secode : excelList) 
    {
        if (result.indexOf(secode) < 0) 
        {
            result.append(secode);
            excelOnlyCodeList.append(secode);
        }
    }

    if (result.indexOf("000303.SH") > 0)
    {
        qDebug() << "000303.SH is IN EXCEL";
    }

    // printList(excelOnlyCodeList, "excelOnlyCodeList");


    QList<QString> indexCodeList = getIndexCode("wind");
    printList(indexCodeList, "indexCodeList");

    for (QString indexCode: indexCodeList)
    {
        if (result.indexOf(indexCode) < 0)
        {
            result.append(indexCode);
        }
    }

    if (result.indexOf("000303.SH") > 0)
    {
        qDebug() << "000303.SH is IN IndexList";
    }    
    return result;
}

QList<QString> RealTimeDataRead::getFutureList() {
    QList<QString> result = m_realtimeDatabase->getAllData(m_futureIndexCode, "Future_Info");
    return result;
}

void RealTimeDataRead::setTableListComplete_slot() 
{
    setPreCloseData();
}

void RealTimeDataRead::setPreCloseData() {
    QMap<QString, QStringList> result = wsqPreCloseData();
    checkPreCloseData(result);

    emit writePreCloseData_signal(result);
}

void RealTimeDataRead::checkPreCloseData(QMap<QString, QStringList>& oriData)
{
    for (QMap<QString, QStringList>::const_iterator it = oriData.begin();
        it != oriData.end(); ++it)
    {
        if (it.value()[1] == "nan")
        {
            qDebug() << it.key() << it.value();
            oriData.remove(it.key());
            m_secodeList.removeOne(it.key());            
        }
    }
}

void RealTimeDataRead::setPrecloseDataComplete_slot() {
    emit startMonitorTimer_signal();

    setRealTimeData_slot();
}

void RealTimeDataRead::resetReadSource() {
    m_secodeList = getSecodeList();
    m_futureList = getFutureList();
    updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("股票代码数: %1, 期货合约数: %2")
                                                .arg(m_secodeList.size()).arg(m_futureList.size()));
    emit setTableList_signal(m_secodeList, m_futureList);
    m_isTooEarly = false;
    m_isRestTime = false;
}

void RealTimeDataRead::writeComplete_slot(int flag) {
    qDebug() << "WriteComplete flag: " << flag;
    emit startMonitorTimer_signal();

    setRealTimeData_slot();
}

void RealTimeDataRead::setRealTimeData_slot() {
    bool isTest = false;

    if (isTest)
    {
        QMap<QString, QStringList> secodeResult = getSnapshootData();
        QMap<QString, QStringList> futureResult = wsqFutureData(m_futureList);
        if (secodeResult.size() > m_secodeList.size() * m_usefulReadRate) {
                emit writeRealTimeData_signal(secodeResult, futureResult);
                emit stopMonitorTimer_signal();
       } else {
            setRealTimeData_slot();
       }
    }
    else
    {
        if (isStockTrading()) {
            QMap<QString, QStringList> secodeResult = getSnapshootData();
            QMap<QString, QStringList> futureResult = wsqFutureData(m_futureList);
            if (secodeResult.size() > m_secodeList.size() * m_usefulReadRate)
            {
                emit writeRealTimeData_signal(secodeResult, futureResult);
                emit stopMonitorTimer_signal();
            }
            else
            {
                setRealTimeData_slot();
            }
            emit stopWaitTradeTimer_signal();
        } else {
            emit stopMonitorTimer_signal();
        }

        if (isStockTradingNotStart() && !m_isTooEarly){
            m_isTooEarly = true;
            updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("时间太早"));

            emit startWaitTradeTimer_signal();
        }

        if (isStockNoonBreak() && !m_isRestTime) {
            updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("午休时间"));
            m_isRestTime = true;

            emit startWaitTradeTimer_signal();
        }

        if (isStockTradingOver()) {
            emit stopWaitTradeTimer_signal();
            waitForNextTradingDay(m_programInfoTableView);
            resetReadSource();
        }
    }
}

QMap<QString, QStringList> RealTimeDataRead::getSnapshootData() 
{
    QMap<QString, QStringList> result;
    for(int i = 0; i < m_secodeList.size(); i+= m_wsqSnapshootDataNumb) {
        QList<QString> subSecodeList = getSubList(m_secodeList, i, i + m_wsqSnapshootDataNumb);
        result.unite(wsqSnapshootData(subSecodeList));
    }
    if (result.size() ==  m_secodeList.size()) {
    //        qDebug() << "getSnapshootData Successfully, result.size: " << result.size();
    }
    return result;
}

QMap<QString, QStringList> RealTimeDataRead::wsqSnapshootData(QList<QString> secodeList) 
{
    bool bOutputMsg = false;
    QMap<QString, QStringList> result;
    int errcode;
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecodeList(secodeList);
        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_latest,rt_pre_close,rt_amt,rt_open");
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
             updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取实时数据失败, 错误代码: %1").arg(errcode));
        }
        delete[] windcodes;
    } else {
        updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取实时数据失败, 还没登录wind"));
    }
    return result;
}

QMap<QString, QStringList> RealTimeDataRead::wsqFutureData(QList<QString> futureList) {
    bool bOutputMsg = false;
    QMap<QString, QStringList> result;
    int errcode;
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecodeList(futureList);
        LPCWSTR indicators = TEXT("rt_spread,rt_pre_oi,rt_oi,rt_oi_chg,rt_oi_change, \
                                  rt_nature,rt_pre_settle,rt_settle,rt_est_settle");
        LPCWSTR options = TEXT("");
        errcode = CWAPIWrapperCpp::wsq(wd, windcodes, indicators, options);
        if (bOutputMsg) qDebug() << "startWsqOneTime errcode: " << errcode;
        if (errcode == 0) {
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            if (bOutputMsg) cout << "codelen: " << codelen << ", fieldslen: " << fieldslen << endl;
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
             updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取实时数据失败, 错误代码: %1").arg(errcode));
        }
        delete[] windcodes;
    } else {
        updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取实时数据失败, 还没登录wind"));
    }
    return result;
}

QMap<QString, QStringList> RealTimeDataRead::wsqPreCloseData() 
{
    int errcode;
    QMap<QString, QStringList> result;
    if (m_login) {
        WindData wd;
        LPCWSTR windcodes = transSecodeList(m_secodeList);
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
            updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("获取前收数据成功."));
        } else {
            updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取前收数据失败, 错误代码为: %1").arg(errcode));
        }
        delete[] beginDate;
    } else {
         updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取前收数据失败, 还没登录wind"));
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
//        errcode = CWAPIWrapperCpp::wset(wd,L"sectorconstituent",L"date=2018-03-09;sectorid=a001010100000000");

        qDebug() << "conditionStr: " << conditionStr;

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
            WCHAR buffer[128];
            int bufferSize = 128;
            CWAPIWrapperCpp::getErrorMsg(errcode, eCHN, buffer, bufferSize);
            cout << buffer << endl;
            std::wcout.imbue(std::locale("chs"));
            std::wcout << buffer << std::endl;
            updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取股票代码列表失败, 错误代码: %1").arg(errcode));
        }
        delete[] conditions;
    } else {
         updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("获取股票代码列表失败, 还没登录wind"));
    }
    return result;
}

RealTimeDataRead::~RealTimeDataRead() {
    emit stopMonitorTimer_signal();

    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
}

//bool testTimeStartTime(QTime time) {
//    QTime pmStopTime = QTime(10, 25, 0);
//    if (time <= pmStopTime){
//        return true;
//    } else {
//        return false;
//    }
//}

//void RealTimeDataRead::setRealTimeData_slot() {
//    qDebug() << "RealTimeDataRead::setRealTimeData_slot ";
//    if (testTimeStartTime(QTime::currentTime())) {
//        QTime pmStopTime = QTime(10, 25, 0);
//        QTime curTime = QTime::currentTime();
//        int sleepTime = QDateTime(QDate::currentDate(), pmStopTime).toMSecsSinceEpoch() -
//                        QDateTime::currentDateTime().toMSecsSinceEpoch();
//        qDebug() << "sleep time: " << sleepTime;
//        updateProgramInfo(m_programInfoTableView, QString("Sleep %1 ms").arg(sleepTime));
//        QThread::msleep(sleepTime);
//        setSecodeList();
//        setPreCloseData();
//        return;
//    }

//    if (isTradingTime(QTime::currentTime())) {
//        QMap<QString, QStringList> result = getSnapshootData();
//        if (result.size() != 0) {
//            emit writeRealTimeData_signal(result);
//            if (m_timer.isActive()) {
//                qDebug() << "emit stopWaitTradeTimer_signal();";
//                emit stopWaitTradeTimer_signal();
//            }
//        }
//    } else if (!isTradingStart()){
//        if (!m_isTooEarly) {
//            m_isTooEarly = true;
//            updateProgramInfo(m_programInfoTableView, QString("时间太早"));
//            if (!m_timer.isActive()) {
//                qDebug() << "emit startWaitTradeTimer_signal();";
//                emit startWaitTradeTimer_signal();
//            }
//        }
//    } else {
//        if (!m_isRestTime) {
//            updateProgramInfo(m_programInfoTableView, QString("午休时间"));
//            m_isRestTime = true;
//            if (!m_timer.isActive()) {
//                qDebug() << "emit startWaitTradeTimer_signal();";
//                emit startWaitTradeTimer_signal();
//            }
//        }
//    }
//}

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
