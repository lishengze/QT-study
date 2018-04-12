#ifndef REALTIMEDATAREAD_H
#define REALTIMEDATAREAD_H
#include <QTableView>
#include <QObject>
#include <QTimer>
#include "realtimedatabase.h"
//#include "strategymodel.h"
//#include "excel.h"

class RealTimeDataRead:public QObject
{
    Q_OBJECT
public:
    RealTimeDataRead(QTableView* programInfoTableView, QTableView* errorMsgTableView,
                     QString dbConnID, QString dbHost,
                     QObject* parent = 0);

    ~RealTimeDataRead();
    void initCommonData();
    void initDatabase();

    void initMonitorTimer();
    void startMonitorTimer();
    void stopMonitorTimer();

    void initTimer();
    void setUpdateTime();
    void startTimer();
    void stopTimer();

    void resetReadSource();

    QList<QString> getSecodeList();
    QList<QString> getFutureList();

    QMap<QString, QStringList> getSnapshootData();
    QMap<QString, QStringList> wsqSnapshootData(QList<QString> secodeList);
    QMap<QString, QStringList> wsqFutureData(QList<QString> futureList);

    void setPreCloseData();
    QMap<QString, QStringList> wsqPreCloseData();

    QList<QString> wsqRealTimeSecodeList();

public slots:
    void loginWind_slot();

    void setTableListComplete_slot();
    void setPrecloseDataComplete_slot();

    void setRealTimeData_slot();
    void writeComplete_slot(int);
    void monitorException_slot();

signals:
    void loginWindFailed_signal();
    void loginWindSucc_signal();

    void setTableList_signal(QList<QString> secodeList, QList<QString> futureList);

    void writePreCloseData_signal(QMap<QString,QStringList>);
    void writeRealTimeData_signal(QMap<QString,QStringList>, QMap<QString,QStringList>);

    void startMonitorTimer_signal();
    void stopMonitorTimer_signal();

    void stopWaitTradeTimer_signal();
    void startWaitTradeTimer_signal();

private:
    QTimer             m_timer;
    int                m_minReadWaitTime;
    int                m_signalWriteTime;
    int                m_updateTime;

    QString            m_dbConnID;
    QString            m_dbHost;
    QString            m_dbName;
    RealTimeDatabase*  m_realtimeDatabase;

    QTimer             m_monitorExceptionTimer;
    int                m_monitorExceptionWaitTime;
    bool               m_currProccessState;

    bool               m_isTimerStart;
    bool               m_login;
    bool               m_isTooEarly;
    bool               m_isRestTime;  
    QString            m_secodeList_IndexCode;
    QList<QString>     m_secodeList;
    QList<QString>     m_futureList;
    QTableView*        m_programInfoTableView;
    QTableView*        m_errorMsgTableView;

    int                m_wsqSnapshootDataNumb;
    double             m_usefulReadRate;
};

#endif // REALTIMEDATAREAD_H
