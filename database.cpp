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
//    qDebug() << "~Database" << 0;
    m_db.close ();
//    qDebug() << "~Database" << 1;
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
        QString succStr = "Open database: " + m_dataSourceName  + ", " + m_connName +" successfully!";
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

QMap<QString, QStringList> Database::getPreCloseData(QString tableName, QString databaseName) {
    QMap<QString, QStringList> result;
    if(m_db.open ()) {
    QSqlQuery queryObj(m_db);
    QString sqlstr = QString("select * from [") + databaseName + "].[dbo].[" + tableName + "] ";
    queryObj.exec(sqlstr);
    int valueNumb = 2;
     while(queryObj.next ()) {
         QStringList signalData;
         QString secode = queryObj.value (0).toString();
         for (int i = 1; i < valueNumb; ++i) {
             signalData.append(queryObj.value (i).toString());
         }
         result.insert(secode, signalData);
     }
    }else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QMap<QString, QStringList> Database::getSnapShootData(QString tableName, QString databaseName) {
    QMap<QString, QStringList> result;
    if(m_db.open ()) {
    QSqlQuery queryObj(m_db);
    QString sqlstr = QString("select * from [") + databaseName + "].[dbo].[" + tableName + "] ";
    queryObj.exec(sqlstr);
    int valueNumb = 6;
     while(queryObj.next ()) {
         QStringList signalData;
         QString secode = queryObj.value (0).toString();
         for (int i = 1; i < valueNumb; ++i) {
             signalData.append(queryObj.value (i).toString());
         }
//         qDebug() << "i: " << ++i << secode << ":  " << signalData;
         result.insert(secode, signalData);
     }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
//    qDebug() << "Database::getSnapShootData End!";
    return result;
}

QMap<QString, QStringList> Database::getSnapShootData(QList<QString> tableNameArray, QString databaseName) {
    QMap<QString, QStringList> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        for (int i = 0; i < tableNameArray.size(); ++i) {
            QString secode = tableNameArray[i];
            QString tableName = secode;
            QString completeTableName = "[" + databaseName + "].[dbo].[" + tableName + "]";
            QString sqlstr = "select * from" + completeTableName + "where 时间 = (select max(时间) from " + completeTableName + ")";
            queryObj.exec(sqlstr);
            int valueNumb = 6;
             while(queryObj.next ()) {
                 QStringList signalData;
                 QString secode = queryObj.value (0).toString();
                 for (int j = 1; j < valueNumb; ++j) {
                     signalData.append(queryObj.value (j).toString());
                 }
                 result.insert(secode, signalData);
             }
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
//    qDebug() << "Database::getSnapShootData End!";
    return result;
}

QMap<QString, QList<QStringList>> Database::getSnapShootHistoryData(QList<QString> tableNameArray, QString databaseName) {
    QMap<QString, QList<QStringList>> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        for (int i = 0; i < tableNameArray.size(); ++i) {
            QString secode = tableNameArray[i];
            QString tableName = secode;
            QString completeTableName = "[" + databaseName + "].[dbo].[" + tableName + "]";
            QString sqlstr = "select * from" + completeTableName;
            queryObj.exec(sqlstr);
            int valueNumb = 7;
            QList<QStringList> oneSecodeData;
             while(queryObj.next ()) {
                 QStringList signalData;
                 QString secode = queryObj.value (0).toString();
                 for (int j = 1; j < valueNumb; ++j) {
                     signalData.append(queryObj.value (j).toString());
                 }
                 oneSecodeData.append(signalData);
             }
             result.insert(secode, oneSecodeData);
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}
