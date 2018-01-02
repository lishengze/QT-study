#include "dataprocess.h"
#include "toolfunc.h"
#include <QPointF>
#include <QDateTime>
#include <QString>

DataProcess::DataProcess(QMap<QString, QList<QStringList>>oridata,
                         QMap<QString, int> buyCount, QList<int> macdTime,
                         bool isRealTime, QObject *parent):
    m_oriData(oridata), m_buyCount(buyCount), m_macdTime(macdTime),
    m_isRealTime(isRealTime), QObject(parent)
{
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

    filterIndexHedegeData();
}

void DataProcess::filterIndexHedegeData() {
    QList<QString> indexHedgeCode = m_indexHedgeMetaInfo.keys();
    QList<QString> secodeList = m_oriData.keys();
    QList<QStringList> tmpIndexHedgeData;
    for (int i = 0; i < indexHedgeCode.size(); ++i) {
        if (secodeList.indexOf(indexHedgeCode[i]) >= 0) {
            qDebug() << "hedgeIndexcode: " << indexHedgeCode[i];
            m_hedgeIndexCode = indexHedgeCode[i];
            tmpIndexHedgeData = m_oriData[indexHedgeCode[i]];
            m_hedgeIndexCount = m_indexHedgeMetaInfo[indexHedgeCode[i]];
            m_oriData.remove(indexHedgeCode[i]);
            break;
        }
    }
    for (int i = 0; i < tmpIndexHedgeData.size (); ++i) {
        QStringList tmpKeyValue;
        tmpKeyValue << tmpIndexHedgeData[i][1] << tmpIndexHedgeData[i][2];
        QString dateTimeString = QDateTime::fromMSecsSinceEpoch(qint64(tmpIndexHedgeData[i][0].toLongLong())).toString ("yyyyMMddhhmmss");
        m_indexHedgeData.insert (dateTimeString, tmpKeyValue);
    }
}

void DataProcess::receiveStartProcessData (QString dataType) {
//    qDebug() << "DataProcess::receiveStartProcessData: " << QThread::currentThreadId();
    if (dataType == "all") {
        if (m_isRealTime) {
            emit sendAllData(computeSnapshootData());
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
    QList<QString> keyList = m_oriData.keys ();
    QList<QPointF> pointDataList;
    for (int i = 0; i < keyList.size (); ++i) {
        QString key = keyList[i];
        QList<QStringList> tmpStringList = m_oriData[key];
        QList<QPointF> tmpPointData;
        for (int i = 0; i < tmpStringList.size (); ++i) {
            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(), tmpStringList[i][1].toDouble() * m_buyCount[key]));
        }
        tmpPointData = sortPointFList(tmpPointData);
        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
    }
    if (m_timeData.size() == 0) {
        for (int i = 0; i < pointDataList.size(); ++i) {
            m_timeData.append(pointDataList[i].x());
        }
    }

    qDebug() << "pointDataList.size:    " << pointDataList.size ();
    qDebug() << "m_indexHedgeData.size: " << m_indexHedgeData.size();

    QList<QString> indexHedgedataTimeKey = m_indexHedgeData.keys ();
    for (int i = 0; i < pointDataList.size(); ++i) {
        QString timeKeyStr = QDateTime::fromMSecsSinceEpoch(qint64(pointDataList[i].x())).toString ("yyyyMMddhhmmss");
        if (indexHedgedataTimeKey.indexOf (timeKeyStr) >= 0) {
            m_strategyData.append(pointDataList[i].y() / (m_buyCount[m_hedgeIndexCode] * m_hedgeIndexCount) - m_indexHedgeData[timeKeyStr][0].toDouble());
        } else {
            m_strategyData.append(pointDataList[i].y() / (m_buyCount[m_hedgeIndexCode] * m_hedgeIndexCount));
            qDebug() << "time not in indexHedgeData: " << timeKeyStr;
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

QList<QList<double>> DataProcess::computeSnapshootData() {
    QList<QString> secodeList = m_oriData.keys();
    int timeNumb = m_oriData[secodeList[0]].size();
    for (int i = 0; i < timeNumb; ++i) {
        QMap<QString, QStringList> onTimeData;
        for (int j = 0; j < secodeList.size(); ++j) {
            QString secode = secodeList[j];
            onTimeData.insert(secode, m_oriData[secode][i]);
        }
        computeChartData(oneTimeData);
    }
    QList<QList<double>> allData;
    allData.append (m_timeData);
    allData.append (m_strategyData);
    allData.append (m_votData);
    allData.append (m_macdData);
    return allData;
}

void DataProcess::computeChartData(QMap<QString, QStringList> oneTimeData) {
    double strategyData = 0;
    double votData = 0;
    double timeData = QDateTime::currentDateTime().toMSecsSinceEpoch();
    MACD macdData;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (secode == m_hedgeIndexCode) {
            continue;
        }
        if (oneTimeData[secode][2] == "0.0000") {
            strategyData += oneTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
        } else {
            strategyData += oneTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
        }
        votData += oneTimeData[secode][4].toDouble();
    }

    strategyData = strategyData / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
                   - oneTimeData[m_hedgeIndexCode][2].toDouble();

    if (m_MACDData.size() > 0) {
        MACD latestData = m_MACDData[m_macdData.size()-1];
        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    } else {
        macdData = MACD(strategyData, strategyData, 0, 0, 0);
    }
    m_strategyData.append(strategyData);
    m_votData.append(votData);
    m_timeData.append(timeData);
    m_MACDData.append(macdData);
    m_macdData << macdData.m_ema1 << macdData.m_ema2
               << macdData.m_diff << macdData.m_dea << macdData.m_macd;
}

DataProcess::~DataProcess () {
//    qDebug() << "~DataProcess";
}
