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
    RealTimeDataRead(QTableView* programInfoTableView, QString m_dirName,
                     int updateTime = 3000, QObject* parent = 0);

    ~RealTimeDataRead();

    void initDatabase();
    void initTimer();
    void startTimer();
    void stopTimer();
    void setSecodeList();
    void insertSnapshootData(QMap<QString, QStringList> data);

public slots:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);
    void cancelWsqRequest(int reqID);
    void cancelAllWsqRequest();
    void getPreData(QList<QString> secodeList);
    void wsqSnapshootData();

signals:
    void loginWindFailed(int errcode);
    void loginWindSucc();
    void startWsqFailed(int errcode, int reqID);
    void startWsqSucc();
    void sendPreData(QMap<QString,QStringList>);
    void sendRealTimeData(QMap<QString,QStringList>);

private:
    RealTimeDatabase* m_realtimeDatabase;
    StrategyModel* m_strategyModel;
    Excel* m_excel;

    QTimer m_timer;
    int m_updateTime;
    bool m_login;
    QString m_strategyFileDir;
    QList<QString> m_secodeList;
    QTableView* m_programInfoTableView;
};

#endif // REALTIMEDATAREAD_H
