#ifndef REALTIMEDATABASE_H
#define REALTIMEDATABASE_H

#include "database.h"

class RealTimeDatabase : public Database
{
public:
    RealTimeDatabase(QWidget* window, QString connName="0", QString connDbName = "MarketData_RealTime",
                     QString host="192.168.211.165", QString port ="1433",
                     QString userName="sa", QString userPwd="sasa",
                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    RealTimeDatabase(QString connName="0", QString connDbName = "MarketData_RealTime",
                     QString host="192.168.211.165", QString port ="1433",
                     QString userName="sa", QString userPwd="sasa",                     
                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    virtual QString getCreateStr(QString tableName);
//    virtual void createTable(QString tableName);

    virtual QString getInsertStr(QString tableName, QList<QString> data);
    virtual void insertData(QString tableName, QList<QString> data);

    virtual QString getUpdateStr(QString tableName, QList<QString> data);
    virtual void updateData(QString tableName, QList<QString> data);

    ~RealTimeDatabase();
};


#endif // REALTIMEDATABASE_H