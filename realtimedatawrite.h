#ifndef REALTIMEDATAWRITE_H
#define REALTIMEDATAWRITE_H

#include <QObject>
#include <QTableView>
#include <QThread>
#include <QMutex>
#include <QDateTime>
#include <QTimer>
#include "subwriteclass.h"
#include "realtimedatabase.h"

class RealTimeDataWrite:public QObject
{
    Q_OBJECT
public:
    RealTimeDataWrite(QTableView *programInfoTableView, QTableView *errorMsgTableView,
                      QString dbConnID, QString dbHost,
                      QObject *parent = 0);

    ~RealTimeDataWrite();
    void initCommonData();
    void initDatabase();

    void initMonitorTimer();
    void startMonitorTimer();
    void stopMonitorTimer();

    void registerParams();
    bool checkDatabase();
    bool clearDatabase();

    QList<QMap<QString, QStringList>> allocateData(QMap<QString, QStringList> oriData);
    void createSubWriteThreads(QList<QMap<QString, QStringList>>);

    void releaseSubClassThreads();

    void resetWriteSource();

    void writeFutureData(QMap<QString, QStringList> futureResult);

public slots:
    void setTableList_slot(QList<QString> secodeList, QList<QString> futureList);
    void writePreCloseData_slot(QMap<QString, QStringList> data);
    void writeRealTimeData_slot(QMap<QString, QStringList> secodeResult, QMap<QString, QStringList> futureResult);
    void writeRealTimeResult_slot(QList<QString> result);

    void monitorException_slot();

signals:
    void startMonitorTimer_signal();

    void stopMonitorTimer_signal();

    void setTableListComplete_signal();

    void setPrecloseDataComplete_signal();

    void writeComplete_signal(int);

    void writeRealTimeData_signal();

private:
    QTimer                  m_monitorExceptionTimer;
    int                     m_monitorExceptionWaitTime;
    bool                    m_currProccessState;

    QString                 m_dataTimeTable;
    QList<QString>          m_secodeList;
    QList<QString>          m_futureList;

    RealTimeDatabase*       m_realtimeDatabase;
    QTableView*             m_programInfoTableView;
    QTableView*             m_errorMsgTableView;
    int                     m_realtimeDataNumb;

    int                     m_waitWriteTime;
    bool                    m_currWriteFlag;
    QString                 m_dbConnID;
    QString                 m_dbHost;
    QString                 m_dbName;

    QMutex                  m_mutex;
    QList<SubWriteClass*>   m_subWriteObjList;
    QList<QThread*>         m_subWriteThreadList;
    int                     m_subWriteThreadNumb;

    int                     m_currCompleteThreadCount;
    int                     m_currSuccessNumb;
    int                     m_currFailedNumb;
    QList<QString>          m_currWriteErrorReuslt;

    int                     m_writeRealTimeDataCount;                   
    int                     m_writeMinimumTime;

    int                     m_testWriteStartTime;
    int                     m_testWriteSumTime;
    int                     m_testWriteCount;
    int                     m_testWriteAveTime;
    int                     m_testCurrWriteTime;

};

#endif // REALTIMEDATAWRITE_H
