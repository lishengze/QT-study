#include "dataprocess.h"
#include "toolfunc.h"
#include <QPointF>
#include <QDateTime>
#include <QString>

DataProcess::DataProcess(QMap<QString, QList<QStringList>>oridata,
                         QMap<QString, int> buyCount, QList<int> macdTime,
                         QObject *parent):
    m_oriData(oridata), m_buyCount(buyCount), m_macdTime(macdTime),
    QObject(parent)
{
    m_indexHedgeMetaInfo.insert("SH000300", 300);
    m_indexHedgeMetaInfo.insert("SH000016", 50);
    m_indexHedgeMetaInfo.insert("SH000852", 1000);
    m_indexHedgeMetaInfo.insert("SH000904", 200);
    m_indexHedgeMetaInfo.insert("SH000905", 500);
    m_indexHedgeMetaInfo.insert("SH000906", 800);
    m_indexHedgeMetaInfo.insert("SZ399903", 100);

    filterIndexHedegeData();
}

void DataProcess::filterIndexHedegeData() {
    QList<QString> indexHedgeCode = m_indexHedgeMetaInfo.keys();
    QList<QString> secodeList = m_oriData.keys();
    QList<QStringList> tmpIndexHedgeData;
    for (int i = 0; i < indexHedgeCode.size(); ++i) {
        if (secodeList.indexOf(indexHedgeCode[i]) >= 0) {
            qDebug() << "hedgeIndexcode: " << indexHedgeCode[i];
            m_indexHedgeCode = indexHedgeCode[i];
            tmpIndexHedgeData = m_oriData[indexHedgeCode[i]];
            m_indexHedgeCount = m_indexHedgeMetaInfo[indexHedgeCode[i]];
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
        emit sendAllData(computeAllData());
    } else {
        if (dataType == "strategy") {
            emit sendStrategyData (computeStrategyData());
        }
        if (dataType == "vot") {
            emit sendVotData (computeVotData());
        }
        if (dataType == "macd") {
            emit sendMACDData (computeMACDData());
        }
    }
}

QList<QList<double>> DataProcess::computeAllData() {
    QList<QList<double>> allData;
    computeStrategyData();
    computeVotData();
    computeMACDData();
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
            m_strategyData.append(pointDataList[i].y() / (m_buyCount[m_indexHedgeCode] * m_indexHedgeCount) - m_indexHedgeData[timeKeyStr][0].toDouble());
        } else {
            m_strategyData.append(pointDataList[i].y() / (m_buyCount[m_indexHedgeCode] * m_indexHedgeCount));
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

QList<QList<double>> DataProcess::computeMACDData () {
    QList<QList<double>> result;
    m_macdData = computeMACDDoubleData(m_strategyData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    result.append(m_timeData);
    result.append(m_macdData);
    return result;
}


DataProcess::~DataProcess () {
//    qDebug() << "~DataProcess";
}
