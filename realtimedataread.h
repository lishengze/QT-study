#ifndef REALTIMEDATAREAD_H
#define REALTIMEDATAREAD_H
#include <QTableView>
#include <QObject>
#include <QTimer>
#include "realtimedatabase.h"
#include "strategymodel.h"
#include "excel.h"

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

    void initTimer();
    void startTimer();
    void stopTimer();

    void setSecodeList();

    QMap<QString, QStringList> getSnapshootData();
    QMap<QString, QStringList> wsqSnapshootData(QList<QString> secodeList);

    void setPreCloseData();
    QMap<QString, QStringList> wsqPreCloseData();

    QList<QString> wsqRealTimeSecodeList();

public slots:
    void loginWind_slot();
    void setRealTimeData_slot();
    void writeComplete_slot();

signals:
    void loginWindFailed_signal();
    void loginWindSucc_signal();

    void setSecodeList_signal(QList<QString> data);

    void writePreCloseData_signal(QMap<QString,QStringList>);
    void writeRealTimeData_signal(QMap<QString,QStringList>);

    void stopReadRealTimeData_signal();
    void startReadRealTimeData_signal();

private:
    QTimer             m_timer;
    QString            m_dbConnID;
    QString            m_dbHost;
    QString            m_dbName;
    RealTimeDatabase*  m_realtimeDatabase;

    bool               m_isTimerStart;
    bool               m_login;
    bool               m_isTooEarly;
    bool               m_isRestTime;  
    QString            m_secodeList_IndexCode;
    QList<QString>     m_secodeList;
    QTableView*        m_programInfoTableView;
    QTableView*        m_errorMsgTableView;
    int                m_updateTime;
    int                m_wsqSnapshootDataNumb;
};

#endif // REALTIMEDATAREAD_H
