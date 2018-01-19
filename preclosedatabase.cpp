#include "preclosedatabase.h"

PreCloseDatabase::PreCloseDatabase(QWidget* window, QString connName="0", QString host="localhost",
                                   QString userName="sa", QString userPwd="sasa",
                                   QString connDbName = "master", QString port ="1433",
                                   QString dataSourceName="SqlServer", QString databaseDriver="QODBC"):
    Database(window, connName, host, userName, userPwd, connDbName, port, dataSourceName, databaseDriver)
{

}

PreCloseDatabase::PreCloseDatabase(QString connName="0", QString host="localhost",
                                   QString userName="sa", QString userPwd="sasa",
                                   QString connDbName = "master", QString port ="1433",
                                   QString dataSourceName="SqlServer", QString databaseDriver="QODBC"):
    Database(connName, host, userName, userPwd, connDbName, port, dataSourceName, databaseDriver)
{

}

void PreCloseDatabase::createTable(QString tableName) {

}

void PreCloseDatabase::insertData(QString tableName, QList<QString> data) {

}

void PreCloseDatabase::updateData(QString tableName, QList<QString> data) {

}

PreCloseDatabase::~PreCloseDatabase() {

}
