#include "database_func.h"
#include "database.h"

QList<int> getOneDayTimeList(QString host, QString databaseName)
{
    QList<int> result;
    Database dbObj(host);
    QString tableName = "SH000001";
    result = dbObj.getOneDayTimeList(tableName, databaseName);
    return result;
}

QDate getRealtimeDate(QString host)
{
    Database dbObj(host);
    
    QDate result = dbObj.getRealtimeDBDate();

    return result;
}
