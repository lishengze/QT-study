#ifndef DATABASE_H
#define DATABASE_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QSqlDatabase>
#include <QWidget>
#include "tabledata.h"

class Database
{
public:
    Database(QWidget* window, QString connName="0", QString host="localhost",
             QString userName="sa", QString userPwd="sasa",
             QString connDbName = "master", QString port ="1433",
             QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    Database(QString connName="0", QString host="localhost",
             QString userName="sa", QString userPwd="sasa",
             QString connDbName = "master", QString port ="1433",
             QString dataSourceName="SqlServer", QString databaseDriver="QODBC");

    ~Database();

    void initDatabase();
    QSqlDatabase getDatabase();
    bool isDatabaseOpen();

    QList<QPointF> getOriChartData(QString startDate, QString endDate, QString keyValue,
                                   QString tableName, QString databasename="MarketData");

    QList<QStringList> getOriChartData(QString startDate, QString endDate, QStringList keyValue,
                                       QString tableName, QString databaseName="MarketData");

    QList<TableData> getOriData(QString startDate, QString endDate, QString keyValue,
                                             QString tableName, QString databaseName="MarketData");

    QMap<QString, QStringList> getSnapShootData(QString tableName="AllData", QString databaseName="MarketData_RealTime");

    QMap<QString, QStringList> getSnapShootData(QList<QString> tableNameArray, QString databaseName="MarketData_RealTime");

    QMap<QString, QList<QStringList>> getSnapShootHistoryData(QList<QString> tableNameArray, QString databaseName="MarketData_RealTime");

    QMap<QString, QStringList> getPreCloseData(QString tableName = "PreCloseData", QString databaseName="MarketData_RealTime");

    QList<QPointF> transData(QList<TableData> oriData, int minuteInterval, QString dataType);

    QList<QString> getTableList(QString databaseName);

    QMap<QString, QList<QStringList>> getAnnouncement(QList<QString> tableNameArray, QString startDate, QString endDate,
                                                      QString databaseName="Announcement");

    void completeTable(QList<QString> tableList);
    virtual void checkData(QString tableName, QString colName, QString value);

    virtual QString getCreateStr(QString tableName);
    virtual void createTable(QString tableName);

    virtual QString getInsertStr(QString tableName, QList<QString> data);
    virtual void insertData(QString tableName, QList<QString> data);

    virtual QString getUpdateStr(QString tableName, QList<QString> data);
    virtual void updateData(QString tableName, QList<QString> data);

protected:
    QString m_connDbName;

private:
    QString m_connName;
    QString m_hostName;
    QString m_userName;
    QString m_userPwd;
    QString m_port;    
    QString m_dataSourceName;
    QString m_databaseDriver;
    QWidget* m_window;

public:
    QSqlDatabase m_db;
    bool m_bdatabaseOpen;
};

#endif // DATABASE_H
