#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTime>
#include <QMessageBox>
#include "toolfunc.h"

Database::Database(QWidget* window, QString connName, QString host,
                   QString userName, QString userPwd,
                   QString connDbName, QString port,
                   QString dataSourceName, QString databaseDriver):
    m_window(window),
    m_connName(connName),m_hostName(host),
    m_userName(userName),m_userPwd(userPwd),
    m_connDbName(connDbName), m_port(port),
    m_dataSourceName(dataSourceName), m_databaseDriver(databaseDriver),
    m_bdatabaseOpen(false)
{
    initDatabase();
}

Database::Database(QString connName, QString host,
                   QString userName, QString userPwd,
                   QString connDbName, QString port,
                   QString dataSourceName, QString databaseDriver):
    m_window(NULL),
    m_connName(connName),m_hostName(host),
    m_userName(userName),m_userPwd(userPwd),
    m_connDbName(connDbName), m_port(port),
    m_dataSourceName(dataSourceName), m_databaseDriver(databaseDriver),
    m_bdatabaseOpen(false)
{
    initDatabase();
}

Database::~Database () {
//    qDebug() << "m_db.close()";
    m_db.close ();
}

void Database::initDatabase() {
    m_db = QSqlDatabase::addDatabase (m_databaseDriver, m_connName);
//    QString dataSourceName = m_dataSourceName;
//    m_db.setHostName(m_hostName);
//    m_db.setDatabaseName (dataSourceName);
//    m_db.setUserName (m_userName);
//    m_db.setPassword (m_userPwd);

    QString dsn = QString("Driver={sql server};SERVER=%1;PORT=%2;DATABASE=%3;UID=%4;PWD=%5;")
                  .arg(m_hostName)
                  .arg(m_port)
                  .arg(m_connDbName)
                  .arg(m_userName)
                  .arg(m_userPwd);
    m_db.setDatabaseName(dsn);

    if (!m_db.open ()) {
        qDebug() << "Open database: " + m_dataSourceName  + " failed!";
        qDebug() << m_db.lastError ();
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
    } else {
        QString succStr = "Open database: " + m_dataSourceName  + " successfully!";
        qDebug() << succStr;
//        QMessageBox::information (m_window, "Succeed", succStr);
        m_bdatabaseOpen = true;
    }
}

bool Database::isDatabaseOpen () {
    return m_bdatabaseOpen;
}

QSqlDatabase Database::getDatabase () {
    return m_db;
}

QList<QPointF> Database::getOriChartData(QString startDate, QString endDate, QString keyValue,
                                         QString tableName, QString databaseName){

    QList<QPointF> pointList;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString sqlstr = QString("select [TDATE], [TIME], [") + keyValue + QString("] ") \
                       + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                       + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
        qDebug() <<sqlstr;
        queryObj.exec(sqlstr);
         while(queryObj.next ()) {
            QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
            QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
            QDateTime xValue;
            xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
            xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
            float yValue = queryObj.value (2).toFloat ();
            pointList.append (QPointF(xValue.toMSecsSinceEpoch (), yValue));
         }
    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return pointList;
}

QList<QStringList> Database::getOriChartData(QString startDate, QString endDate, QStringList keyValueList,
                                         QString tableName, QString databaseName){

    QList<QStringList> resultList;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString keyValueStr = "";
        for (int i = 0; i < keyValueList.size (); ++i) {
            keyValueStr += ", [" + keyValueList[i] + "] ";
        }

        QString sqlstr = QString("select [TDATE], [TIME] ") + keyValueStr  \
                       + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                       + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
//        qDebug() <<sqlstr;
        queryObj.exec(sqlstr);
         while(queryObj.next ()) {
            QStringList tmpResult;
            QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
            QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
            QDateTime timeValue;
            timeValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
            timeValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
            tmpResult.append (QString("%1").arg(timeValue.toMSecsSinceEpoch ()));
            for (int i = 0; i < keyValueList.size(); ++i) {
                tmpResult.append(queryObj.value (i+2).toString ());
            }
            resultList.append (tmpResult);
         }
    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return resultList;
}


QList<TableData> Database::getOriData(QString startDate, QString endDate, QString keyValue,
                                         QString tableName, QString databaseName){

    QList<TableData> tableDataList;
    if(m_db.open ()) {
    QSqlQuery queryObj(m_db);
    QString sqlstr = QString("select [TDATE], [TIME], [") + keyValue + QString("] ") \
                   + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                   + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
    qDebug() <<sqlstr;
    queryObj.exec(sqlstr);

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
    }else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return tableDataList;
}
