#ifndef EXTRACTMARKETDATA_H
#define EXTRACTMARKETDATA_H
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QDateTime>
#include <QMap>
#include "workprogressdialog.h"
#include "readdatabasedata.h"
#include "database.h"

class ExtractMarketData:public QObject
{
    Q_OBJECT
public:
    ExtractMarketData(QString dbhost, int dbConnectID, QString dataType,
                      QString startDate, QString endDate,
                      QStringList secodeList, QString desDirName,
                      QStringList keyValueList, int threadCount,
                      QObject *parent = 0);
    void initCommonData();
    void initKeyValueMap();
    void initWorkProgressDialog();
    void initDatabase();
    void initSecodeList();
    void initReadThreadCount();
    void initIndexTimeList();
    void allocateThreadData();
    void createReadThreads();

    void registerMetatype();

    void startReadData();
    void storeData();
    int writeMatrixExcelData(QString fileName, QList<QStringList>& oriData,
                        QString sheetName, bool isTranspose);
    ~ExtractMarketData();

signals:
    void startReadMarketData_signal();

    void extractMarketDataComplete_signal(QStringList);

public slots:
    void readMarketDataComplete_slot(QList<QList<QStringList>> threadResult);

    void testSignal_slot();

    void readOneMarketDataComplete_slot();

    void stopWork_slot();

private:
    int                         m_extractID;
    QString                     m_dbhost;
    int                         m_dbConnectID;
    QString                     m_dataType;
    QString                     m_oriFileName;
    QString                     m_desFileName;
    QString                     m_startDate;
    QString                     m_endDate;
    QStringList                 m_keyValueList;
    QString                     m_desDirName;
    QStringList                 m_desFileNameList;

    Database*                   m_database;
    QString                     m_indexCode;
    QList<QStringList>          m_indexTimeList;
    QStringList                 m_indexTimeStrList;
    QMap<QString, QString>      m_keyValueMap;

    int                         m_readThreadCount;
    QList<QStringList>          m_readThreadData;
    QList<QString>              m_secodeList;
    QList<QList<QStringList>>   m_sumResult;
    QList<QThread*>             m_ReadThreadList;
    QList<ReadDatabaseData*>    m_ReadDataObjList;
    int                         m_currCompleteCount;
    mutable QMutex             m_oneThreadCompleteMutex;
    mutable QMutex             m_readOneDataCompleteMutex;

    WorkProgressDialog*         m_workProgressDialog;
    QDateTime                   m_startTime;
    QDateTime                   m_endTime;
};

#endif // EXTRACTMARKETDATA_H
