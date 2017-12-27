#include "monitorrealtimedata.h"
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include "toolfunc.h"

extern QMap<QString, QList<QStringList>> g_wsqData;
extern QMutex g_wsqMutex;

MonitorRealTimeData::MonitorRealTimeData(QObject* parent):
     m_monitorTime(3000), QObject(parent)
{
    initIndexHedgeMetaInfo();
//    connect(&m_timer, SIGNAL(timeout()), this, SLOT(monitorRealTimeData()));
}

MonitorRealTimeData::MonitorRealTimeData(int monitorTime,  QList<int> macdTime,
                                         QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
                                         QString hedgeIndexCode, int hedgeIndexCount,
                                         QObject* parent):
    m_monitorTime(monitorTime), m_macdTime(macdTime),
    m_seocdebuyCountMap(seocdebuyCountMap), m_secodeNameList(secodeNameList),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    QObject(parent)
{
    initIndexHedgeMetaInfo();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(monitorRealTimeData()));
}

void MonitorRealTimeData::initIndexHedgeMetaInfo() {
    m_indexHedgeMetaInfo.insert("000300.SH", 300);
    m_indexHedgeMetaInfo.insert("000016.SH", 50);
    m_indexHedgeMetaInfo.insert("000852.SH", 1000);
    m_indexHedgeMetaInfo.insert("000904.SH", 200);
    m_indexHedgeMetaInfo.insert("000905.SH", 500);
    m_indexHedgeMetaInfo.insert("000906.SH", 800);
    m_indexHedgeMetaInfo.insert("399903.SZ", 100);
}

void MonitorRealTimeData::startTimer() {
     m_timer.start(m_monitorTime);
}

void MonitorRealTimeData::stopTimer() {
     m_timer.stop();
}

void MonitorRealTimeData::monitorRealTimeData() {
    bool dataChange = false;
    QList<QString> global_secodeList = g_wsqData.keys();
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
       QString secode = m_secodeNameList[i];
       if (global_secodeList.indexOf(secode) >= 0) {
           QList<QStringList> currData = g_wsqData[secode];
           QStringList latestData = currData[currData.size()-1];
           if (m_realTimeData[secode].size() == 0
                   || latestData[1].toDouble() > m_realTimeData[secode][1].toDouble()) {
               m_realTimeData[secode] = latestData;
               m_realTimeData[secode][4] = QString("%1").arg(0);
               if (currData.size() == 2) {
                   m_realTimeData[secode][4] = QString("%1").arg(g_wsqData[secode][1][4].toDouble() - g_wsqData[secode][0][4].toDouble());
               }
               dataChange = true;
           }
       } else {
           dataChange = false;
           break;
       }
    }
    if(dataChange) {
        dataChange = false;
        updateData();
    }
}

void MonitorRealTimeData::updateData() {
//    qDebug() << "MonitorRealTimeData::updateData(): " << QThread::currentThread();
    double strategyData = 0;
    double votData = 0;
    double timeData = QDateTime::currentDateTime().toMSecsSinceEpoch();
    MACD macdData;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (secode == m_hedgeIndexCode) {
            continue;
        }
        if (m_realTimeData[secode][2] == "0.0000") {
            strategyData += m_realTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
        } else {
            strategyData += m_realTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
        }
        votData += m_realTimeData[secode][4].toDouble();
    }

    strategyData = strategyData / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
                   - m_realTimeData[m_hedgeIndexCode][2].toDouble();

    if (m_macdData.size() > 0) {
        MACD latestData = m_macdData[m_macdData.size()-1];
        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    } else {
        macdData = MACD(strategyData, strategyData, 0, 0, 0);
    }

    ChartData curChartData(strategyData, votData, timeData, macdData);
    m_macdData.append(macdData);
    emit sendReadTimeData(curChartData);
}

void MonitorRealTimeData::startMonitorRealTimeData() {
    qDebug() << "MonitorRealTimeData::startMonitorRealTimeData ";
}
