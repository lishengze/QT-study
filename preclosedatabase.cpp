#include "preclosedatabase.h"

PreCloseDatabase::PreCloseDatabase(QWidget* window, QString connName, QString host,
                                   QString userName, QString userPwd,
                                   QString connDbName, QString port,
                                   QString dataSourceName, QString databaseDriver):
    Database(window, connName, host, userName, userPwd, connDbName, port, dataSourceName, databaseDriver)
{

}

PreCloseDatabase::PreCloseDatabase(QString connName, QString host,
                                   QString userName, QString userPwd,
                                   QString connDbName, QString port,
                                   QString dataSourceName, QString databaseDriver):
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
