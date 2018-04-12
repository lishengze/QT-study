#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTime>
#include <QMessageBox>
#include "toolfunc.h"

extern const int g_maxFutureSpread;

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
        QString succStr = "Open database: " + m_connDbName  + ", "+ m_hostName
                        + ", " + m_connName +" successfully!";
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

        if (queryObj.exec(sqlstr) ) {
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
            QStringList tmp; tmp.append("Error");
            resultList.append(tmp);
        }

    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return resultList;
}

QMap<QString, QList<QStringList>> Database::getLongTimeHistoryData(QString startDate, QString endDate, QStringList keyValueList,
                                                                   QStringList tableNameList, QString databaseName) {
   QMap<QString, QList<QStringList>> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString keyValueStr = "";
        for (int i = 0; i < keyValueList.size (); ++i) {
            keyValueStr += ", [" + keyValueList[i] + "] ";
        }
        for (QString tableName : tableNameList) {
            QString sqlstr = QString("select [TDATE], [TIME] ") + keyValueStr  \
                           + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                           + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
             QList<QStringList> oneSecodeData;
            if (queryObj.exec(sqlstr) ) {
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
                   oneSecodeData.append (tmpResult);
                }
                result.insert(tableName, oneSecodeData);
            } else {
                result.insert("Error", oneSecodeData);
            }
        }
    } else {
        QList<QStringList> oneSecodeData;
        QStringList errorData;
        errorData.append(QString::fromLocal8Bit(m_db.lastError().text().toStdString().c_str()));
        oneSecodeData.append(errorData);
        result.insert("Error", oneSecodeData);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
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
            QList<QStringList> oneSecodeData;
//            qDebug() << sqlstr;
            if (queryObj.exec(sqlstr) ) {
                int valueNumb = 7;
                while(queryObj.next ()) {
                     QStringList signalData;
                     QString secode = queryObj.value (0).toString();
                     for (int j = 1; j < valueNumb; ++j) {
                         signalData.append(queryObj.value (j).toString());
                     }
                     oneSecodeData.append(signalData);
                 }
                 result.insert(secode, oneSecodeData);
            } else {
                result.insert("Error", oneSecodeData);
            }
        }
    } else {
        QList<QStringList> oneSecodeData;
        QStringList errorData;
        errorData.append(QString::fromLocal8Bit(m_db.lastError().text().toStdString().c_str()));
        oneSecodeData.append(errorData);
        result.insert("Error", oneSecodeData);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}


