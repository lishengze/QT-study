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
                      QString dbConnID, QString dbName, QString dbHost,
                      QObject *parent = 0);

    ~RealTimeDataWrite();
    void initDatabase();
    void checkDatabase();
    void clearDatabase();

    QList<QMap<QString, QStringList>> allocateData(QMap<QString, QStringList> oriData);
    void createSubWriteThreads(QList<QMap<QString, QStringList>>);

    void registerParams();
    void releaseSubClassThreads();

public slots:
    void setSecodeList_slot(QList<QString> data);
    void writePreCloseData_slot(QMap<QString, QStringList> data);
    void writeRealTimeData_slot(QMap<QString, QStringList> data);
    void writeRealTimeResult_slot(QList<QString> result);

signals:
    void setRealTimeData_signal();
    void writeComplete_signal();

    void writeRealTimeData_signal();

private:
    QList<QString>          m_secodeList;
    RealTimeDatabase*       m_realtimeDatabase;
    QTableView*             m_programInfoTableView;
    QTableView*             m_errorMsgTableView;
    int                     m_realtimeDataNumb;

    QString                 m_dbConnID;
    QString                 m_dbHost;
    QString                 m_dbName;

    QMutex                  m_mutex;
    QList<SubWriteClass*>   m_subWriteObjList;
    QList<QThread*>         m_subWriteThreadList;
    int                     m_subWriteThreadNumb;

    int                     m_currCompleteThreadCount;
    int                     m_currSuccessNumb;
    QList<QString>          m_currWriteReuslt;

    int                     m_writeRealTimeDataCount;                   

    int                     m_testWriteStartTime;
    int                     m_testWriteSumTime;
    int                     m_testWriteCount;
    int                     m_testWriteAveTime;
};

#endif // REALTIMEDATAWRITE_H
