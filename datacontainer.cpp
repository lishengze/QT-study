#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QMetaObject>
#include "datacontainer.h"
#include "worker.h"

DataContainer::DataContainer()
{

}

DataContainer::DataContainer(QList<int> oridata, QObject *parent):
    m_oriData(oridata), QObject(parent)
{
    qRegisterMetaType<QList<int>>("QList<int>");
    Worker* worker = new Worker();
    worker->moveToThread (&m_thread);

    connect(&m_thread, SIGNAL(finished()), worker, SLOT(deleteLater()), Qt::DirectConnection);
    connect (this, SIGNAL(sendOriDataToWorker(QList<int>)), worker, SLOT(processData(QList<int>)), Qt::DirectConnection );
    connect (worker, SIGNAL(sendProcessedData(QList<int>)), this, SLOT(receiveProcessedData(QList<int>)), Qt::DirectConnection );

    m_thread.start ();

}

void DataContainer::start () {
    emit sendOriDataToWorker (m_oriData);
}

void DataContainer::receiveProcessedData (QList<int> processedData) {
    qDebug() << "processedData: " << processedData;
}

DataContainer::~DataContainer () {
    m_thread.quit ();
    m_thread.wait ();
}
