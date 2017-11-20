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
#include "setdata.h"
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

QList<QPointF> getDatabaseData() {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     int x = 0;
     QList<QPointF> pointList;
     while(queryObj.next ()) {
        pointList.append (QPointF(x++, queryObj.value (0).toFloat ()));
//        pointList.append (QPointF(x++, queryObj.value (0).toInt ()));
//        pointList.append (QPointF(x++, x));
        if (x < 10) qDebug() << queryObj.value (0).toFloat ();
     }
     int numRows = queryObj.at ();
     qDebug() << "numRows: " << numRows;
    return pointList;
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
