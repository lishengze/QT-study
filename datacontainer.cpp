#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QMetaObject>
#include "datacontainer.h"

DataContainer::DataContainer(QList<int> oridata, QObject *parent):
    m_oriData(oridata), QObject(parent)
{

}

void DataContainer::start() {
    QList<int> processedData = processData();
    emit sendProcessedDataToMaster (processedData);
}

void DataContainer::receiveTestData(QList<int> testData) {
    qDebug() << "DataContainer::receiveTestData: " << QThread::currentThreadId();
    start();
}

QList<int> DataContainer::processData () {
    qDebug() << "DataContainer::processData: " << QThread::currentThreadId();
    qDebug() << "oriData: " << m_oriData;
    QList<int> result;
    for (int i = 0; i < m_oriData.size (); ++i) {
        result.append (m_oriData[i] * 2);
    }
    return result;
}

DataContainer::~DataContainer () {
    qDebug() << "~DataContainer";
}
