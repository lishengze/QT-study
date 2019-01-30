#ifndef READDATABASEDATA_H
#define READDATABASEDATA_H
#include <QString>
#include "database.h"

class ReadDatabaseData:public QObject
{
    Q_OBJECT
public:
    ReadDatabaseData();

    ReadDatabaseData(QString dbhost, int dbConnectID, QString dataType,
                     QString stateDate, QString endDate, QStringList secodeList,
                     QList<QStringList> indexTimeList, QStringList keyValueList,
                     QObject *parent = 0);

    ReadDatabaseData(QString dbhost, QString indexDate,  
                     QString indexCode, QObject *parent = 0);

    ReadDatabaseData(QString dbhost, int dbConnectID,
                     QString date, QStringList industryList,
                     QObject *parent = 0);

    ~ReadDatabaseData();

    void initDatabase();
    void releaseDatabase();

signals:
    void readMarketDataComplete_signal(QList<QList<QStringList>> result);
    void readOneMarketDataComplete_signal();

    void readWeightDataComplete_signal(QList<QStringList> result);
    void readOneWeightDataComplete_signal();

    void readIndustryDataComplete_signal(QList<QStringList> result);
    void readOneIndustryDataComplete_signal();

    void testSignal_signal();

public slots:
    void startReadMarketData_slot();

    void startReadWeightData_slot();

    void startReadIndustryData_slot();

private:
    QString             m_dbhost;
    int                 m_dbConnectID;

    QString             m_dataType;

    QString             m_startDate;
    QString             m_endDate;
    QStringList         m_secodeList;
    QList<QStringList>  m_indexTimeList;
    QStringList         m_keyValueList;
    Database*           m_database;

    QString             m_weightIndexCode;
    QString             m_indexDate;

    QStringList         m_industryList;
    QString             m_industryDate;

};

#endif // READDATABASEDATA_H
