#ifndef MONITORREALTIMEDATA_H
#define MONITORREALTIMEDATA_H
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "toolfunc.h"
#include "macd.h"
#include "chartdata.h"
#include "database.h"

extern const int g_maxFutureSpread;

class MonitorRealTimeData: public QObject
{
    Q_OBJECT
public:
    MonitorRealTimeData(QString dbConnId, bool isBuySalePortfolio,
                        QString dbhost, int monitorTime,  QList<int> macdTime,
                        QString hedgeIndexCode, int hedgeIndexCount,
                        QMap<QString, int> seocdebuyCountMap=EmpytQStringIntMap(),
                        QStringList secodeNameList=EmpytStringList(),
                        QMap<QString, int> buyStrategyMap = EmpytQStringIntMap(),
                        QMap<QString, int> saleStrategyMap = EmpytQStringIntMap(),
                        bool isFuture = false, QObject* parent = 0);

    MonitorRealTimeData(QString dbConnId, QString dbhost, QString futureName,
                        int monitorTime, bool isFuture = true, QObject* parent = 0);

    ~MonitorRealTimeData();

    void initCommonData();
    void initDatabase();
    void initTimer();
    void startTimer();
    void stopTimer();
    void initIndexHedgeMetaInfo();
    void setInitMacd(MACD initMacdData);

    bool preProcessRealTimeData(QMap<QString,QStringList> realTimeData);
    ChartData computeRealTimeData();
signals:
    void sendRealTimeData(ChartData data);
    void sendPreCloseData(double);

    void sendHistFutureData_signal(QList<double>);
    void sendFutureData_signal(QList<double>);

    void sendTradeOver();
    void startMonitorTimer_signal();
    void stopMonitorTimer_signal();

public slots:
    void setRealTimeData();
    void getPreCloseSpread();
    void setFutureData();
    void getHistFutureData_slot();

private:
    QString                         m_dbConnId;
    QString                         m_dbhost;
    Database*                       m_database;
    bool                            m_isBuySalePortfolio;
    bool                            m_isFuture;
    QString                         m_futureName;

    QMap<QString, QStringList>      m_realTimeData;
    int                             m_monitorTime;
    int                             m_singleSecodeReadTime;
    int                             m_minWaitTime;
    QTimer                          m_timer;
    int                             m_hedgeIndexCount;
    QList<int>                      m_macdTime;
    QMap<QString, QList<double>>    m_vot;
    QMap<QString, QList<QString>>   m_time;

    QMap<QString, double>           m_indexHedgeMetaInfo;
    QString                         m_hedgeIndexCode;
    QStringList                     m_secodeNameList;
    QMap<QString, int>              m_seocdebuyCountMap;

    QMap<QString, int>              m_buyStrategyMap;
    QMap<QString, int>              m_saleStrategyMap;

    QList<MACD>                     m_macdData;
};
#endif // MONITORREALTIMEDATA_H
