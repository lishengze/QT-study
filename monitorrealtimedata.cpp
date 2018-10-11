#include "monitorrealtimedata.h"
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <algorithm>
#include "io_func.h"
#include "secode_func.h"
#include "time_func.h"
#include "compute_func.h"
#pragma execution_character_set("utf-8")

using namespace  std;

MonitorRealTimeData::MonitorRealTimeData(QString dbConnId, bool isBuySalePortfolio,
                                        QString dbhost, int monitorTime,  QList<int> macdTime,
                                        QString hedgeIndexCode, int hedgeIndexCount,
                                        QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
                                        QMap<QString, int> buyStrategyMap, QMap<QString, int> saleStrategyMap,
                                        bool isFuture, QObject* parent):
    m_dbConnId(dbConnId), m_isBuySalePortfolio(isBuySalePortfolio), m_dbhost(dbhost),
    m_monitorTime(monitorTime), m_macdTime(macdTime),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_seocdebuyCountMap(seocdebuyCountMap), m_secodeNameList(secodeNameList),
    m_buyStrategyMap(buyStrategyMap), m_saleStrategyMap(saleStrategyMap),
    m_isFuture(isFuture), QObject(parent)
{
    initCommonData();
}

MonitorRealTimeData::MonitorRealTimeData(QString dbConnId, QString dbhost, QString futureName,
                                         int monitorTime, bool isFuture, QObject* parent):
    m_dbConnId(dbConnId), m_dbhost(dbhost), m_futureName(futureName),
    m_monitorTime(monitorTime), m_isFuture(isFuture), QObject(parent)
{
    initTimer();
    initDatabase();
}

MonitorRealTimeData::MonitorRealTimeData(QString dbConnId, QString dbhost,
                                        QString selectIndex, QString hedgedIndex,
                                        int monitorTime, QObject* parent):
  m_dbConnId(dbConnId), m_dbhost(dbhost), m_monitorTime(monitorTime),
  m_selectIndex(selectIndex), m_hedgedIndex(hedgedIndex),
  QObject(parent)
{
    initDatabase();
    initIndexHedgeTimer();
}

MonitorRealTimeData::~MonitorRealTimeData() {
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
}

void MonitorRealTimeData::initCommonData() {
    initIndexHedgeMetaInfo();
    m_singleSecodeReadTime = 30;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QList<double> tmpVot;
        QList<QString> tmpTime;
        m_vot.insert(m_secodeNameList[i], tmpVot);
        m_time.insert(m_secodeNameList[i], tmpTime);
    }
    initTimer();
    initDatabase();
}

void MonitorRealTimeData::initDatabase() {
//    qDebug() << "MonitorRealTimeData, m_dbConnId: " << m_dbConnId << ", m_dbhost: " << m_dbhost;
    m_database = new Database(m_dbConnId, m_dbhost);
}

void MonitorRealTimeData::initTimer() {
    m_minWaitTime = 3000;
    if (m_isFuture) {
        m_monitorTime = m_minWaitTime;
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(setFutureData()));
    } else {
        m_monitorTime = max(m_singleSecodeReadTime * m_secodeNameList.size(), m_minWaitTime);
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(setRealTimeData()));
    }
}

void MonitorRealTimeData::initIndexHedgeTimer() {
//    qDebug() << "MonitorRealTimeData::initIndexHedgeTimer";
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(setIndexData()));
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

void MonitorRealTimeData::setInitMacd(MACD initMacdData) {
    m_macdData.append(initMacdData);
}

void MonitorRealTimeData::startTimer() {
    if (!m_timer.isActive()) {
        qDebug() << "MonitorRealTimeData::startTimer, m_monitorTime: " << m_monitorTime;
        m_timer.start(m_monitorTime);
    }
}

void MonitorRealTimeData::stopTimer() {
    if (m_timer.isActive()) {
        qDebug() << "MonitorRealTimeData::stopTimer";
        m_timer.stop();
    }
}

