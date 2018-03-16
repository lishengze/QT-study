#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "realtimedatabase.h"
#include "toolfunc.h"

RealTimeDatabase::RealTimeDatabase(QWidget* window, QString connName, QString connDbName,
                                   QString host, QString port,
                                   QString userName, QString userPwd,
                                   QString dataSourceName, QString databaseDriver):
    Database(window, connName, host, userName, userPwd,
             connDbName, port, dataSourceName, databaseDriver)
{

}

RealTimeDatabase::RealTimeDatabase(QString connName, QString connDbName,
                                   QString host, QString port,
                                   QString userName, QString userPwd,                                    
                                   QString dataSourceName, QString databaseDriver):
    Database(connName, host, userName, userPwd,
             connDbName, port, dataSourceName, databaseDriver)
{
}


QDate RealTimeDatabase::getDatabaseDataDate() {
    QList<QString> tableList = getTableList(m_connDbName);
    QDate date = QDate::currentDate();
//    qDebug() << "tableList: " << tableList;
    if (tableList.size() != 0 && m_db.open ()) {
        QSqlQuery queryObj(m_db);
        QString complete_tablename = "[" + m_connDbName + "].[dbo].["+ tableList[0] + "]";
        QString sqlstr = "select top 1 [日期] from" + complete_tablename;
//        qDebug() << "sqlstr: " << sqlstr;
        queryObj.exec(sqlstr);
        QList<int> tmpDate;
         while(queryObj.next ()) {
             tmpDate = getDateList(queryObj.value (0).toInt ());
             qDebug() << "CurrDate: " << tmpDate;
         }
         if (tmpDate.size() > 0) {
             date = QDate(tmpDate[0], tmpDate[1], tmpDate[2]);
         }
    } else {
        date.addDays(-1);
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return date;
}

bool RealTimeDatabase::checkdataTime() {
    if (getDatabaseDataDate() == QDate::currentDate()) {
        return true;
    } else {
        return false;
    }
}

QString RealTimeDatabase::getCreateStr(QString tableName) {
    QString value_str = " (股票 varchar(15) not null, 日期 int not null, 时间 int not null, \
                          最新成交价 decimal(15,4), 前收 decimal(15,4), 成交金额 decimal(25,4), \
                          请求时间 varchar(35) not null,  Primary Key(股票, 请求时间))";
    QString complete_tablename = "[" + m_connDbName + "].[dbo].["+ tableName + "]";
    QString create_str = "create table " + complete_tablename + value_str;
    return create_str;
}

QString RealTimeDatabase::getInsertStr(QString tableName, QList<QString> oridata) {
    QString col_str = " (股票, 日期, 时间, 最新成交价, 前收, 成交金额, 请求时间)";
    QString secode = oridata[0];
    QString date = oridata[1] ;
    QString time = oridata[2];
    QString last = oridata[3];
    QString pre_close = oridata[4];
    QString amt = oridata[5];
    QString wsqtime = oridata[6];

    QString valStr = "\'" + secode + "\', " + date + ", " + time + ", "+ last + "," \
                   + pre_close + ", " + amt + ", \'" + wsqtime + "\' ";

    QString complete_tablename = "[" + m_connDbName + "].[dbo].[" + tableName + "]" ;
    QString insertStr = "insert into "+ complete_tablename + col_str + "values ("+ valStr +")";
    return insertStr;
}

QString RealTimeDatabase::getUpdateStr(QString tableName, QList<QString> data) {
    QString updateStr = "";
    return updateStr;
}

void RealTimeDatabase::createPreCloseTable(QString tableName) {
    if (m_db.open()) {
        QString value_str = "(股票 varchar(15) not null Primary Key(股票), 前收 decimal(15,4))";
        QString complete_tablename = "[" + m_connDbName + "].[dbo].["+ tableName + "]";
        QString create_str = "create table " + complete_tablename + value_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(create_str);
        result;
//        qDebug() << "create str: " << create_str;
//        qDebug() << "create " <<tableName << " rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
}

void RealTimeDatabase::insertPreCloseData(QString tableName, QList<QString> oridata) {
    if (m_db.open()) {
        QString col_str = " (股票, 前收) ";
        QString secode = oridata[0];
        QString pre_close = oridata[1];
        QString valStr = "\'" + secode + "\', " + pre_close;
        QString complete_tablename = "[" + m_connDbName + "].[dbo].[" + tableName + "]" ;
        QString insert_str = "insert into "+ complete_tablename + col_str + "values ("+ valStr +")";
//        qDebug() << "insert str: " << insert_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(insert_str);
        result;
//        qDebug() << "insert rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
}

QList<QString> RealTimeDatabase::getSecodeList(QString tableName) {
    QList<QString> result;
    if (m_db.open()) {
        QString completeTableName = "[" + m_connDbName + "].[dbo].[" + tableName + "]";
        QString sqlstr = "select * from" + completeTableName;
//        qDebug() << "sqlstr: " << sqlstr;
        QSqlQuery queryObj(m_db);
        queryObj.exec(sqlstr);
        while(queryObj.next ()) {
            QString secode = queryObj.value (0).toString();
            result.append(secode);
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

void RealTimeDatabase::updatePreCloseData(QString tableName, QList<QString> oridata) {
    if (m_db.open()) {
        QString pre_close = oridata[0];
        QString secode = oridata[1];
        QString set_str = "  set 前收 = " +  pre_close + " where 股票 = \'" + secode + "\'";
        QString complete_tablename = "[" + m_connDbName+ "].[dbo].[" + tableName + "]";
        QString update_str = "update "+ complete_tablename + set_str ;
        qDebug() << "update str: " << update_str;
        QSqlQuery queryObj(m_db);
        bool result = queryObj.exec(update_str);
        qDebug() << "update rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
}

bool RealTimeDatabase::checkData(QString colname, QString keyvalue, QString tableName) {
    bool result = false;
    if (m_db.open()) {
        QString complete_tablename = "[" + m_connDbName+ "].[dbo].[" + tableName + "]";
        QString check_str = "select * from "  + complete_tablename + "where " + colname + " = " + "\'" + keyvalue + " \'";
        qDebug() << "check str: " << check_str;
        QSqlQuery queryObj(m_db);
        qDebug() << "check rst: " << queryObj.exec(check_str);
        while(queryObj.next())  {
            result = true;
        }
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

int RealTimeDatabase::getDatabaseWorkingFlag(QString tableName) {
    int result = -1;
    if (m_db.open()) {
        QString complete_tablename = "[" + m_connDbName+ "].[dbo].[" + tableName + "]";
        QString check_str = "select top 1 [Flag] from "  + complete_tablename;
        QString sql_str = "select [Flag] from" + complete_tablename
                        +  "where Date = (select max(Date) from " + complete_tablename + ")";
        QSqlQuery queryObj(m_db);
        queryObj.exec(sql_str);
//        qDebug() << "check_str: " << sql_str;
        while(queryObj.next())  {
            result = queryObj.value (0).toInt ();
//            qDebug() << "result: " << result;
        }
        qDebug() <<"Working Flag: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

bool RealTimeDatabase::setDatabaseWorkingState(QString tableName, int flag) {
    bool result = false;
    if (m_db.open()) {
        QString col_str = QString::fromLocal8Bit(" (Date, Time, Flag) ");
        QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
        QString time = QDateTime::currentDateTime().toString("hhmmss");
        QString flagValue = QString("%1").arg(flag);
        QString valStr = date + ", " + time + ", " + flagValue;
        QString complete_tablename = "[" + m_connDbName + "].[dbo].[" + tableName + "]" ;
        QString insert_str = "insert into "+ complete_tablename + col_str + "values ("+ valStr +")";
        qDebug() << "insert str: " << insert_str;
        QSqlQuery queryObj(m_db);
        result = queryObj.exec(insert_str);
        qDebug() << "insert rst: " << result;
    } else {
        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
    }
    return result;
}

//void RealTimeDatabase::insertData(QString tableName, QList<QString> data) {
//    if (m_db.open()) {
//        QString insert_str = getInsertStr(tableName, data);
//        qDebug() << "insert str: " << insert_str;
//        QSqlQuery queryObj(m_db);
//        bool result = queryObj.exec(insert_str);
//        qDebug() << "insert rst: " << result;
//    } else {
//        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
//    }
//}

//void RealTimeDatabase::updateData(QString tableName, QList<QString> data) {

//}

//void RealTimeDatabase::createTable(QString tableName) {
//    if (m_db.open()) {
//        QString create_str = getCreateStr(tableName);
////        qDebug() << "create_str: " << create_str;
//        QSqlQuery queryObj(m_db);
//        bool result = queryObj.exec(create_str);
//        qDebug() << "create rst: " << result;
//    } else {
//        qDebug() <<"error_SqlServer:\n" << m_db.lastError().text();
//    }
//}


RealTimeDatabase::~RealTimeDatabase() {

}
