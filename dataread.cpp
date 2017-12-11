#include "dataread.h"

DataRead::DataProcess(QList<int> oridata, QObject *parent):
    m_oriData(oridata), QObject(parent)
{

}

void DataRead::receiveStartReadData(QString dataType) {
    qDebug() << "DataContainer::receiveTestData: " << QThread::currentThreadId();
    if (dataType == "HistoryData") {
        emit sendHistoryData (readHistoryData());
    }
    if (dataType == "RealTimeData") {
        emit sendRealTimeData (readRealTimeData());
    }

}

QList<int> DataRead::readHistoryData () {
    qDebug() << "DataContainer::processData: " << QThread::currentThreadId();
    qDebug() << "oriData: " << m_oriData;
    QList<int> result;
    return result;
}

QList<int> DataRead::readRealTimeData () {
    qDebug() << "DataContainer::processData: " << QThread::currentThreadId();
    qDebug() << "oriData: " << m_oriData;
    QList<int> result;
    return result;
}

DataRead::~DataRead () {
    qDebug() << "~DataContainer";
}
