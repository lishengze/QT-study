#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTime>
#include "toolfunc.h"

Database::Database(QString connName, QString host, QString userName, QString userPwd,
         QString dataSourceName, QString databaseDriver):
    m_connName(connName),m_hostName(host), m_userName(userName),
    m_userPwd(userPwd), m_dataSourceName(dataSourceName),
    m_databaseDriver(databaseDriver), m_bdatabaseOpen(false)
{
    m_db = initDatabase();
}

Database::~Database () {
    m_db.close ();
}

QSqlDatabase Database::initDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase (m_databaseDriver, m_connName);
    QString dataSourceName = m_dataSourceName;
//    QString dataSourceName = QString::fromLocal8Bit (m_dataSourceName.toStdString ().c_str ());
    db.setHostName(m_hostName);
    db.setDatabaseName (dataSourceName);
    db.setUserName (m_userName);
    db.setPassword (m_userPwd);

    if (!db.open ()) {
        qDebug() << "Open database: " + dataSourceName  + " failed!";
        qDebug() << db.lastError ();
    } else {
        qDebug() << "Open database: " + dataSourceName  + " successfully!";
        m_bdatabaseOpen = true;
    }
    return db;
}

bool Database::isDatabaseOpen () {
    return m_bdatabaseOpen;
}

QSqlDatabase Database::getDatabase () {
    return m_db;
}

QList<QPointF> Database::getOriChartData(QString startDate, QString endDate, QString keyValue,
                                         QString tableName, QString databaseName){

    QSqlQuery queryObj(m_db);
    QString sqlstr = QString("select [TDATE], [TIME], [") + keyValue + QString("] ") \
                   + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                   + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
    qDebug() <<sqlstr;
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

QList<TableData> Database::getOriData(QString startDate, QString endDate, QString keyValue,
                                         QString tableName, QString databaseName){

    QSqlQuery queryObj(m_db);
    QString sqlstr = QString("select [TDATE], [TIME], [") + keyValue + QString("] ") \
                   + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                   + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
    qDebug() <<sqlstr;
    queryObj.exec(sqlstr);

     QList<TableData> tableDataList;
     while(queryObj.next ()) {
        tableDataList.append (TableData(queryObj.value (0).toInt (), queryObj.value (1).toInt (), queryObj.value (2).toFloat ()));
//        QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
//        QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
//        QDateTime xValue;
//        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
//        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
//        float yValue = queryObj.value (2).toFloat ();
//        pointList.append (QPointF(xValue.toMSecsSinceEpoch (), yValue));
     }
    return tableDataList;
}
