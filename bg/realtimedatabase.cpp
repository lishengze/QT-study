#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "realtimedatabase.h"

RealTimeDatabase::RealTimeDatabase(int test):Database() {
    test;
}

//RealTimeDatabase::RealTimeDatabase(QWidget* window, QString connName, QString host,
//                                   QString userName, QString userPwd,
//                                   QString connDbName, QString port,
//                                   QString dataSourceName, QString databaseDriver):
//    Database(window, connName, host, userName, userPwd,
//             connDbName, port, dataSourceName, databaseDriver)
//{

//}

//RealTimeDatabase::RealTimeDatabase(QString connName, QString host,
//                                   QString userName, QString userPwd,
//                                   QString connDbName, QString port,
//                                   QString dataSourceName, QString databaseDriver):
//    Database(connName, host, userName, userPwd,
//             connDbName, port, dataSourceName, databaseDriver)
//{
//}

RealTimeDatabase::~RealTimeDatabase() {

}

QString RealTimeDatabase::getCreateStr(QString tableName) {
    QString value_str = " (股票 varchar(15) not null, 日期 int not null, 时间 int not null, \
                          最新成交价 decimal(15,4), 前收 decimal(15,4), 成交 decimal(25,4), \
                          请求时间 varchar(35) not null,  Primary Key(股票, 请求时间))";
    QString complete_tablename = "[" + m_connDbName + "].[dbo].["+ tableName + "]";
    QString create_str = "create table " + complete_tablename + value_str;
    return create_str;
}

QString RealTimeDatabase::getInsertStr(QString tableName, QList<QString> oridata) {
    QString col_str = " (股票, 日期, 时间, 最新成交价, 前收, 成交额, 请求时间)";
    QString date = oridata[0];
    QString time = oridata[1];
    QString last = oridata[2];
    QString pre_close = oridata[3];
    QString amt = oridata[4];
    QString secode = oridata[5];
    QString wsqtime = oridata[6];

    QString val_str = "\'" + secode + "\', " + date + ", " + time + ", "+ last + "," \
            + pre_close + ", " + amt + ", \'" + wsqtime + "\' ";

    QString complete_tablename = "[" + m_connDbName + "].[dbo].[" + tableName + "]" ;
    QString insertStr = "insert into "+ complete_tablename + col_str + "values ("+ val_str +")";
    return insertStr;
}

QString RealTimeDatabase::getUpdateStr(QString tableName, QList<QString> data) {
    QString updateStr = "";
    return updateStr;
}

/*
基类已经实现的虚函数;
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
*/
