#include "dataprocess.h"
#include "toolfunc.h"
#include <QPointF>
#include <QDateTime>
#include <QString>

DataProcess::DataProcess(bool isRealTime, QMap<QString, QList<QStringList>> oridata,
                         QMap<QString, int> buyCount, QString hedgeIndexCode, QList<int> macdTime,
                         QObject *parent):
    m_isRealTime(isRealTime), m_oriData(oridata),
    m_seocdebuyCountMap(buyCount), m_hedgeIndexCode(hedgeIndexCode),
    m_macdTime(macdTime), QObject(parent)
{
    initIndexHedgeMetaInfo();
    m_secodeNameList = m_seocdebuyCountMap.keys();
    if (!m_isRealTime) {
        filterHedgeIndexData();
    }
}

DataProcess::DataProcess(bool isRealTime, QMap<QString, QList<QStringList>> oridata,
            QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy,
            QList<int> macdTime, QObject *parent):
    m_isRealTime(isRealTime), m_oriData(oridata),
    m_buyStrategyMap(buyStrategy), m_saleStrategyMap(saleStrategy),
    m_macdTime(macdTime), QObject(parent)
{

}

DataProcess::DataProcess(QMap<QString, QList<QStringList>> oridata,
            QMap<QString, int> buyCount, QList<int> macdTime, QObject *parent):
    m_oriData(oridata), m_seocdebuyCountMap(buyCount),
    m_macdTime(macdTime), QObject(parent)
{
    initIndexHedgeMetaInfo();
    filterHedgeIndexData();
}

void DataProcess::initIndexHedgeMetaInfo() {
    m_indexHedgeMetaInfo.insert("SH000300", 300);
    m_indexHedgeMetaInfo.insert("SH000016", 50);
    m_indexHedgeMetaInfo.insert("SH000852", 1000);
    m_indexHedgeMetaInfo.insert("SH000904", 200);
    m_indexHedgeMetaInfo.insert("SH000905", 500);
    m_indexHedgeMetaInfo.insert("SH000906", 800);
    m_indexHedgeMetaInfo.insert("SZ399903", 100);

    m_indexHedgeMetaInfo.insert("000300.SH", 300);
    m_indexHedgeMetaInfo.insert("000016.SH", 50);
    m_indexHedgeMetaInfo.insert("000852.SH", 1000);
    m_indexHedgeMetaInfo.insert("000904.SH", 200);
    m_indexHedgeMetaInfo.insert("000905.SH", 500);
    m_indexHedgeMetaInfo.insert("000906.SH", 800);
    m_indexHedgeMetaInfo.insert("399903.SZ", 100);
}

void DataProcess::filterHedgeIndexData() {
    QList<QStringList> tmpIndexHedgeData = m_oriData[m_hedgeIndexCode];
    m_oriData.remove(m_hedgeIndexCode);
    for (int i = 0; i < tmpIndexHedgeData.size (); ++i) {
        QStringList tmpKeyValue;
        tmpKeyValue << tmpIndexHedgeData[i][1] << tmpIndexHedgeData[i][2];
        QString dateTimeString = QDateTime::fromMSecsSinceEpoch(qint64(tmpIndexHedgeData[i][0].toLongLong())).toString ("yyyyMMddhhmmss");
        m_indexHedgeData.insert (dateTimeString, tmpKeyValue);
    }
}

void DataProcess::receiveOrigianlHistoryData (QString dataType) {
    if (dataType == "all") {
        if (m_isRealTime) {
            emit sendAllData(computeSnapshootData());
//            emit sendAllData(computeSnapshootDataReverse());
        } else {
            emit sendAllData(computeAllData());
        }
    } else {
        if (dataType == "strategy") {
            emit sendStrategyData (computeStrategyData());
        }
        if (dataType == "vot") {
            emit sendVotData (computeVotData());
        }
        if (dataType == "macd") {
            emit sendMACDData (computeMacdData());
        }
    }
}

QList<QList<double>> DataProcess::computeAllData() {
    QList<QList<double>> allData;
    computeStrategyData();
    computeVotData();
    computeMacdData();
    allData.append (m_timeData);
    allData.append (m_strategyData);
    allData.append (m_votData);
    allData.append (m_macdData);
    return allData;
}

