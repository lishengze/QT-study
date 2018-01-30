#include <QList>
#include <QPointF>
#include <QDebug>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include <QProcess>
#include <QStringList>
#include <string>

#include "test.h"
#include "setdata.h"
#include "toolfunc.h"
#include "strategy.h"
#include "tabledata.h"
#include "excel.h"
#include "macd.h"
#include "database.h"
#include "dialog.h"

#include <WinNls.h>

#include "WAPIWrapperCpp.h"
#include "realtimedatabase.h"

using namespace  std;

void testReadExcelData () {
    QString excelFilePath = "D:/strategy/test.xlsx";
//    readBasicDataFromExcel(excelFilePath);
    readStrategyDataFromExcel(excelFilePath);
}

void testGetExcelFileName() {
    QString dirName = "E:/github/study/QT/Creator/chapter4/layout";
//    dirName = "E:/DataBase/original-data-20160910-20170910-1m";
    QList<QString> fileNameList = getExcelFileName(dirName);
    qDebug() << fileNameList;
}

void testRemovePathName () {
    QString fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
    QString pureFileName = removePathName (fullFileName);
    qDebug() << "FullFileName: " << fullFileName;
    qDebug() << "PureFileName: " << pureFileName;
}

void testReadStrategyDataFromExcel() {
    QString fullFileName = "E:/github/work-program/client/client-qt/test.xlsx";
    QList<strategy_ceil> m_currStrategy = readStrategyDataFromExcel (fullFileName);
    for (int i = 0; i < m_currStrategy.size(); ++i) {
        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
    }
}

void testExcel() {
    Excel excelObj;
    QString fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
    QList<strategy_ceil> m_currStrategy = excelObj.readStrategyDataFromExcel (fullFileName);
    for (int i = 0; i < m_currStrategy.size(); ++i) {
        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
    }

    fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test1.xlsx";
    m_currStrategy = excelObj.readStrategyDataFromExcel (fullFileName);
    for (int i = 0; i < m_currStrategy.size(); ++i) {
        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
    }
}

void testSortPointList() {
    QList<QPointF> oriData;
    int dataNumb = 5;
    for (int i = 0; i < dataNumb; ++i) {
        oriData.append (QPointF(dataNumb-i, i));
    }
    qDebug() << "oridata: " << oriData;

    QList<QPointF>sortedData = sortPointFList(oriData);
    qDebug() << "sortedData: " << sortedData;
}

void testMergeSortedList() {
    QList<QPointF> firstList;
    QList<QPointF> secondList;
    int numb = 10;
    int index = -5;
    for (int i = 0; i<numb; ++i) {
        firstList.append (QPointF(i, i));
        secondList.append (QPointF(i + index, i));
    }
    qDebug() << "firstList: " << firstList;
    qDebug() << "secondList: " << secondList;

    QList<QPointF> mergedList = mergeSortedPointedList (firstList, 1, secondList, 1);
    qDebug() << "mergedList: " << mergedList;
}

void testTableData () {
    TableData tmpdata(20170111, 110911, 13.0);
    cout << tmpdata;
}

void testComputeMACD() {
//    double data[] = {99.9800, 104.3900, 109.1300,114.5500,
//                    120.2500,125.2700,125.2800,126.4500,
//                    127.6100,128.8400,130.1400,131.4400,
//                    132.0600,132.6800,133.3400,133.9700,
//                    134.6000,134.6700,134.7400,134.2400,
//                    134.2500,134.2400,134.2400,133.7200,
//                    133.1700,132.6100,132.0500,131.4600,
//                    130.9500,130.4400,129.9700,129.5100,
//                    129.0500,128.5800,129.1400,129.7900,
//                    130.3800};
    double data[] = {-0.5, -0.5, -0.5, -0.5};
    QList<double> oriData;
    int numb = sizeof(data) / sizeof(double);
    for (int i = 0; i < numb; ++i) {
        oriData.append (data[i]);
    }
    int t1 = 12;
    int t2 = 26;
    int t3 = 9;
    QList<MACD> result = computeMACD (oriData, t1, t2, t3);

    for (int i =0; i < result.size (); ++i) {
        cout << result.at(i);
    }
    cout<< "result.size: " << result.size ();
}

