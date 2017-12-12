#include "dataprocess.h"
#include "toolfunc.h"
#include <QPointF>

DataProcess::DataProcess(QMap<QString, QList<QStringList>>oridata, QMap<QString, int> buyCount, QObject *parent):
    m_oriData(oridata), m_buyCount(buyCount), QObject(parent)
{

}

void DataProcess::receiveStartProcessData (QString dataType) {
    qDebug() << "DataProcess::receiveStartProcessData: " << QThread::currentThreadId();
    if (dataType == "all") {
        emit sendAllData(computeAllData());
    }
    if (dataType == "strategy") {
        emit sendStrategyData (computeStrategyData());
    }
    if (dataType == "vot") {
        emit sendVotData (computeVotData());
    }
    if (dataType == "macd") {
        emit sendMACDData (computeMACDData());
    }
    emit (sendTimeData(computeTimeData()));
}

QList<QList<double>> DataProcess::computeAllData() {
    QList<QList<double>> allData;
    allData.append (computeTimeData());
    allData.append (computeStrategyData());
    allData.append (computeVotData());
    allData.append (computeMACDData());
    return allData;
}

QList<double> DataProcess::computeStrategyData () {
    QList<double> result;
    QList<QString> keyList;
    QList<QPointF> pointDataList;
    for (int i = 0; i < keyList.size (); ++i) {
        QString key = keyList[i];
        QList<QStringList> tmpStringList = m_oriData[key];
        QList<QPointF> tmpPointData;
        for (int i = 0; i < tmpStringList.size (); ++i) {
            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble, tmpStringList[i][1].toDouble * m_buyCount[key]));
        }
        tmpPointData = sortPointFList(tmpPointData);
        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
    }
    return result;
}

QList<double> DataProcess::computeVotData () {
    QList<double> result;
    return result;
}

QList<double> DataProcess::computeMACDData () {
    QList<double> result;
    return result;
}

QList<double> DataProcess::computeTimeData() {
    QList<double> result;
    return result;
}

DataProcess::~DataProcess () {
    qDebug() << "~DataProcess";
}