QList<QList<double>> DataProcess::computeStrategyData () {
    QList<QList<double>> result;
    QList<QPointF> pointDataList;

    if (m_buyStrategyMap.size() != 0 && m_saleStrategyMap.size() != 0) {
        QList<QPointF> buyPointDataList = getStrategyPointList(m_oriData, m_buyStrategyMap);
        QList<QPointF> salePointDataList = getStrategyPointList(m_oriData, m_saleStrategyMap);
        pointDataList = getHedgedData(buyPointDataList, salePointDataList);
        for (int i  = 0; i < pointDataList.size(); ++i) {
            m_strategyData.append(pointDataList[i].y() / 300);
        }

        qDebug() << "buyPointDataList: " << buyPointDataList.size();
        qDebug() << "salePointDataList: " << salePointDataList.size();
        qDebug() << "pointDataList: " << pointDataList.size();
    } else {
        QMap<QString, int> seocdebuyCountMap = m_seocdebuyCountMap;
        seocdebuyCountMap.remove(m_hedgeIndexCode);
        pointDataList = getStrategyPointList(m_oriData, m_seocdebuyCountMap);
        m_strategyData = getHedgedData(pointDataList, m_indexHedgeData,
                                       m_seocdebuyCountMap[m_hedgeIndexCode],  m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }

    if (m_timeData.size() == 0) {
        for (int i = 0; i < pointDataList.size(); ++i) {
            m_timeData.append(pointDataList[i].x());
        }
    }
    result.append(m_timeData);
    result.append(m_strategyData);
    return result;
}

QList<QList<double>> DataProcess::computeVotData () {
    QList<QList<double>> result;
    QList<QString> keyList = m_oriData.keys ();
    QList<QPointF> pointDataList;
    for (int i = 0; i < keyList.size (); ++i) {
        QString key = keyList[i];
        QList<QStringList> tmpStringList = m_oriData[key];
        QList<QPointF> tmpPointData;
        for (int i = 0; i < tmpStringList.size (); ++i) {
            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(), tmpStringList[i][2].toDouble()));
        }
        tmpPointData = sortPointFList(tmpPointData);
        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
    }

    if (m_timeData.size() == 0) {
        for (int i = 0; i < pointDataList.size(); ++i) {
            m_timeData.append(pointDataList[i].x());
        }
    }

    for (int i = 0; i < pointDataList.size(); ++i) {
        m_votData.append(pointDataList[i].y());
    }

    result.append(m_timeData);
    result.append(m_votData);
    return result;
}