void testComputeMACDDoubleData() {
    double data[] = {99.9800, 104.3900, 109.1300,114.5500,
                    120.2500,125.2700,125.2800,126.4500,
                    127.6100,128.8400,130.1400,131.4400,
                    132.0600,132.6800,133.3400,133.9700,
                    134.6000,134.6700,134.7400,134.2400,
                    134.2500,134.2400,134.2400,133.7200,
                    133.1700,132.6100,132.0500,131.4600,
                    130.9500,130.4400,129.9700,129.5100,
                    129.0500,128.5800,129.1400,129.7900,
                    130.3800};
    QList<double> oriData;
    int numb = sizeof(data) / sizeof(double);
    for (int i = 0; i < numb; ++i) {
        oriData.append (data[i]);
    }
    int t1 = 12;
    int t2 = 26;
    int t3 = 9;
    QList<double> doubleResult = computeMACDDoubleData (oriData, t1, t2, t3);
    QList<MACD> classResult = computeMACD (oriData, t1, t2, t3);

    for (int i = 0; i < classResult.size (); ++i) {
        cout << classResult[i];
        cout << doubleResult[i*5] << ", " << doubleResult[i*5+1] << ", " << doubleResult[i*5+2] << ", "
             << doubleResult[i*5+3] << ", " << doubleResult[i*5+4] << "\n" << endl;

        if (classResult[i].m_ema1 != doubleResult[i*5] ||
            classResult[i].m_ema2 != doubleResult[i*5+1] ||
            classResult[i].m_diff != doubleResult[i*5+2] ||
            classResult[i].m_dea != doubleResult[i*5+3] ||
            classResult[i].m_macd != doubleResult[i*5+4]) {
            qDebug() << "Differernt ";
        }
    }

//    for (int i = 0; i < doubleResult.size (); i+= 5) {
//        cout << doubleResult[i] << ", "
//             << doubleResult[i+1] << ", "
//             << doubleResult[i+2] << ", "
//             << doubleResult[i+3] << ", "
//             << doubleResult[i+4] << endl;
//    }
//    cout << "doubleResult.size: " << doubleResult.size ();
}

void testTranstime() {
    QDateTime xValue;
    int year = 2017;
    int month = 11;
    int day = 28;
    int hour = 16;
    int min = 31;
    int secs = 20;
    xValue.setDate (QDate(year, month, day));
    xValue.setTime (QTime(hour, min, secs));

    qint64 offSecs = xValue.toMSecsSinceEpoch();

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(offSecs);

    qDebug() << "oriDateTime: " << xValue;
    qDebug() << "offSecs: " << offSecs;
    qDebug() << "transedTime: " << dateTime;
}

void testGetNumbList() {
    int dataNumb = 14;
    int interval = 5;
    qDebug() << "dataNumb: " << dataNumb;
    qDebug() << "interval: " << interval;
    qDebug() << "List: " << getNumbList(dataNumb, interval);
}

void testConnectDatabase() {
    QString server = "192.168.211.165";
    QString port = "1433";
    QString dbName = "master";
    QString userName = "sa";
    QString userPwd = "sasa";
    connectSql (server, port, dbName, userName, userPwd);
}

void testDBClass() {
    QString connName = "0";
    QString server = "192.168.211.165";
    Database dbObj = Database(connName, server);

//    QSqlDatabase sqlDb = dbObj.getDatabase ();

    if(dbObj.m_db.open ()) {
        QSqlQuery query3 = QSqlQuery(dbObj.m_db);
        QString sqlStr = "select * from [TestRemote].[dbo].[BasicInfo]";
        bool c = query3.exec(sqlStr);
        if (c)
        {
            qDebug() << "select data success!";
            while(query3.next())
            {
                qDebug() << query3.value(0).toString ();
                qDebug() << query3.value(1).toString ();
            }
        }
        else
        {
            qDebug() << query3.lastError().text().data();
        }
    }
}

void testProcess () {
    QProcess p(0);
    p.start("cmd", QStringList()<<"/c"<<"ping www.baidu.com");
    p.waitForStarted();
    p.waitForFinished();
    QString strTemp=QString::fromLocal8Bit(p.readAllStandardOutput());
//    QString strTemp=p.readAllStandardOutput();

    qDebug () << strTemp;
}

void testKillProcess() {
    QString pid = "20128";
    killProcessByPid(pid);
}

