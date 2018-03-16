#ifndef REALTIMEDATABASE_H
#define REALTIMEDATABASE_H
#include <QDate>
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

    QDate getDatabaseDataDate();

    bool checkdataTime();

    bool checkData(QString colname, QString keyvalue, QString table_name);

    int getDatabaseWorkingFlag(QString tableName);

    bool setDatabaseWorkingState(QString tableName, int flag);

    virtual QString getCreateStr(QString tableName);
//    virtual void createTable(QString tableName);

    virtual QString getInsertStr(QString tableName, QList<QString> data);
//    virtual void insertData(QString tableName, QList<QString> data);

    virtual QString getUpdateStr(QString tableName, QList<QString> data);
//    virtual void updateData(QString tableName, QList<QString> data);

    void createPreCloseTable(QString tableName);
    void insertPreCloseData(QString tableName, QList<QString> data);
    void updatePreCloseData(QString tableName, QList<QString> data);



    QList<QString> getSecodeList(QString tableName);

    ~RealTimeDatabase();
};


#endif // REALTIMEDATABASE_H
