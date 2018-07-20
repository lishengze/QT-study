#ifndef REALTIMEDATABASE_H
#define REALTIMEDATABASE_H

#include "database.h"

class RealTimeDatabase : public Database
{
public:
    RealTimeDatabase(int test);

//    RealTimeDatabase(QWidget* window, QString connName="0", QString host="192.168.211.165",
//                     QString userName="sa", QString userPwd="sasa",
//                     QString connDbName = "MarketData_RealTime", QString port ="1433",
//                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

//    RealTimeDatabase(QString connName="0", QString host="192.168.211.165",
//                     QString userName="sa", QString userPwd="sasa",
//                     QString connDbName = "MarketData_RealTime", QString port ="1433",
//                     QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    ~RealTimeDatabase();

    virtual QString getCreateStr(QString tableName);

    virtual QString getInsertStr(QString tableName, QList<QString> data);

    virtual QString getUpdateStr(QString tableName, QList<QString> data);

//    virtual bool checkData(QString tableName, QString colName, QString value);
//    virtual QString insertData(QString tableName, QList<QString> data);
//    virtual QString updateData(QString tableName, QList<QString> data);
//    virtual QString createTable(QString tableName);

};


#endif // REALTIMEDATABASE_H
