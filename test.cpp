#include <QList>
#include <QPointF>
#include <QDebug>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include <QProcess>
#include <QStringList>
#include <string>
#include <QDate>
#include <QDesktopServices>
#include <QSqlQuery>
#include <QUrl>

#include "test.h"
#include "toolfunc.h"
#include "macd.h"
#include "database.h"
#include "dialog.h"
#include "realtimedatabase.h"
#include "excel_func.h"
#include "compute_func.h"
#include "io_func.h"
#include "process_data_func.h"
#include "secode_func.h"
#include "terminal_func.h"
#include "time_func.h"
#include "indexchart.h"
#pragma execution_character_set("utf-8")

using namespace  std;

void testReadExcelData () {
    QString excelFilePath = "D:/strategy/test.xlsx";
//    readBasicDataFromExcel(excelFilePath);
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
//    connectSql (server, port, dbName, userName, userPwd);
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
    transDateTime(oriDatetime);
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
//    RealTimeDatabase realtimeDatabaseObj = RealTimeDatabase("0", "192.168.211.165", "sa", "sasa", "MarketData_RealTime");
//    RealTimeDatabase realtimeDatabaseObj = RealTimeDatabase();
//    QString testTableName = "test3";
//    realtimeDatabaseObj.createTable(testTableName);

//    QList<QString> data;
//    realtimeDatabaseObj.insertData(dbName, testTableName, data);
}

void testOpenUrl() {
    QDesktopServices::openUrl(QUrl("http://www.baidu.com"));
}

void testGetDirName() {
     QString accountFatherDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/");
     qDebug() << "accountFatherDir: " << accountFatherDir;
     QList<QFileInfo> dirInfoList = getDirName(accountFatherDir);
     for (int i = 0; i < dirInfoList.size(); ++i) {
         qDebug() << dirInfoList[i].fileName();
     }
}

void testDatabaseBasic() {
    Database databaseObj;

//    databaseObj.getClosePrice("000001", "realtime");

    QList<QString> secodeList;
    secodeList << "000001.SZ";
    QMap<QString, QStringList> result = databaseObj.getSnapShootData(secodeList);
    qDebug() << result;
}

void testXlsx() {
    QString fileName = QString::fromLocal8Bit("//192.168.211.182/it程序设计/生成组合/Z策略/策略1_因子学习.xlsx");
    readExcelData(fileName);
}

void testGetFutureSpread() {
    QString host = "192.168.211.165";
    Database databaseObj("0", host);
    QString futureName = "IF1804.CFE";
    QList<double> realTimeResult = databaseObj.getFutureSpread(futureName);
    qDebug() << "realTimeResult: " << realTimeResult;

    QList<double> histResult = databaseObj.getHistFutureSpread(futureName);
    qDebug() << "histResult: " << histResult;
}

void testTransIntDateTime() {
    double oriTime = 20180414113400;
    qDebug() << "TransDateTime: " << transIntDateTime(oriTime);
}

void testRealTimeData() {
   QString dbConnId = "0";
   QString dbhost = "192.168.211.162";
   Database databaseObj(dbConnId, dbhost);

//   QString secode = "000001.SZ";
//   QMap<QString, QStringList> result = databaseObj.getSnapShootData(secode);

   QStringList secodeList;
   secodeList << "000001.SZ" << "000002.SZ" << "000006.SZ";
   QMap<QString, QStringList> result = databaseObj.getSnapShootData(secodeList);

   printMap(result, "SnapShootData");
}

void testGetOneDayTimeList() {
    QString dbConnId = "0";
    QString dbhost = "192.168.211.162";
    Database databaseObj(dbConnId, dbhost);
    QString databaseName = "MarketData_5m";
    QString tableName = "SH000300";
    QList<int> timeList = databaseObj.getOneDayTimeList(tableName, databaseName);
    printList(timeList, databaseName + " TimeList: ");
}

void testTransRealTimeDataToMinuteData() {
    QString dbConnId = "0";
    QString dbhost = "192.168.211.162";
    Database databaseObj(dbConnId, dbhost);
    QString databaseName = "MarketData_5m";
    QStringList todayKeyValueList;
    todayKeyValueList << "日期" << "时间" << "最新成交价";
    QString secode = "000300";

    QList<int> standardTimeList = databaseObj.getOneDayTimeList(getCompleteIndexCode(secode),
                                                                databaseName);
    //    printList(standardTimeList, "standardTimeList");

    QList<QStringList> oriData = databaseObj.getAllRealtimeData(getCompleteIndexCode(secode, "wind"),
                                                                todayKeyValueList);
    //    printList(oriData, "oriData");

    QList<QStringList> transResult = transRealTimeDataToMinuteData(oriData, standardTimeList);
    printList(transResult, "transResult");
}

