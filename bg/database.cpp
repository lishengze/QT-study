#include "database.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTime>
#include <QMessageBox>
#include "time_func.h"
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
        QString succStr = "Open database: " + m_connDbName  + ", "+ m_hostName
                        + ", " + m_connName +" successfully!";
//        qDebug() << succStr;
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

QList<QString> Database::getAllData(QString tableName, QString databaseName) {
    QList<QString> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString completeTableName = QString("[%1].[dbo].[%2]").arg(databaseName).arg(tableName);
        QString sqlstr = QString("select * from %1").arg(completeTableName);
        queryObj.exec(sqlstr);
         while(queryObj.next ()) {
            QString contract_code = queryObj.value (0).toString();
            result.append(contract_code);
         }
    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QList<QStringList> Database::getDataByDate(QString startDate, QString endDate, QStringList keyValue,
                                       QString tableName, QString databaseName) {
    QList<QStringList> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString completeTableName = QString("[%1].[dbo].[%2]").arg(databaseName).arg(tableName);
        QString keyValueStr = keyValue.join(",");
        QString sqlstr = QString("select %1 from %2 where TDATE >= %3 and TDATE <= %4 order by TDATE, TIME")
                        .arg(keyValueStr).arg(completeTableName).arg(startDate).arg(endDate);
//        qDebug() << "sqlstr: " << sqlstr;
        queryObj.exec(sqlstr);
         while(queryObj.next ()) {
             QStringList tmpResult;
             for (int i = 0; i < keyValue.size(); ++i) {
                 tmpResult.append(queryObj.value (i).toString());
             }
            result.append(tmpResult);
         }
    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QList<QStringList> Database::getWeightData(QString date, QString tableName) {
    QList<QStringList> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString databaseName = "WeightData";
        QString completeTableName = QString("[%1].[dbo].[%2]").arg(databaseName).arg(tableName);
        QString keyValueStr = "*";
        int     keyNumb = 9;
        QString sqlstr = QString::fromLocal8Bit("select %1 from %2 where 指数截止日 = '%3' order by 排名")
                        .arg(keyValueStr).arg(completeTableName).arg(date);
//        qDebug() << "sqlstr: " << sqlstr;
        queryObj.exec(sqlstr);
         while(queryObj.next ()) {
             QStringList tmpResult;
             for (int i = 0; i < keyNumb; ++i) {
                 tmpResult.append(queryObj.value (i).toString());
             }
            result.append(tmpResult);
         }
    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QList<QStringList> Database::getIndustryData(QStringList keyValueList, QString tableName) {
    QList<QStringList> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString databaseName = "IndustryData";
        QString completeTableName = QString("[%1].[dbo].[%2]").arg(databaseName).arg(tableName);
        QString keyValueStr = keyValueList.join(",");
        QString sqlstr = QString::fromLocal8Bit("select %1 from %2")
                        .arg(keyValueStr).arg(completeTableName);
//        qDebug() << "sqlstr: " << sqlstr;
        queryObj.exec(sqlstr);
         while(queryObj.next ()) {
             QStringList tmpResult;
             for (int i = 0; i < keyValueList.size(); ++i) {
                 tmpResult.append(queryObj.value (i).toString());
             }
            result.append(tmpResult);
         }
    } else {
        QMessageBox::information (m_window, "Error", m_db.lastError().text());
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

QString Database::getCreateStr(QString tableName) {
    return "";
}

QString Database::getInsertStr(QString tableName, QList<QString> data) {
    return "";
}

QString Database::getUpdateStr(QString tableName, QList<QString> data) {
    return "";
}

QList<QString> Database::getTableList(QString databaseName) {
    QList<QString> result;
    if(m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString sqlstr = "select name from "+ databaseName +"..sysobjects where xtype= 'U'";
        queryObj.exec(sqlstr);
        while(queryObj.next ()) {
             result.append(queryObj.value (0).toString());
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

QString Database::getDatabaseName() {
    return m_connDbName;
}

QString Database::dropTable(QString tableName) {
    QString msg = "SUCCESS";
    if (m_db.open()) {
        QString complete_tablename = "[" + m_connDbName + "].[dbo].[" + tableName + "]" ;
        QString drop_str = "drop table " + complete_tablename;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(drop_str);
        if (!result) {
            msg = "drop " + tableName +"faild";
        }
//        qDebug() << "drop " <<tableName << " rst: " << result;
    } else {
        msg = m_db.lastError().text();
        qDebug() <<"error_SqlServer:\n" << msg;
    }
    return msg;
}

QString Database::clear() {
    QString msg = "SUCCESS";
    QString curMsg = "";
    QList<QString> tableList = getTableList(m_connDbName);
    for (int i = 0; i < tableList.size(); ++i) {
        QString tmpMsg = dropTable(tableList[i]);
        if (tmpMsg != "SUCCESS") {
            curMsg += tmpMsg + "\n";
        }
    }
    if (curMsg != "") {
        msg = curMsg;
    }
    return msg;
}

QString Database::completeTable(QList<QString> tableList) {
    QString msg = "SUCCESS";
    QString curMsg = "";
    QList<QString> oriTableList = getTableList(m_connDbName);
    for (int i = 0; i < tableList.size(); ++i) {
        QString secode = tableList[i];
        if (oriTableList.indexOf(secode) < 0) {
            QString tmpMsg = createTable(secode);
            if (tmpMsg != "SUCCESS") {
                curMsg += tmpMsg + "\n";
            }
        }
    }
    if (curMsg != "") {
        msg = curMsg;
    }
    return msg;
}

QString Database::createTable(QString tableName) {
    QString msg = "SUCCESS";
    if (m_db.open()) {
        QString create_str = getCreateStr(tableName);
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(create_str);
        if (!result) {
            msg = "createTable " + tableName +" faild";
        }
//        qDebug() << "create str: " << create_str;
//        qDebug() << "create " <<tableName << " rst: " << result;
    } else {
        msg = m_db.lastError().text();
        qDebug() <<"error_SqlServer:\n" << msg;
    }
    return msg;
}

QString Database::insertData(QString tableName, QList<QString> data) {
    QString msg = "SUCCESS";
    if (m_db.open()) {
        QString insert_str = getInsertStr(tableName, data);
//        qDebug() << "insert str: " << insert_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(insert_str);
        if (!result) {
            msg = "insert " + tableName +" faild";
        }
//        qDebug() << "insert rst: " << result;
    } else {
        msg = m_db.lastError().text();
        qDebug() <<"error_SqlServer:\n" << msg;
    }
    return msg;
}

QString Database::updateData(QString tableName, QList<QString> data) {
    QString msg = "SUCCESS";
    if (m_db.open()) {
        QString update_str = getUpdateStr(tableName, data);
        qDebug() << "update str: " << update_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(update_str);
        if (!result) {
            msg = "update " + tableName +" faild";
        }
        qDebug() << "update rst: " << result;
    } else {
        msg = m_db.lastError().text();
        qDebug() <<"error_SqlServer:\n" << msg;
    }
    return msg;
}

bool Database::checkData(QString tableName, QString colName, QString value) {
    return false;
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
            queryObj.exec(sqlstr);
            int valueNumb = 3;
            QList<QStringList> oneSecodeData;
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
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}



