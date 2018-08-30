#ifndef DATABASE_H
#define DATABASE_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QSqlDatabase>
#include <QWidget>

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

    QList<QString> getAllData(QString tableName, QString databaseName);

    QList<QStringList> getAllData(QString tableName, QString databaseName, QStringList keyValueList);

    QList<QStringList> getAllRealtimeData(QString tableName, QStringList keyValueList,
                                          QString databaseName="MarketData_RealTime");

    QList<QStringList> getDataByDate(QString startDate, QString endDate, QStringList keyValue,
                                 QString tableName, QString databasename, bool bGetShortedDate=false);

    QList<QStringList> getWeightData(QString date, QString tableName);

    QList<QStringList> getIndustryData(QStringList keyValueList, QString tableName);

    QList<QPointF> getOriChartData(QString startDate, QString endDate, QString keyValue,
                                   QString tableName, QString databasename="MarketData");

    QList<QStringList> getOriChartData(QString startDate, QString endDate, QStringList keyValue,
                                       QString tableName, QString databaseName="MarketData");

    QMap<QString, QStringList> getSnapShootData(QString tableName="AllData", 
                                                QString databaseName="MarketData_RealTime");   

    QMap<QString, QStringList> getSnapShootData(QList<QString> tableNameArray, 
                                                QString databaseName="MarketData_RealTime");

    QStringList getSnapShootData(QString tableName, QStringList keyValueList,
                                 QString databaseName="MarketData_RealTime");

    QMap<QString, QList<QStringList>> getSnapShootHistoryData(QList<QString> tableNameArray, 
                                                              QString databaseName="MarketData_RealTime");

    QMap<QString, QList<QStringList>> getLongTimeHistoryData(QString startDate, QString endDate, QStringList keyValueList,
                                                             QStringList tableNameList, QString databaseName="MarketData");

    QMap<QString, QStringList> getPreCloseData(QString tableName="PreCloseData", 
                                                QString databaseName="MarketData_RealTime");

    QList<double> getFutureSpread(QString future,
                           QString databaseName="MarketData_RealTime");

    QList<double> getHistFutureSpread(QString future,
                           QString databaseName="MarketData_RealTime");

    double getClosePrice(QString secode, QString date);

    QList<QString> getTableList(QString databaseName);

    QMap<QString, QList<QStringList>> getAnnouncement(QList<QString> tableNameArray, 
                                                      QString startDate, QString endDate,
                                                      QString databaseName="Announcement");

    QList<int> getOneDayTimeList(QString tableName, QString databaseName);

    QList<QStringList> getMarketSecodeList(QString tableName, QString databaseName="Market_Info");

    QString getDatabaseName();
    QString dropTable(QString tableName);
    QString clear();

    virtual QString completeTable(QList<QString> tableList);
    virtual bool checkData(QString tableName, QString colName, QString value);

    virtual QString getCreateStr(QString tableName);
    virtual QString createTable(QString tableName);

    virtual QString getInsertStr(QString tableName, QList<QString> data);
    virtual QString insertData(QString tableName, QList<QString> data);

    virtual QString getUpdateStr(QString tableName, QList<QString> data);
    virtual QString updateData(QString tableName, QList<QString> data);

protected:
    QString        m_connDbName;

private:
    QString        m_connName;
    QString        m_hostName;
    QString        m_userName;
    QString        m_userPwd;
    QString        m_port;
    QString        m_dataSourceName;
    QString        m_databaseDriver;
    QWidget*       m_window;

public:
    QSqlDatabase   m_db;
    bool           m_bdatabaseOpen;
};

#endif // DATABASE_H