void testDatabase() {
    Database testDB;
    QString tableName = "SH600000";
    QString databaseName = "MarketData_day";
    QString startDate = "20161221";
    QString endDate = "20170101";
    QStringList keyValueList;
    keyValueList << "TCLOSE" << "VOTRUNOVER";
    QList<QStringList> result = testDB.getOriChartData (startDate, endDate, keyValueList, tableName, databaseName);
    qDebug() << result;
}

void testDataRead () {
    Dialog testDialog(8);
    testDialog.show ();
}

void testTransSecode() {
    QStringList oriData;
    oriData << "SH600000" << "SZ000001" << "SZ000002";
//    qDebug() << "oriData: " << oriData;
    LPCWSTR transData = transSecode(oriData);
    wcout << "transData: " << transData << endl;
//    LPCWSTR lpdata = transData.toStdWString().c_str();
//    qDebug() << "transData: " << transData;
//    wcout << "lpdata: " << lpdata << endl;
//    string data = transData.toStdString();
//    size_t size = data.length();
//    wchar_t *buffer = new wchar_t[size + 1];
//    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
//    buffer[size] = 0;  //确保以 '\0' 结尾
//    wcout << "buffer: " << buffer << endl;
//    LPCWSTR lpdata = buffer;
//    wcout << "lpdata: " << lpdata << endl;
    delete[] transData;
}

void testWindWsq() {
    int errcode = CWAPIWrapperCpp::start();
    qDebug() << "loginErrcode: " << errcode;
    LPCWSTR windcodes = TEXT("600000.SH,000001.SZ,000002.SZ");
    wcout << "wcout windcodes: " << windcodes << endl;
    LPCWSTR indicators = TEXT("rt_date,rt_time,rt_last,rt_pre_close,rt_amt");
    LPCWSTR options = TEXT("");
    ULONGLONG reqid = 0;
    errcode = CWAPIWrapperCpp::wsq(reqid, windcodes, indicators, wsqCallBack, options, TRUE);
    qDebug() << "wsq errcode: " << errcode;
}

void testGetSnapShootData() {
    Database databaseObj;
//    qDebug() << databaseObj.getSnapShootData();
    QList<QString> secodeList;
    secodeList << "000001.SZ" << "000002.SZ" << "000016.SH" << "000300.SH";
    qDebug() << databaseObj.getSnapShootData(secodeList);
//    qDebug() << databaseObj.getSnapShootHistoryData(secodeList);
}

void testTransDateTime() {
    double oriDatetime = 20180104110956.98776;
//    transDateTime(oriDatetime);
}


void testGetAnnounce() {
    QString host = "192.168.211.165";
    Database databaseObj("0", host);
    QList<QString> secodeList;
    secodeList << "000513" << "000877" << "002236";
    QString startDate = QDate::currentDate().toString("yyyyMMdd");
    QString endDate = startDate;
    qDebug() << "startdate: " << startDate;
    QMap<QString, QList<QStringList>> result = databaseObj.getAnnouncement(secodeList, startDate, endDate);
    qDebug() << "result: " << result;
}

void testGetTableList() {
    QString host = "192.168.211.165";
    Database databaseObj("0", host);
    QList<QString> result = databaseObj.getTableList("Announcement");
    qDebug() << "result: " << result;
}

void testRealtimeDatabaseApi() {
    RealTimeDatabase realtimeDatabaseObj = RealTimeDatabase("0", "Test");
    QString testTableName = "test3";
    QList<QString> secodeList;
    secodeList << "000001.SZ" << "600000.SH";

//    realtimeDatabaseObj.completeTable(secodeList);
//    realtimeDatabaseObj.createTable(testTableName);

//    QList<QString> data;
//    realtimeDatabaseObj.insertData(dbName, testTableName, data);
    wsqSnaphootData(secodeList);
}

void testMain() {
    //    testReadExcelData ();
    //    testGetExcelFileName();
    //    testRemovePathName();
    //    testExcel();
    //    testReadStrategyDataFromExcel();
//        testSortPointList ();
//        testMergeSortedList();
    //    testTableData();
    //    testComputeMACD ();
    //    testTranstime();
//        testGetNumbList();
    //    testConnectDatabase();
    //    testDBClass();
    //    testProcess();
    //    testKillProcess();
//    testDatabase();
//    testDataRead();
//    testComputeMACD();
//    testTransSecode();
//    testComputeMACDDoubleData();
//    testWindWsq();
    testRealtimeDatabaseApi();
}



