void MonitorRealTimeData::getPreCloseSpread() {
    qDebug() << "MonitorRealTimeData::getPreData" << QThread::currentThreadId();
    QMap<QString, QStringList> allPreCLoseData = m_database->getPreCloseData("PreCloseData");
//    printList(allPreCLoseData, "allPreCLoseData");
//    qDebug() << "m_isBuySalePortfolio: " << m_isBuySalePortfolio;
    double preCloseSpread;
    if (m_isBuySalePortfolio) {
        preCloseSpread = getHedgedSpread(allPreCLoseData, m_buyStrategyMap, m_saleStrategyMap);
    } else {
        preCloseSpread = getHedgedSpread(allPreCLoseData, m_seocdebuyCountMap,
                                         m_hedgeIndexCode, m_seocdebuyCountMap[m_hedgeIndexCode],
                                         m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }
    qDebug() << "preCloseSpread: " << preCloseSpread;
    emit sendPreCloseData(preCloseSpread);
}

void MonitorRealTimeData::setRealTimeData() {
    if (isStockTradingOver()) {
       emit sendTradeOver();
    }
    if (isStockTrading()) {
        QMap<QString, QStringList> oriRealTimeData = m_database->getSnapShootData(m_secodeNameList);
//         qDebug() << "oriRealTimeData.size: " << oriRealTimeData.size();
         printMap(oriRealTimeData, "oriRealTimeData");
        bool isDataUseful = preProcessRealTimeData(oriRealTimeData);
        if (isDataUseful) {
            emit sendRealTimeData(computeRealTimeData());
        }
    }
}

void MonitorRealTimeData::setFutureData() {
//    QList<double> result = m_database->getFutureSpread(m_futureName);
//    double datetime = result[1];
//    if (datetime > 0) {
//        emit sendFutureData_signal(result);
//    } else {
//        qDebug() << QString("获取实时期货基差失败");
//    }

    if (isStockTradingOver()) {
       emit sendTradeOver();
    }
    if (isStockTrading()) {
        QList<double> result = m_database->getFutureSpread(m_futureName);
        double datetime = result[1];
        if (datetime > 0) {
            emit sendFutureData_signal(result);
        } else {
            qDebug() << QString("获取实时期货基差失败");
        }
    }
}

void MonitorRealTimeData::setIndexData() {
    QStringList todayKeyValueList;
    todayKeyValueList << "日期" << "时间" << "最新成交价";
    QStringList selectData = m_database->getSnapShootData(getCompleteIndexCode(m_selectIndex, "wind"), todayKeyValueList);
    QStringList hedgedData = m_database->getSnapShootData(getCompleteIndexCode(m_hedgedIndex, "wind"), todayKeyValueList);
    emit sendRealTimeIndexData_signal(selectData, hedgedData);
}

void MonitorRealTimeData::getHistFutureData_slot() {
    QList<double> result = m_database->getHistFutureSpread(m_futureName);
    double datetime = result[1];
    if (datetime > 0) {
        emit sendHistFutureData_signal(result);
    } else {
        qDebug() << QString("获取历史期货基差失败");
    }
}

bool MonitorRealTimeData::preProcessRealTimeData(QMap<QString, QStringList> realTimeData) {
    int sameTimeCount = 0;
    double unUpdatedDataPercent = 0.8;
    if (realTimeData.find("Error") != realTimeData.end()) {
        return false;
    }
    for (QMap<QString, QStringList>::iterator it = realTimeData.begin();
         it != realTimeData.end(); ++it) {
        QString secode = it.key();

        m_vot[secode].append(realTimeData[secode][4].toDouble());
        m_time[secode].append(realTimeData[secode][1]);

        if (m_time[secode].size() > 1 && m_time[secode].last() == m_time[secode][m_time[secode].size()-2]) {
            ++sameTimeCount ;
        }

        m_realTimeData[secode] = realTimeData[secode];
        double currUpdateTime = 100;
        if(m_time[secode].size()>1) {
           currUpdateTime = m_time[secode].last().toDouble() - m_time[secode][m_time[secode].size()-2].toDouble();
        }
        if ( m_vot[secode].size() > 1 && (currUpdateTime < 8 || currUpdateTime >= 2*60*60)) {
            m_realTimeData[secode][4] = QString("%1").arg(m_vot[secode].last() - m_vot[secode][m_vot[secode].size()-2]);
        } else {
            m_realTimeData[secode][4] = QString("%1").arg(0);
        }
    }

//    qDebug() << "sameTimeCount: " << sameTimeCount;
    if (realTimeData.size() == m_secodeNameList.size() &&
        sameTimeCount < m_secodeNameList.size() * unUpdatedDataPercent ) {
        return true;
    } else {
        if (sameTimeCount >= m_secodeNameList.size() * unUpdatedDataPercent) {
            for (QMap<QString, QStringList>::iterator it = realTimeData.begin();
                 it != realTimeData.end(); ++it) {
                QString secode = it.key();
                if (!m_vot[secode].isEmpty()) {
                    m_vot[secode].pop_back();
                }
                if (!m_vot[secode].isEmpty()) {
                    m_time[secode].pop_back();
                }
            }
        }
        return false;
    }
}

ChartData MonitorRealTimeData::computeRealTimeData() {
    QList<double> result;
    if (m_isBuySalePortfolio) {
        result = getHedgedData(m_realTimeData, m_buyStrategyMap, m_saleStrategyMap);
    } else {
        result = getHedgedData(m_realTimeData, m_seocdebuyCountMap,
                               m_hedgeIndexCode, m_seocdebuyCountMap[m_hedgeIndexCode],
                               m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }

    double strategyData = result[0];
    double votData = result[1];
//    double timeData = result[2];
    double timeData = QDateTime::currentMSecsSinceEpoch();
    MACD macdData;

    if (m_macdData.size() > 0) {
        MACD latestData = m_macdData[m_macdData.size()-1];
        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    } else {
        macdData = MACD(strategyData, strategyData, 0, 0, 0);
    }
    m_macdData.append(macdData);

    ChartData curChartData(strategyData, votData, timeData, macdData);
    return curChartData;
}

void MonitorRealTimeData::getIndexRealtimeData_slot() {

}

/*
//void MonitorRealTimeData::computeChartData() {
//    double strategyData = 0;
//    double votData = 0;
//    double timeData = QDateTime::currentDateTime().toMSecsSinceEpoch();
//    MACD macdData;
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        QString secode = m_secodeNameList[i];
//        if (secode == m_hedgeIndexCode) {
//            continue;
//        }
//        if (m_realTimeData[secode][2] == "0.0000") {
//            strategyData += m_realTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
//        } else {
//            strategyData += m_realTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
//        }
//        votData += m_realTimeData[secode][4].toDouble();
//    }

//    strategyData = strategyData / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
//                   - m_realTimeData[m_hedgeIndexCode][2].toDouble();

//    if (votData == 0) {
//        qDebug() << "votData: " << votData <<", strategyData: " << strategyData;
//    }

//    if (m_macdData.size() > 0) {
//        MACD latestData = m_macdData[m_macdData.size()-1];
//        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
//    } else {
//        macdData = MACD(strategyData, strategyData, 0, 0, 0);
//    }

//    ChartData curChartData(strategyData, votData, timeData, macdData);
//    m_macdData.append(macdData);
//    emit sendRealTimeData(curChartData);
//}

//void MonitorRealTimeData::computePreCloseData(QMap<QString, QStringList> preCLose) {
//    double result = 0;
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        QString secode = m_secodeNameList[i];
//        if (secode != m_hedgeIndexCode) {
//            result += preCLose[secode][0].toDouble() * m_seocdebuyCountMap[secode];
//        }
//    }
//    result = result / (m_seocdebuyCountMap[m_hedgeIndexCode] * m_indexHedgeMetaInfo[m_hedgeIndexCode])
//            - preCLose[m_hedgeIndexCode][0].toDouble();

//    emit sendPreCloseData(result);
//}

//MonitorRealTimeData::MonitorRealTimeData(int monitorTime,  QList<int> macdTime,
//                                         QMap<QString, int> seocdebuyCountMap, QStringList secodeNameList,
//                                         QString hedgeIndexCode, int hedgeIndexCount,
//                                         QString dbConnId, QString dbhost,
//                                         QObject* parent):
//    m_monitorTime(monitorTime), m_macdTime(macdTime),
//    m_seocdebuyCountMap(seocdebuyCountMap), m_secodeNameList(secodeNameList),
//    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
//     m_dbConnId(dbConnId), m_dbhost(dbhost),
//    QObject(parent)
//{
//    initCommonData();
//}

//void MonitorRealTimeData::monitorRealTimeData() {
//    bool dataChange = false;
//    QList<QString> global_secodeList = g_wsqData.keys();
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//       QString secode = m_secodeNameList[i];
//       if (global_secodeList.indexOf(secode) >= 0) {
//           QList<QStringList> currData = g_wsqData[secode];
//           QStringList latestData = currData[currData.size()-1];
//           if (m_realTimeData[secode].size() == 0
//                   || latestData[1].toDouble() > m_realTimeData[secode][1].toDouble()) {
//               m_realTimeData[secode] = latestData;
//               m_realTimeData[secode][4] = QString("%1").arg(0);
//               if (currData.size() == 2) {
//                   m_realTimeData[secode][4] = QString("%1").arg(g_wsqData[secode][1][4].toDouble() - g_wsqData[secode][0][4].toDouble());
//               }
//               dataChange = true;
//           }
//       } else {
//           dataChange = false;
//           break;
//       }
//    }
//    if(dataChange) {
//        dataChange = false;
//        computeChartData();
//    }
//}
*/
