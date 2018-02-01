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
    RealTimeDataRead(QTableView* programInfoTableView, QObject* parent = 0);
    RealTimeDataRead(QTableView* programInfoTableView, QTableView* errorMsgTableView,QString m_dirName,
                     QList<QString> secodeList, Excel* excel,
                     int updateTime = 3000, QObject* parent = 0);

    ~RealTimeDataRead();

    void initComEnv();
    void initDatabase();
    void clearDatabase();
    void setPreCloseData();
    void initTimer();
    void startTimer();
    void setSecodeList();
    void insertSnapshootData(QMap<QString, QStringList> data);
    QMap<QString, QStringList> wsqSnapshootData();
    QMap<QString, QStringList> wsqPreCloseData();

public slots:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);
    void cancelWsqRequest(int reqID);
    void cancelAllWsqRequest();
    void getPreData(QList<QString> secodeList);
    void getRealTimeData();
    void stopTimer();

signals:
    void loginWindFailed(int errcode);
    void loginWindSucc();
    void startWsqFailed(int errcode, int reqID);
    void startWsqSucc();
    void sendPreData(QMap<QString,QStringList>);
    void sendRealTimeData(QMap<QString,QStringList>);
    void sendStopTimerSignal();

private:
    RealTimeDatabase* m_realtimeDatabase;
    StrategyModel* m_strategyModel;
    Excel* m_excel;

    QTimer m_timer;
    int m_updateTime;
    bool m_login;
    QString m_strategyFileDir;
    QList<QString> m_secodeList;
    QList<QString> m_strategyFileList;
    QTableView* m_programInfoTableView;
    QTableView* m_errorMsgTableView;
    int m_realtimeDataNumb;
};

#endif // REALTIMEDATAREAD_H
