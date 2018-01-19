#include "realtimedatabase.h"

RealTimeDatabase::RealTimeDatabase(QWidget* window, QString connDbName = "master", QString connName="0", QString host="localhost",
                                   QString userName="sa", QString userPwd="sasa",
                                   QString port ="1433", QString dataSourceName="SqlServer", QString databaseDriver="QODBC"):
    Database(window, connName, host, userName, userPwd, connDbName, port, dataSourceName, databaseDriver)
{

}

RealTimeDatabase::RealTimeDatabase(QString connDbName = "master", QString connName="0", QString host="localhost",
                                   QString userName="sa", QString userPwd="sasa",
                                   QString port ="1433", QString dataSourceName="SqlServer", QString databaseDriver="QODBC"):
    Database(connName, host, userName, userPwd, connDbName, port, dataSourceName, databaseDriver)
{

}

void RealTimeDatabase::createTable(QString tableName) {

    QString createStr = "(股票 varchar(15) not null, 日期 int not null, 时间 int not null, \
                          最新成交价 decimal(15,4), 前收 decimal(15,4), 成交额 decimal(25,4), \
                          请求时间 varchar(35) not null,  Primary Key(股票, 请求时间))";

    QString complete_tablename = u'[' + m_connDbName + '].[dbo].['+ tableName +']';
    QString create_str = "create table " + complete_tablename + value_str;


}

void RealTimeDatabase::insertData(QString tableName, QList<QString> data) {

}

void RealTimeDatabase::updateData(QString tableName, QList<QString> data) {

}

RealTimeDatabase::~RealTimeDatabase() {

}
