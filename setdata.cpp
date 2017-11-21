#include <iostream>
#include <iomanip>
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include "windows.h"
#include <qDebug>
#include <QList>
#include <QPointF>
#include <QLineSeries>
#include <QDateTime>
#include "setdata.h"
#include "toolfunc.h"
using namespace std;

QSqlDatabase createODBCConn() {
    QSqlDatabase db = QSqlDatabase::addDatabase ("QODBC");
    QString databaseName = QString::fromLocal8Bit ("SqlServer");
    db.setHostName("localhost");
    db.setDatabaseName (databaseName);
    db.setUserName ("sa");
    db.setPassword ("sasa");

    if (!db.open ()) {
        qDebug() << "Open database: " + databaseName  + " failed!";
        qDebug() << db.lastError ();
    } else {
        qDebug() << "Open database: " + databaseName  + " successfully!";
    }
    return db;
}

QList<QPointF> getOpenPrice() {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     int x = 0;
     QList<QPointF> pointList;
     while(queryObj.next ()) {
        pointList.append (QPointF(x++, queryObj.value (0).toFloat ()));
//        if (x < 10) qDebug() << queryObj.value (0).toFloat ();
     }
    return pointList;
}

QList<QPointF> getTimeData() {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TDATE], [TIME], [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     QList<QPointF> pointList;
     while(queryObj.next ()) {
        QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
        QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
        QDateTime xValue;
        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
        float yValue = queryObj.value (2).toFloat ();
        pointList.append (QPointF(xValue.toMSecsSinceEpoch (), yValue));
     }
    return pointList;
}

void testSetTimeData(QLineSeries* series) {
    int date[3] = {20170101, 20170202, 20170303};
    int time[3] = {93000, 103000, 113000};
    float yValue[3] = {12.1, 12.2, 12.3};

    for (int i = 0; i < 3; ++i) {
        QList<int> tmpDate = getDateList(date[i]);
        QList<int> tmpTime = getTimeList (time[i]);
        QDateTime xValue;
        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
        series->append(xValue.toMSecsSinceEpoch(), yValue[i]);
    }
}

void setSeriresData(QLineSeries* series) {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     int x = 0;
     QList<QPointF> pointList;
     while(queryObj.next ()) {
        QPointF point = QPointF(x++, queryObj.value (0).toFloat ());
        pointList.append (point);
        series->append(x, queryObj.value (0).toFloat ());
        if (x < 10) qDebug() << queryObj.value (0).toFloat ();
     }
     int numRows = queryObj.at ();
     qDebug() << "numRows: " << numRows;
     qDebug() << "ListCount: " << pointList.count ();
}
