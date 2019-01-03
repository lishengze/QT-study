#include "test.h"
#include <QDate>
#include <QDebug>
#include <QDateTime>
#include "database.h"
#include "excel_func.h"
#include "time_func.h"
#include "io_func.h"
#include "readdatabasedata.h"
#include <QThread>

Test::Test()
{
//    IsTradingDay();
//    get_future_data();
//    get_database_data();
//    test_extract_marketdata();
//    test_getWeightData();
//    test_getShortedStartEndDateList();
    test_getDateByDate();
}

void Test::IsTradingDay() {
    QDate testDate = QDate(2018, 3, 24);
    if (isTradingDay(testDate)) {
        qDebug() << testDate << " is trading day";
    } else {
        int dayOfWeek = testDate.dayOfWeek();
        int waitDays = -1;
        if (dayOfWeek > 5) {
            waitDays = 8 - dayOfWeek;
        } else {
            waitDays = 1;
        }
        qDebug() << testDate << " is not trading day, "
                 << "waitDays: " << waitDays;
    }

}

void Test::get_future_data() {
    Database database_obj;
    QList<QString> result = database_obj.getAllData("000300", "Future_Info");
    qDebug() << result;
}

void Test::get_database_data() {
    QDateTime startTime = QDateTime::currentDateTime();
    Database databaseObj("1");

    QString startDate = "20150130";
    QString endDate = "20180417";
//    QString dbhost = "192.168.211.162";
    QString dbhost = "localhost";
    QString databaseName = "MarketData_10m";
//    QString databaseName = "MarketData_day";

    QList<QStringList> indexTimeList = getIndexTimeList(startDate, endDate, databaseName, dbhost);
    qDebug() << "indexTimeList.size: " << indexTimeList.size();
//    qDebug() << indexTimeList[0];
    QStringList indexTimeStrList = getIndexTimeStrList(indexTimeList);
    indexTimeStrList.insert(0, "time/code");
//    qDebug() << "indexTimeStrList: " << indexTimeStrList;


    QStringList keyValueList;
    keyValueList << "TDATE" << "TIME" << "TCLOSE" << "TURNOVER";

    bool testSingle = false;
    bool testMulti = true;

    if (testSingle) {
        QString testTableName = "SH601966";
        QList<QStringList> ori_result = databaseObj.getDataByDate(startDate, endDate, keyValueList,
                                                                  testTableName, databaseName);
        qDebug() << "ori_result.size: " << ori_result.size();
        qDebug() << ori_result[0];

        completeExcelData(ori_result, indexTimeList, keyValueList.size());
        qDebug() << "compResult.size: " << ori_result.size();
        qDebug() << ori_result[0];
    }

    if (testMulti) {
        QString fileName = QString::fromLocal8Bit("D:/excel/沪深300成分股 20170130~20180417 10min收盘价.xlsx");
        QList<QStringList> sumResult;
        sumResult.append(indexTimeStrList);
        QList<QString> secodeList = readExcelSecodeList(fileName, "Sheet1", 2, "tinysoft");
        QString desFileName = QString::fromLocal8Bit("D:/excel/test_cplus_%1_%2.xlsx")
                                .arg(databaseName).arg(secodeList.size());
//        qDebug() << secodeList;
        for (int i = 0; i <secodeList.size(); ++i) {
            QString tableName = secodeList[i];
            QList<QStringList> ori_result = databaseObj.getDataByDate(startDate, endDate, keyValueList,
                                                                      tableName, databaseName);
            completeExcelData(ori_result, indexTimeList, keyValueList.size());
            QList<QString> singleResult = getSingleColData(ori_result, 0);
            singleResult.insert(0, tableName);
            sumResult.append(singleResult);
            qDebug() << QString("%1 done, %2 left.").arg(tableName).arg(secodeList.size()-i-1);
        }
//        writeMatrixData(desFileName, sumResult, "Close", true);
    }

    QDateTime endTime = QDateTime::currentDateTime();
    int costSeconds = (endTime.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()) / 1000;
    qDebug() << "costSeconds: " << costSeconds;
}

void Test::test_extract_marketdata() {
//    QString dbhost = "192.168.211.162";
    QString dbhost = "localhost";
//    QString databaseName = "MarketData_10m";
    QString databaseName = "MarketData_day";
    QString startDate = "20150130";
    QString endDate = "20180417";

    QStringList keyValueList;
    keyValueList << "TCLOSE" << "TURNOVER";
    QString secodeFileName = QString::fromLocal8Bit("D:/excel/沪深300成分股 20170130~20180417 10min收盘价.xlsx");

//    ExtractMarketData extractMarketDataObj(dbhost, dbConnectID, databaseName,
//                                           startDate, endDate,
//                                           secodeFileName, keyValueList);
//    extractMarketDataObj.startReadData();

//    QThread::sleep(20);
}

void Test::test_getWeightData() {
    QString dbhost = "192.168.211.162";
    int dbConnectId = 1;
    QString date = "20180610";
    QString indexCode = "SH000016";
    Database databaseObj(QString("%1").arg(dbConnectId), dbhost);
    QList<QStringList> ori_result = databaseObj.getWeightData(date, indexCode);
    qDebug() << ori_result;
}

void Test::test_getShortedStartEndDateList() {
    int startDate = 20120101;
    int endDate = 20150101;
    QString databaseName = "MarketData_15m";
    QList<QList<int>> shortedStartEndDateList = getShortedStartEndDateList(startDate, endDate, databaseName);

    printList(shortedStartEndDateList, "shortedStartEndDateList");
}

void Test::test_getDateByDate() {
    QString startDate = "20010101";
    QString endDate = "20100101";
    QString tableName = "SH000001";
    QString databaseName = "MarketData_15m";
    Database databaseObj("0", "192.168.211.162");
    QStringList keyValueList;
    keyValueList << "TDATE" << "TIME" << "TCLOSE" << "TOPEN";

    QDateTime startDateTime = QDateTime::currentDateTime();
    databaseObj.getDataByDate(startDate, endDate, keyValueList, tableName, databaseName, true);
    QDateTime endDateTime = QDateTime::currentDateTime();
    qDebug() << QString("Shorted costTime: %1s").arg(startDateTime.secsTo(endDateTime));

    startDateTime = QDateTime::currentDateTime();
    databaseObj.getDataByDate(startDate, endDate, keyValueList, tableName, databaseName, false);
    endDateTime = QDateTime::currentDateTime();
    qDebug() << QString("Ori costTime: %1s").arg(startDateTime.secsTo(endDateTime));
}
