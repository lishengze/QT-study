#ifndef EXTRACTINDUSTRYDATA_H
#define EXTRACTINDUSTRYDATA_H
#include <QObject>
#include <QString>
#include <QThread>
#include <QMutexLocker>
#include <QDateTime>
#include <QMap>
#include "workprogressdialog.h"
#include "readdatabasedata.h"


class ExtractIndustryData : public QObject
{
    Q_OBJECT
public:
    explicit ExtractIndustryData(int dbConnectID, QString dbhost,
                                 QString startDate, QString endDate,
                                 QStringList industryList, QString desDirName,
                                 int threadCount, QObject *parent = 0);
    ~ExtractIndustryData();

    void initCommonData();
    void initDateList();
    void initWorkProgressDialog();
    void initReadThreadCount();
    void registerMetatype();
    void allocateThreadData();
    void createReadThreads();
    void startReadData();
    void storeData();

    int writeMatrixExcelData(QString fileName, QList<QStringList>& oriData,
                            QString sheetName, bool isTranspose);
signals:
    void startReadIndustryData_signal();

    void extractIndustryDataComplete_signal(QStringList);

public slots:
    void readIndustryDataComplete_slot(QList<QStringList> threadResult);

    void readOneIndustryDataComplete_slot();

    void stopWork_slot();

private:
    int                         m_dbConnectID;
    QString                     m_dbhost;
    QString                     m_startDate;
    QString                     m_endDate;
    QStringList                 m_industryList;
    QStringList                 m_dateList;
    QString                     m_desDirName;
    QStringList                 m_desFileNameList;

    QStringList                 m_readThreadData;
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

#endif // EXTRACTINDUSTRYDATA_H
