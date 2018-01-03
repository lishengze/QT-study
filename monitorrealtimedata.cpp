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
}

MonitorRealTimeData::MonitorRealTimeData(int monitorTime,  QList<int> macdTime,
                                         QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
                                         QString hedgeIndexCode, int hedgeIndexCount, bool bTestRealTime,
                                         QString dbConnId, QString dbhost,
                                         QObject* parent):
    m_monitorTime(monitorTime), m_macdTime(macdTime),
    m_seocdebuyCountMap(seocdebuyCountMap), m_secodeNameList(secodeNameList),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_bTestRealTime(bTestRealTime), m_dbConnId(dbConnId), m_dbhost(dbhost),
    m_database(NULL),QObject(parent)
{
    initIndexHedgeMetaInfo();
    if (m_bTestRealTime) {
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(monitorRealTimeData()));
    } else {
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(wsqRealTimeData()));
    }

    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QList<double> tmpVot;
        QList<QString> tmpTime;
        m_vot.insert(m_secodeNameList[i], tmpVot);
        m_time.insert(m_secodeNameList[i], tmpTime);
    }
    m_database = new Database(m_dbConnId, m_dbhost);
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

void MonitorRealTimeData::getPreData() {
    qDebug() << "MonitorRealTimeData::getPreData";
    QMap<QString, QStringList> allPreCLoseData = m_database->getPreCloseData("PreCloseData");
    computePreCloseData(allPreCLoseData);
}

void MonitorRealTimeData::computePreCloseData(QMap<QString, QStringList> preCLose) {
    double result = 0;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (secode != m_hedgeIndexCode) {
            result += preCLose[secode][0].toDouble() * m_seocdebuyCountMap[secode];
        }
    }
    result = result / (m_seocdebuyCountMap[m_hedgeIndexCode] * m_indexHedgeMetaInfo[m_hedgeIndexCode])
            - preCLose[m_hedgeIndexCode][0].toDouble();

    emit sendPreCloseData(result);
}

void MonitorRealTimeData::wsqRealTimeData() {
//    qDebug() << "startWsqOneTime(m_secodeNameList): ";
//    preprecessRealTimeData(wsqSnaphootData(m_secodeNameList));
//      preprecessRealTimeData(m_database->getSnapShootData(m_secodeNameList));

    if (isTradingTime(QTime::currentTime())) {
//        preprecessRealTimeData(wsqSnaphootData(m_secodeNameList));
//        preprecessRealTimeData(m_database->getSnapShootData());
        preprecessRealTimeData(m_database->getSnapShootData(m_secodeNameList));
    } else {
        return;
    }
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
        computeChartData();
    }
}

void MonitorRealTimeData::preprecessRealTimeData(QMap<QString, QStringList> realTimeData) {
//    qDebug() << "realTimeData.size: " << realTimeData.size();
    QList<QString> realTimeDataSecodeList = realTimeData.keys();
    int sameTimeCount = 0;
    double unUpdatedDataPercent = 0.5;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (realTimeDataSecodeList.indexOf(secode) < 0) {
            qDebug() << "secode: " << secode;
            continue;
        }

        m_vot[secode].append(realTimeData[secode][4].toDouble());
        m_time[secode].append(realTimeData[secode][1]);

        if (m_time[secode].size() > 1 && m_time[secode].last() == m_time[secode][m_time[secode].size()-2]) {
            ++sameTimeCount ;
            qDebug() << secode << ": " << m_time[secode];
        }

        m_realTimeData[secode] = realTimeData[secode];
        if ( m_vot[secode].size() > 1) {
            m_realTimeData[secode][4] = QString("%1").arg(m_vot[secode].last() - m_vot[secode][m_vot[secode].size()-2]);
        } else {
            m_realTimeData[secode][4] = QString("%1").arg(0);
        }

        if (m_realTimeData[secode][4].toDouble() == 0.0 && m_vot[secode].size() > 1) {
//            qDebug() << secode <<" amt is 0: " << m_realTimeData[secode];
//            qDebug() << m_vot[secode];
//            qDebug() << m_time[secode];
//            qDebug() << "m_vot[secode].last() : " << m_vot[secode].last()
//                     << ",  m_vot[secode][m_vot[secode].size()-2]: " <<  m_vot[secode][m_vot[secode].size()-2];
        }
    }
    qDebug() << "sameTimeCount: " << sameTimeCount;
    if (sameTimeCount < m_secodeNameList.size() * unUpdatedDataPercent) {
        computeChartData();
    }
}

void MonitorRealTimeData::computeChartData() {
    double strategyData = 0;
    double votData = 0;
    double timeData = QDateTime::currentDateTime().toMSecsSinceEpoch();
    double preSpread = 0;
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
        preSpread += m_realTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
        votData += m_realTimeData[secode][4].toDouble();
    }

    strategyData = strategyData / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
                   - m_realTimeData[m_hedgeIndexCode][2].toDouble();

    preSpread = preSpread / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
                - m_realTimeData[m_hedgeIndexCode][3].toDouble();

    if (votData == 0) {
        qDebug() << "votData: " << votData <<", strategyData: " << strategyData;
    }

    if (m_macdData.size() > 0) {
        MACD latestData = m_macdData[m_macdData.size()-1];
        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    } else {
        macdData = MACD(strategyData, strategyData, 0, 0, 0);
    }

    ChartData curChartData(strategyData, votData, timeData, macdData, preSpread);
    m_macdData.append(macdData);
    emit sendRealTimeData(curChartData);
}

MonitorRealTimeData::~MonitorRealTimeData() {
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
}
