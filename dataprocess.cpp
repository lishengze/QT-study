#include "dataprocess.h"

DataProcess::DataProcess(QList<int> oridata, QObject *parent = Q_NULLPTR):
    m_oriData(oridata)
{

}

void DataProcess::receiveStartProcessData (QString dataType) {
    if (dataType == "all") {
        emit sendAllData (computeAllData());
    }
    if (dataType == "strategy") {
        emit sendAllData (computeStrategyData());
    }
    if (dataType == "vot") {
        emit sendAllData (computeVotData());
    }
    if (dataType == "macd") {
        emit sendAllData (computeMACDData());
    }
}

QList<int> DataProcess::computeAllData () {
    QList<int> result;
    return result;
}

QList<int> DataProcess::computeStrategyData () {
    QList<int> result;
    return result;
}

QList<int> DataProcess::computeVotData () {
    QList<int> result;
    return result;
}

QList<int> DataProcess::computeMACDData () {
    QList<int> result;
    return result;
}
