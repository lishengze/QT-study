#ifndef REALTIMEDATABASE_H
#define REALTIMEDATABASE_H

#include "database.h"

class RealTimeDatabase : public Database
{
public:
    RealTimeDatabase(QWidget* window, QString connName="0", QString host="localhost",
                     QString userName="sa", QString userPwd="sasa",
                     QString connDbName = "master", QString port ="1433",
                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    RealTimeDatabase(QString connName="0", QString host="localhost",
                     QString userName="sa", QString userPwd="sasa",
                     QString connDbName = "master", QString port ="1433",
                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    virtual void createTable(QString tableName);

    virtual void insertData(QString tableName, QList<QString> data);

    virtual void updateData(QString tableName, QList<QString> data);

    ~RealTimeDatabase();
};

#endif // REALTIMEDATABASE_H
