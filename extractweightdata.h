#ifndef EXTRACTWEIGHTDATA_H
#define EXTRACTWEIGHTDATA_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QDateTime>
#include <QMap>
#include "workprogressdialog.h"
#include "readdatabasedata.h"

class ExtractWeightData:public QObject
{
    Q_OBJECT
public:
    ExtractWeightData(QString dbhost, int dbConnectID,
                      QString startDate, QString endDate,
                      QStringList indexCodeList, QString desDirName,
                      int threadCount, QObject *parent = 0);
    ~ExtractWeightData();

    void initCommonData();
    void initDateList();
    void initWorkProgressDialog();
    void initReadThreadCount();
    void initIndexCodeMap();
    void registerMetatype();
    void allocateThreadData();
    void createReadThreads();
    void startReadData();
    void storeData();

    int writeMatrixExcelData(QString fileName, QList<QStringList>& oriData,
                            QString sheetName, bool isTranspose);
signals:
    void startReadWeightData_signal();

    void extractWeightDataComplete_signal(QStringList);

public slots:
    void readWeightDataComplete_slot(QList<QStringList> threadResult);

    void readOneWeightDataComplete_slot();

    void stopWork_slot();

private:
    int                         m_dbConnectID;
    QString                     m_dbhost;
    QString                     m_startDate;
    QString                     m_endDate;
    QStringList                 m_indexCodeList;
    QString                     m_desDirName;
    QStringList                 m_desFileNameList;
    QStringList                 m_dateList;
    QMap<QString, QString>      m_indexCodeMap;

    QList<QStringList>          m_readThreadData;
    int                         m_readThreadCount;
    QList<QThread*>             m_ReadThreadList;
    QList<ReadDatabaseData*>    m_ReadDataObjList;
    QList<QList<QStringList>>   m_sumResult;
    int                         m_currCompleteCount;
    mutable QMutex             m_oneThreadCompleteMutex;
    mutable QMutex             m_readOneDataCompleteMutex;

    WorkProgressDialog*         m_workProgressDialog;
    QDateTime                   m_startTime;
    QDateTime                   m_endTime;
};

#endif // EXTRACTWEIGHTDATA_H