QList<QList<double>> DataProcess::computeMacdData () {
    QList<QList<double>> result;
    m_macdData = computeMACDDoubleData(m_strategyData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    result.append(m_timeData);
    result.append(m_macdData);
    return result;
}

// 逆序计算的原因: 实时数据的同时获取的数据各个股票的时间不一致，也无法排序进行计算，根据获取的时间顺序来进行计算;
QList<QList<double>> DataProcess::computeSnapshootData() {
//    qDebug()<< "DataProcess::computeSnapshootData begin!";
    QList<QString> secodeList = m_oriData.keys();

    QList<int> timeNumbList;
    int timeNumb = 1000000;
    for (int i = 0; i < secodeList.size(); ++i) {
        QString secode = secodeList[i];
        if (timeNumbList.indexOf(m_oriData[secode].size()) < 0) {
            timeNumbList.append(m_oriData[secode].size());
            if (timeNumb > m_oriData[secode].size()) {
                timeNumb = m_oriData[secode].size();
            }
        }
    }

    for (int i = timeNumb-1; i > -1; --i) {
        QMap<QString, QStringList> oneTimeData;
        for (int j = 0; j < secodeList.size(); ++j) {
            QString secode = secodeList[j];
            QStringList tmpData = m_oriData[secode][i];
            double currUpdateTime = 100;
            if (i > 0) {
                currUpdateTime = m_oriData[secode][i][1].toDouble() -  m_oriData[secode][i-1][1].toDouble();
            }
            if (currUpdateTime < 8 || currUpdateTime >= 2*60*60) {
                tmpData[4] = QString("%1").arg(m_oriData[secode][i][4].toDouble()
                                               - m_oriData[secode][i-1][4].toDouble());
            } else {
                tmpData[4] = QString("%1").arg(0);
            }
            oneTimeData.insert(secode, tmpData);
        }

        QList<double> tmpResult;
        if (m_buyStrategyMap.size() != 0 && m_saleStrategyMap.size() != 0) {
            tmpResult = getHedgedData(oneTimeData, m_buyStrategyMap, m_saleStrategyMap);
        } else {
            tmpResult = getHedgedData(oneTimeData, m_seocdebuyCountMap,
                                      m_hedgeIndexCode, m_seocdebuyCountMap[m_hedgeIndexCode],  m_indexHedgeMetaInfo[m_hedgeIndexCode]);
        }
        if (tmpResult.size() != 0 && tmpResult[1] != 0) {
            m_strategyData.prepend(tmpResult[0]);
            m_votData.prepend(tmpResult[1]);
            m_timeData.prepend(tmpResult[2]);
        }
    }

    QList<QList<double>> allData;
    allData.append (m_timeData);
    allData.append (m_strategyData);
    allData.append (m_votData);

    m_macdData = computeMACDDoubleData(m_strategyData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    allData.append (m_macdData);

    qDebug()<< "DataProcess::computeSnapshootDataReverse end!";
    return allData;
}

DataProcess::~DataProcess () {
//    qDebug() << "~DataProcess";
}

//// 逆序计算的原因: 实时数据的同时获取的数据各个股票的时间不一致，也无法排序进行计算，根据获取的时间顺序来进行计算;
//QList<QList<double>> DataProcess::computeSnapshootDataReverse() {
////    qDebug()<< "DataProcess::computeSnapshootData begin!";
//    QList<QString> secodeList = m_oriData.keys();

//    QList<int> timeNumbList;
//    int timeNumb = 1000000;
//    for (int i = 0; i < secodeList.size(); ++i) {
//        QString secode = secodeList[i];
//        if (timeNumbList.indexOf(m_oriData[secode].size()) < 0) {
//            timeNumbList.append(m_oriData[secode].size());
//            if (timeNumb > m_oriData[secode].size()) {
//                timeNumb = m_oriData[secode].size();
//            }
//        }
//    }

//    for (int i = timeNumb-1; i > -1; --i) {
//        QMap<QString, QStringList> oneTimeData;
//        for (int j = 0; j < secodeList.size(); ++j) {
//            QString secode = secodeList[j];
//            QStringList tmpData = m_oriData[secode][i];
//            double currUpdateTime = 100;
//            if (i > 0) {
//                currUpdateTime = m_oriData[secode][i][1].toDouble() -  m_oriData[secode][i-1][1].toDouble();
//            }
//            if (currUpdateTime < 8 || currUpdateTime >= 2*60*60) {
//                tmpData[4] = QString("%1").arg(m_oriData[secode][i][4].toDouble()
//                                               - m_oriData[secode][i-1][4].toDouble());
//            } else {
//                tmpData[4] = QString("%1").arg(0);
//            }
//            oneTimeData.insert(secode, tmpData);
//        }
//        computeChartDataReverse(oneTimeData);
//    }

//    QList<QList<double>> allData;
//    allData.append (m_timeData);
//    allData.append (m_strategyData);
//    allData.append (m_votData);

//    m_macdData = computeMACDDoubleData(m_strategyData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
//    allData.append (m_macdData);

//    qDebug()<< "DataProcess::computeSnapshootDataReverse end!";
//    return allData;
//}


//void DataProcess::computeChartDataReverse(QMap<QString, QStringList> oneTimeData) {
//    double strategyData = 0;
//    double votData = 0;
//    double timeData = 0;
//    MACD macdData;
//    int timeCount = 0;
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        QString secode = m_secodeNameList[i];
//        if (secode == m_hedgeIndexCode) {
//            continue;
//        }
//        if (oneTimeData[secode][2] == "0.0000") {
//            strategyData += oneTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
//        } else {
//            strategyData += oneTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
//        }
//        votData += oneTimeData[secode][4].toDouble();
//        timeData += oneTimeData[secode][5].toDouble();
//        ++timeCount;
//    }

//    if (votData == 0) return;

//    timeData = timeData / timeCount;
//    timeData = transDateTime(timeData);
//    strategyData = strategyData / (m_seocdebuyCountMap[m_hedgeIndexCode] * m_indexHedgeMetaInfo[m_hedgeIndexCode])
//                   - oneTimeData[m_hedgeIndexCode][2].toDouble();

//    m_strategyData.prepend(strategyData);
//    m_votData.prepend(votData);
//    m_timeData.prepend(timeData);
//}


//QList<QList<double>> DataProcess::computeSnapshootData() {
//    qDebug()<< "DataProcess::computeSnapshootData begin!";
//    QList<QString> secodeList = m_oriData.keys();

//    QList<int> timeNumbList;
//    int timeNumb = -1;
//    for (int i = 0; i < secodeList.size(); ++i) {
//        QString secode = secodeList[i];
//        if (timeNumbList.indexOf(m_oriData[secode].size()) < 0) {
//            timeNumbList.append(m_oriData[secode].size());
//            if (timeNumb < m_oriData[secode].size()) {
//                timeNumb = m_oriData[secode].size();
//            }
//        }
//    }
//    qDebug() << "timeNumbList: " << timeNumbList;
//    qDebug() << "timeNumb: " <<timeNumb;

//    for (int i = 0; i < timeNumb; ++i) {
//        QMap<QString, QStringList> oneTimeData;
//        for (int j = 0; j < secodeList.size(); ++j) {
//            QString secode = secodeList[j];
//            int t = i;
//            while (t >= m_oriData[secode].size()) --t;
//            QStringList tmpData = m_oriData[secode][t];
//            double currUpdateTime = m_oriData[secode][t][1].toDouble() -  m_oriData[secode][t-1][1].toDouble();
//            if (t > 0 && (currUpdateTime < 6) || currUpdateTime >= 2*60*60) {
//                tmpData[4] = QString("%1").arg(m_oriData[secode][t][4].toDouble()
//                                               - m_oriData[secode][t-1][4].toDouble());
//            } else {
//                tmpData[4] = QString("%1").arg(0);
//            }
//            oneTimeData.insert(secode, tmpData);
//        }
//        computeChartData(oneTimeData);
//    }

//    QList<QList<double>> allData;
//    allData.append (m_timeData);
//    allData.append (m_strategyData);
//    allData.append (m_votData);
//    allData.append (m_macdData);
//    qDebug()<< "DataProcess::computeSnapshootData end!";
//    return allData;
//}

//void DataProcess::computeChartData(QMap<QString, QStringList> oneTimeData) {
//    double strategyData = 0;
//    double votData = 0;
//    double timeData = 0;
//    MACD macdData;
//    int timeCount = 0;
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        QString secode = m_secodeNameList[i];
//        if (secode == m_hedgeIndexCode) {
//            continue;
//        }
//        if (oneTimeData[secode][2] == "0.0000") {
//            strategyData += oneTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
//        } else {
//            strategyData += oneTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
//        }
//        votData += oneTimeData[secode][4].toDouble();
//        timeData += oneTimeData[secode][5].toDouble();
//        ++timeCount;
//    }

//    if (votData == 0) return;

//    timeData = timeData / timeCount;
////    qDebug() << "timeData: " << timeData;
//    timeData = transDateTime(timeData);

////    qDebug() << "timeData: " << QDateTime::fromMSecsSinceEpoch(timeData);

//    strategyData = strategyData / (m_seocdebuyCountMap[m_hedgeIndexCode] * m_indexHedgeMetaInfo[m_hedgeIndexCode])
//                   - oneTimeData[m_hedgeIndexCode][2].toDouble();

//    if (m_MACDData.size() > 0) {
//        MACD latestData = m_MACDData[m_MACDData.size()-1];
//        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
//    } else {
//        macdData = MACD(strategyData, strategyData, 0, 0, 0);
//    }
//    m_strategyData.append(strategyData);
//    m_votData.append(votData);
//    m_timeData.append(timeData);
//    m_MACDData.append(macdData);
//    m_macdData << macdData.m_ema1 << macdData.m_ema2
//               << macdData.m_diff << macdData.m_dea << macdData.m_macd;
//}

//QList<QList<double>> DataProcess::computeStrategyData () {
//    QList<QList<double>> result;
//    QList<QString> keyList = m_oriData.keys ();
//    QList<QPointF> pointDataList;
//    for (int i = 0; i < keyList.size (); ++i) {
//        QString key = keyList[i];
//        QList<QStringList> tmpStringList = m_oriData[key];
//        QList<QPointF> tmpPointData;
//        for (int i = 0; i < tmpStringList.size (); ++i) {
//            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(), tmpStringList[i][1].toDouble() * m_seocdebuyCountMap[key]));
//        }
//        tmpPointData = sortPointFList(tmpPointData);
//        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
//    }

//    if (m_timeData.size() == 0) {
//        for (int i = 0; i < pointDataList.size(); ++i) {
//            m_timeData.append(pointDataList[i].x());
//        }
//    }

//    qDebug() << "pointDataList.size:    " << pointDataList.size ();
//    qDebug() << "m_indexHedgeData.size: " << m_indexHedgeData.size();

//    QList<QString> indexHedgedataTimeKey = m_indexHedgeData.keys ();
//    for (int i = 0; i < pointDataList.size(); ++i) {
//        QString timeKeyStr = QDateTime::fromMSecsSinceEpoch(qint64(pointDataList[i].x())).toString ("yyyyMMddhhmmss");
//        if (indexHedgedataTimeKey.indexOf (timeKeyStr) >= 0) {
//            m_strategyData.append(pointDataList[i].y() / (m_seocdebuyCountMap[m_hedgeIndexCode] * m_indexHedgeMetaInfo[m_hedgeIndexCode])
//                                  - m_indexHedgeData[timeKeyStr][0].toDouble());
//        } else {
//            m_strategyData.append(pointDataList[i].y() / (m_seocdebuyCountMap[m_hedgeIndexCode] * m_indexHedgeMetaInfo[m_hedgeIndexCode]));
//            qDebug() << "time not in indexHedgeData: " << timeKeyStr;
//        }
//    }

//    result.append(m_timeData);
//    result.append(m_strategyData);
//    return result;
//}
