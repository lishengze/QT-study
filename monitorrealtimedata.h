#ifndef MONITORREALTIMEDATA_H
#define MONITORREALTIMEDATA_H
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "macd.h"
#include "chartdata.h"
#include "database.h"

class MonitorRealTimeData: public QObject
{
    Q_OBJECT
public:
    MonitorRealTimeData(QObject* parent = 0);
    MonitorRealTimeData(int monitorTime,  QList<int> macdTime,
                        QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
                        QString hedgeIndexCode, int hedgeIndexCount, bool bTestRealTime,
                        QString dbConnId, QString dbhost,
                        QObject* parent = 0);
    ~MonitorRealTimeData();

    void startTimer();
    void stopTimer();
    void initIndexHedgeMetaInfo();
    void preprecessRealTimeData(QMap<QString,QStringList> realTimeData);
    void computeChartData();

signals:
    void sendRealTimeData(ChartData data);
    void sendPreData(QMap<QString,QStringList>);

public slots:
    void monitorRealTimeData();
    void wsqRealTimeData();
    void getPreData();

private:
    QMap<QString, QStringList> m_realTimeData;
    int m_monitorTime;
    QTimer m_timer;
    int m_hedgeIndexCount;
    QList<int> m_macdTime;
    QMap<QString, QList<double>> m_vot;

    QMap<QString, double> m_indexHedgeMetaInfo;
    QString m_hedgeIndexCode;
    QStringList m_secodeNameList;
    QMap<QString, int> m_seocdebuyCountMap;
    QList<MACD> m_macdData;
    bool m_bTestRealTime;

    QString m_dbConnId;
    QString m_dbhost;
    Database* m_database;
};

#endif // MONITORREALTIMEDATA_H