void testGetRelativeProfit() {
    QString dbConnId = "0";
    QString dbhost = "192.168.211.162";
    Database databaseObj(dbConnId, dbhost);
    QString databaseName = "MarketData_15m";
    QString hedgedSecode = "000300";
    QString selectSecode = "000913";
    QString startDate = "20180507";
    QString endDate = "20180713";

    QStringList histKeyValueList;
    histKeyValueList << "TDATE" << "TIME" << "TCLOSE";
    QList<QStringList> selectIndexHistData = databaseObj.getDataByDate(startDate, endDate, histKeyValueList,
                                                                      getCompleteIndexCode(selectSecode), databaseName);
    QList<QStringList> hedgedIndexHistData = databaseObj.getDataByDate(startDate, endDate, histKeyValueList,
                                                                      getCompleteIndexCode(hedgedSecode), databaseName);
    //    printList(selectIndexHistData, "selectIndexHistData");
    //    printList(hedgedIndexHistData, "hedgedIndexHistData");

    QList<QStringList> profiltList = getRelativeProfitList(selectIndexHistData, hedgedIndexHistData);
//    printList(profiltList, "profiltList");

    QList<QStringList> cssList = getCSSList(profiltList, 32, 0.014, 1.0/3.0, 2.0/3.0, 300, -300);
//    printList(cssList, "cssList");

//    if (selectIndexHistData.size() != hedgedIndexHistData.size()) {
//        QString message = "选中的指数与对冲指数历史数据不完整";
//        qDebug() << message;
//    } else {
//        QStringList todayKeyValueList;
//        todayKeyValueList << "日期" << "时间" << "最新成交价";
//        QList<QStringList> selectIndexTodayData = databaseObj.getAllRealtimeData(getCompleteIndexCode(selectSecode, "wind"),
//                                                                                 todayKeyValueList);
//        QList<QStringList> hedgedIndexTodayData = databaseObj.getAllRealtimeData(getCompleteIndexCode(hedgedSecode, "wind"),
//                                                                                 todayKeyValueList);
//    //        printList(selectIndexTodayData, "selectIndexTodayData");
//    //        printList(hedgedIndexTodayData, "hedgedIndexTodayData");

//        QList<int> standardTimeList = databaseObj.getOneDayTimeList(getCompleteIndexCode(hedgedSecode), databaseName);
//    //        printList(standardTimeList, "standardTimeList");

//        QList<QStringList> selectIndexTransData = transRealTimeDataToMinuteData(selectIndexTodayData, standardTimeList);
//        QList<QStringList> hedgedIndexTransData = transRealTimeDataToMinuteData(hedgedIndexTodayData, standardTimeList);

//    //        printList(selectIndexTransData, "selectIndexTransData");
//    //        printList(hedgedIndexTransData, "hedgedIndexTransData");

//        resizeMinuteData(selectIndexTransData, hedgedIndexTransData);

//        QList<QStringList> profiltList = getRelativeProfitList(selectIndexHistData + selectIndexTransData,
//                                                               hedgedIndexHistData + hedgedIndexTransData);
//        printList(profiltList, "profiltList");
//    }
}

void testIndexChart() {
    int dbConnId = 0;
    QString dbhost = "192.168.211.162";
    QString databaseName = "MarketData_15m";
    QString hedgedSecode = "000300";
    QString selectSecode = "000913";
    QString startDate = "20180507";
    QString endDate = "20180713";
    int     aveNumb = 32;
    double  css12Rate = 0.014;
    double  cssRate1 = 1.0/3.0;
    double  cssRate2 = 2.0/3.0;
    double  maxCSS = 300;
    double  minCSS = -300;
//    IndexChart indexChartWindow(dbConnId, dbhost, databaseName,
//                                selectSecode, hedgedSecode, startDate, endDate,
//                                aveNumb, css12Rate, cssRate1, cssRate2,
//                                maxCSS, minCSS);
//    indexChartWindow.setWindowTitle("测试指数对冲图");
//    indexChartWindow.show();
}

void testMean() {
    QList<double> oriData;
    int numb = 10;
    for (int i = 0; i < numb; ++i) {
        oriData.append(i);
    }
//    double meanData = mean(oriData, 0, oriData.size());
    double meanData = getAveValue(oriData, 0, oriData.size());
    qDebug() << oriData;
    qDebug() << "meanData: " << meanData;
}

void testMain() {
    //        testReadExcelData ();
    //        testGetExcelFileName();
    //        testRemovePathName();
    //        testExcel();
    //        testReadStrategyDataFromExcel();
    //        testSortPointList ();
    //        testMergeSortedList();
    //        testTableData();
    //        testComputeMACD ();
    //        testTranstime();
    //        testGetNumbList();
    //        testConnectDatabase();
    //        testDBClass();
    //        testProcess();
    //        testKillProcess();
    //    testDatabase();
    //    testDataRead();
    //    testComputeMACD();e
    //    testTransSecode();
    //    testComputeMACDDoubleData();
    //    testWindWsq();
    //    testGetSnapShootData();
    //    testTransDateTime();
    //    testGetAnnounce();
    //    testGetTableList();
    //    testRealtimeDatabaseApi();
    //    testOpenUrl();
    //    testGetDirName();
    //    testDatabaseBasic();
    //    testXlsx();
    //    testGetFutureSpread();
    //    testTransIntDateTime();
    //    testRealTimeData();
    //    testGetOneDayTimeList();
    //    testTransRealTimeDataToMinuteData();
//    testGetRelativeProfit();
//    testIndexChart();
    testMean();
}

