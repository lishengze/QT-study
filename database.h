#ifndef DATABASE_H
#define DATABASE_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QSqlDatabase>
#include "tabledata.h"

class Database
{
public:
    Database(QString connName="0", QString host="localhost", QString userName="sa", QString userPwd="sasa",
             QString dataSourceName="SqlServer", QString databaseDriver="QODBC");
    ~Database();

    QSqlDatabase initDatabase();
    QSqlDatabase getDatabase();
    bool isDatabaseOpen();

    QList<QPointF> getOriChartData(QString startDate, QString endDate, QString keyValue,
                                   QString tableName, QString databasename ="MarketData");

    QList<TableData> getOriData(QString startDate, QString endDate, QString keyValue,
                                             QString tableName, QString databaseName="MarketData");

    QList<QPointF> transData(QList<TableData> oriData, int minuteInterval, QString dataType);

private:
    QString m_connName;
    QString m_hostName;
    QString m_userName;
    QString m_userPwd;
    QString m_dataSourceName;
    QString m_databaseDriver;
    QSqlDatabase m_db;
    bool m_bdatabaseOpen;
};

#endif // DATABASE_H
