#include "dataprocess.h"
#include "toolfunc.h"
#include <QPointF>

DataProcess::DataProcess(QMap<QString, QList<QStringList>>oridata, QMap<QString, int> buyCount, QList<int> macdTime, QObject *parent):
    m_oriData(oridata), m_buyCount(buyCount), m_macdTime(macdTime), QObject(parent)
{
    m_indexHedgeMetaInfo.insert("SH000300", 2*300);
    m_indexHedgeMetaInfo.insert("SH000016", 2*50);
    m_indexHedgeMetaInfo.insert("SH000852", 2*1000);
    m_indexHedgeMetaInfo.insert("SH000904", 2*200);
    m_indexHedgeMetaInfo.insert("SH000905", 2*500);
    m_indexHedgeMetaInfo.insert("SH000906", 2*800);
    m_indexHedgeMetaInfo.insert("SZ399903", 2*100);
}

void DataProcess::filterIndexHedegeData() {
    QList<QString> indexHedgeCode = m_indexHedgeMetaInfo.keys();
    QList<QString> secodeList = m_oriData.keys();

    for (int i = 0; i < indexHedgeCode.size(); ++i) {
        if (secodeList.indexOf(indexHedgeCode[i]) > 0) {
            qDebug() << "indexcode: " << indexHedgeCode[i];
            m_indexHedgeData = m_oriData[indexHedgeCode[i]];
            m_indexHedgeCount = m_indexHedgeMetaInfo[indexHedgeCode[i]];
            m_oriData.remove(indexHedgeCode[i]);
            break;
        }
    }
}

void DataProcess::receiveStartProcessData (QString dataType) {
    qDebug() << "DataProcess::receiveStartProcessData: " << QThread::currentThreadId();
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

QList<double> DataProcess::computeStrategyData () {
    QList<QList<double>> result;
    QList<QString> keyList;
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

    for (int i = 0; i < pointDataList.size(); ++i) {
        m_strategyData.append(pointDataList[i].y() / m_indexHedgeCount - m_indexHedgeData[i][1].toDouble());
    }

    result.append(m_timeData);
    result.append(m_strategyData);
    return result;
}

QList<QList<double>> DataProcess::computeVotData () {
    QList<QList<double>> result;
    QList<QString> keyList;
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
    m_macdData = computeMACD(m_strategyData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    result.append(m_timeData);
    result.append(m_macdData);
    return result;
}


DataProcess::~DataProcess () {
    qDebug() << "~DataProcess";
}
