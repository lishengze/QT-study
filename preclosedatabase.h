#ifndef PRECLOSEDATABASE_H
#define PRECLOSEDATABASE_H
#include "database.h"

class PreCloseDatabase : public Database
{
public:
    PreCloseDatabase(QWidget* window, QString connName="0", QString host="localhost",
                     QString userName="sa", QString userPwd="sasa",
                     QString connDbName = "master", QString port ="1433",
                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    PreCloseDatabase(QString connName="0", QString host="localhost",
                     QString userName="sa", QString userPwd="sasa",
                     QString connDbName = "master", QString port ="1433",
                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    ~PreCloseDatabase();

    virtual void createTable(QString tableName);

    virtual void insertData(QString tableName, QList<QString> data);

    virtual void updateData(QString tableName, QList<QString> data);

};

#endif // PRECLOSEDATABASE_H
