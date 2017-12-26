#ifndef MONITORREALTIMEDATA_H
#define MONITORREALTIMEDATA_H
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "macd.h"
#include "chartdata.h"

class MonitorRealTimeData: public QObject
{
    Q_OBJECT
public:
    MonitorRealTimeData(QObject* parent = 0);
    MonitorRealTimeData(int monitorTime,  QList<int> macdTime,
                        QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
                        QString hedgeIndexCode, int hedgeIndexCount,
                        QObject* parent = 0);
    void startTimer();
    void stopTimer();
    void initIndexHedgeMetaInfo();
signals:
    void sendReadTimeData(ChartData data);

public slots:
    void startMonitorRealTimeData();
    void monitorRealTimeData();
    void updateData();

private:
    QMap<QString, QStringList> m_realTimeData;
    int m_monitorTime;
    QTimer m_timer;
    int m_hedgeIndexCount;
    QList<int> m_macdTime;

    QMap<QString, double> m_indexHedgeMetaInfo;
    QString m_hedgeIndexCode;
    QStringList m_secodeNameList;
    QMap<QString, int> m_seocdebuyCountMap;
    QList<MACD> m_macdData;
};

#endif // MONITORREALTIMEDATA_H