QMap<QString, QStringList> Database::getPreCloseData(QString tableName, QString databaseName) {
    QMap<QString, QStringList> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString sqlstr = QString("select * from [") + databaseName + "].[dbo].[" + tableName + "] ";

        if (queryObj.exec(sqlstr)) {
            int valueNumb = 2;
             while(queryObj.next ()) {
                 QStringList signalData;
                 QString secode = queryObj.value (0).toString();
                 for (int i = 1; i < valueNumb; ++i) {
                     signalData.append(queryObj.value (i).toString());
                 }
                 result.insert(secode, signalData);
             }
        } else {
            QStringList tmpdata;
            result.insert("Error", tmpdata);
        }
    }else {
        QStringList errorData;
        errorData.append(QString::fromLocal8Bit(m_db.lastError().text().toStdString().c_str()));
        result.insert("Error", errorData);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QMap<QString, QStringList> Database::getSnapShootData(QString tableName, QString databaseName) {
    QMap<QString, QStringList> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString sqlstr = QString("select * from [") + databaseName + "].[dbo].[" + tableName + "] ";

        if (queryObj.exec(sqlstr)) {
            int valueNumb = 6;
             while(queryObj.next ()) {
                 QStringList signalData;
                 QString secode = queryObj.value (0).toString();
                 for (int i = 1; i < valueNumb; ++i) {
                     signalData.append(queryObj.value (i).toString());
                 }
                 result.insert(secode, signalData);
             }
        } else {
            QStringList tmpdata;
            result.insert("Error", tmpdata);
        }
    } else {
        QStringList errorData;
        errorData.append(QString::fromLocal8Bit(m_db.lastError().text().toStdString().c_str()));
        result.insert("Error", errorData);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
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
            QString sqlstr = "select * from " + completeTableName + QString::fromLocal8Bit(" where 时间 = (select max(时间) from ") + completeTableName + ")";
//            qDebug() << "sqlstr: " << sqlstr;
            if (queryObj.exec(sqlstr) ) {
                 int valueNumb = 7;
                 while(queryObj.next ()) {
                     QStringList signalData;
                     QString secode = queryObj.value (0).toString();
                     for (int j = 1; j < valueNumb; ++j) {
                         signalData.append(queryObj.value (j).toString());
                     }
                     result.insert(secode, signalData);
                 }
            } else {
                QStringList tmpdata;
                result.insert("Error", tmpdata);
            }
        }
    } else {
        QStringList errorData;
        errorData.append(QString::fromLocal8Bit(m_db.lastError().text().toStdString().c_str()));
        result.insert("Error", errorData);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

double Database::getClosePrice(QString secode, QString date) {
    double result = -1;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString tableName;
        QString completeTableName;
        QString sqlstr;
        if (date == "realtime") {
            tableName = getCompleteSecode(secode, "wind");
            completeTableName = "[MarketData_RealTime].[dbo].[" + tableName + "]";
            sqlstr = QString::fromLocal8Bit("select 最新成交价 from") + completeTableName +
                     QString::fromLocal8Bit(" where 时间 = (select max(时间) from ") + completeTableName + ")";
//            qDebug() << "sqlstr: " << sqlstr;
        } else {
            tableName = getCompleteSecode(secode, "tinysoft");
            completeTableName = "[MarketData_day].[dbo].[" + tableName + "]";
            sqlstr = "select TCLOSE from " + completeTableName + " where TDATE = "+ date ;
//            qDebug() << "sqlstr: " << sqlstr;
        }

        if (queryObj.exec(sqlstr)) {
            while(queryObj.next ()) {
                result = queryObj.value (0).toDouble();
            }
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QList<double> Database::getFutureSpread(QString futureName, QString databaseName) {
    QList<double> result;
    double spread = g_maxFutureSpread;
    double datetime = -1;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString tableName = futureName;
        QString completeTableName = QString("[%1].[dbo].[%2]").arg(databaseName).arg(tableName);
        QString sqlstr = QString::fromLocal8Bit("select [基差],[请求时间] from %1  where 请求时间 = (select max(请求时间) from %2)")
                                                .arg(completeTableName).arg(completeTableName);
        if (queryObj.exec(sqlstr)) {
            while(queryObj.next ()) {
                spread = queryObj.value (0).toDouble();
                datetime = queryObj.value (1).toString().toDouble();
            }
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    result.append(spread);
    result.append(datetime);
    return result;
}

QList<double> Database::getHistFutureSpread(QString futureName, QString databaseName) {
    QList<double> result;
    double spread = g_maxFutureSpread;
    double datetime = -1;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString tableName = futureName;
        QString completeTableName = QString("[%1].[dbo].[%2]").arg(databaseName).arg(tableName);
        QString sqlstr = QString::fromLocal8Bit("select [基差], [请求时间] from %1")
                                               .arg(completeTableName);
//        qDebug() << "sqlstr: " << sqlstr;
        if (queryObj.exec(sqlstr)) {
            while(queryObj.next ()) {
                spread = queryObj.value (0).toDouble();
                datetime = queryObj.value (1).toString().toDouble();
                result.append(spread);
                result.append(datetime);
            }
        } else {
            result.append(spread);
            result.append(datetime);
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;

}

QList<QString> Database::getTableList(QString databaseName) {
    QList<QString> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString sqlstr = "select name from "+ databaseName +"..sysobjects where xtype= 'U'";
        if (queryObj.exec(sqlstr) ) {
            while(queryObj.next ()) {
                 result.append(queryObj.value (0).toString());
            }
        } else {
            result.append("Error");
        }
    } else {
        result.append("Error");
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QMap<QString, QList<QStringList>> Database::getAnnouncement(QList<QString> tableNameArray, QString startDate,
                                                        QString endDate, QString databaseName) {
    QMap<QString, QList<QStringList>> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        for (int i = 0; i < tableNameArray.size(); ++i) {
            QString secode = tableNameArray[i];
            QString tableName = secode;
            QString completeTableName = "[" + databaseName + "].[dbo].[" + tableName + "]";
            QString sqlstr = "select * from" + completeTableName  + " where Date <= " + endDate + " and Date >= " + startDate;
            QList<QStringList> oneSecodeData;
            if (queryObj.exec(sqlstr) ) {
                int valueNumb = 4;

                 while(queryObj.next ()) {
                     QStringList signalData;
                     QString secode = queryObj.value (0).toString();
                     for (int j = 0; j < valueNumb; ++j) {
                         QString tmpdata = queryObj.value (j).toString();
                         tmpdata.remove("\t");
                         signalData.append(tmpdata);
                     }
                     oneSecodeData.append(signalData);
                 }
                 if (oneSecodeData.size() > 0) {
                     result.insert(secode, oneSecodeData);
                 }
            } else {
                result.insert("Error", oneSecodeData);
            }
        }
    } else {
        QList<QStringList> oneSecodeData;
        QStringList errorData;
        errorData.append(QString::fromLocal8Bit(m_db.lastError().text().toStdString().c_str()));
        oneSecodeData.append(errorData);
        result.insert("Error", oneSecodeData);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
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

QString Database::getCreateStr(QString tableName) {
    return "";
}

QString Database::getInsertStr(QString tableName, QList<QString> data) {
    return "";
}

QString Database::getUpdateStr(QString tableName, QList<QString> data) {
    return "";
}

void Database::completeTable(QList<QString> tableList) {
//    QList<QString> oriTableList = getTableList(m_connDbName);
//    for (int i = 0; i < tableList.size(); ++i) {
//        QString secode = tableList[i];
//        if (oriTableList.indexOf(secode) < -1) {
//            createTable(secode);
//        }
//    }
}

void Database::createTable(QString tableName) {
    if (m_db.open()) {
        QString create_str = getCreateStr(tableName);
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(create_str);
        qDebug() << "create rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
}

void Database::insertData(QString tableName, QList<QString> data) {
    if (m_db.open()) {
        QString insert_str = getInsertStr(tableName, data);
        qDebug() << "insert str: " << insert_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(insert_str);
        qDebug() << "insert rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
}

void Database::updateData(QString tableName, QList<QString> data) {
    if (m_db.open()) {
        QString update_str = getUpdateStr(tableName, data);
        qDebug() << "update str: " << update_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(update_str);
        qDebug() << "update rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
}

void Database::checkData(QString tableName, QString colName, QString value) {

}

//QList<TableData> Database::getOriData(QString startDate, QString endDate, QString keyValue,
//                                         QString tableName, QString databaseName){

//    QList<TableData> tableDataList;
//    if(m_db.open ()) {
//    QSqlQuery queryObj(m_db);
//    QString sqlstr = QString("select [TDATE], [TIME], [") + keyValue + QString("] ") \
//                   + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
//                   + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
//    qDebug() <<sqlstr;
//    queryObj.exec(sqlstr);

//     while(queryObj.next ()) {
//        tableDataList.append (TableData(queryObj.value (0).toInt (), queryObj.value (1).toInt (), queryObj.value (2).toFloat ()));
////        QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
////        QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
////        QDateTime xValue;
////        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
////        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
////        float yValue = queryObj.value (2).toFloat ();
////        pointList.append (QPointF(xValue.toMSecsSinceEpoch (), yValue));
//     }
//    }else {
//        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
//    }
//    return tableDataList;
//}
