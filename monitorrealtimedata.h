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

class MonitorRealTimeData: public QObject
{
    Q_OBJECT
public:
    MonitorRealTimeData(QString dbConnId, QString dbhost,
                        int monitorTime,  QList<int> macdTime,
                        QString hedgeIndexCode, int hedgeIndexCount,
                        QMap<QString, int> seocdebuyCountMap=EmpytQStringIntMap(),
                        QStringList secodeNameList=EmpytStringList(),
                        QMap<QString, int> buyStrategyMap = EmpytQStringIntMap(),
                        QMap<QString, int> saleStrategyMap = EmpytQStringIntMap(),
                        QObject* parent = 0);

    ~MonitorRealTimeData();

    void initCommonData();
    void startTimer();
    void stopTimer();
    void initIndexHedgeMetaInfo();
    void setInitMacd(MACD initMacdData);

    bool preProcessRealTimeData(QMap<QString,QStringList> realTimeData);
    ChartData computeRealTimeData();

signals:
    void sendRealTimeData(ChartData data);
    void sendPreCloseData(double);
    void sendTradeOver();

public slots:
    void getRealTimeData();
    void getPreCloseSpread();

private:
    QMap<QString, QStringList> m_realTimeData;
    int m_monitorTime;
    QTimer m_timer;
    int m_hedgeIndexCount;
    QList<int> m_macdTime;
    QMap<QString, QList<double>> m_vot;
    QMap<QString, QList<QString>> m_time;

    QMap<QString, double> m_indexHedgeMetaInfo;
    QString m_hedgeIndexCode;
    QStringList m_secodeNameList;
    QMap<QString, int> m_seocdebuyCountMap;

    QMap<QString, int> m_buyStrategyMap;
    QMap<QString, int> m_saleStrategyMap;

    QList<MACD> m_macdData;

    QString m_dbConnId;
    QString m_dbhost;
    Database* m_database;
};

//    void computePreCloseData(QMap<QString, QStringList> allPreCLoseData);
//    void computeChartData();

//    MonitorRealTimeData(int monitorTime,  QList<int> macdTime,
//                        QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
//                        QString hedgeIndexCode, int hedgeIndexCount,
//                        QString dbConnId, QString dbhost,
//                        QObject* parent = 0);

#endif // MONITORREALTIMEDATA_H
