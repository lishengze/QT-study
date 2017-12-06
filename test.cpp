#include <QList>
#include <QPointF>
#include <QDebug>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include <QProcess>
#include "test.h"
#include "setdata.h"
#include "toolfunc.h"
#include "strategy.h"
#include "tabledata.h"
#include "excel.h"
#include "macd.h"
#include "database.h"

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
    qDebug() << oriData;
    int t1 = 12;
    int t2 = 26;
    int t3 = 9;
    QList<MACD> result = computeMACD (oriData, t1, t2, t3);

    for (int i =0; i < result.size (); ++i) {
        cout << result.at(i);
    }
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
    int dataNumb = 5;
    int interval = 3;
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

void testMain() {
    //    testReadExcelData ();
    //    testGetExcelFileName();
    //    testRemovePathName();
    //    testExcel();
    //    testReadStrategyDataFromExcel();
        testSortPointList ();
        testMergeSortedList();
    //    testTableData();
    //    testComputeMACD ();
    //    testTranstime();
    //    testGetNumbList();
    //    testConnectDatabase();
    //    testDBClass();
    //    testProcess();
    //    testKillProcess();
}



























