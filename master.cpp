#include <QDebug>
#include <QMetaType>
#include <QMutexLocker>
#include "master.h"
#include "datacontainer.h"

Master::Master(int dataNumb, int threadNumb, QObject *parent):
    m_processedThreadCount(0), m_dataNumb(dataNumb), m_threadNumb(threadNumb), QObject(parent)
{
    qRegisterMetaType<QList<int>>("QList<int>");
    setOriData();
}

void Master::setOriData () {
    for (int i = 0; i < m_dataNumb; ++i) {
        m_oriData.append (i);
    }
}

QList<QList<int>> Master::allocateThreadData() {
    QList<QList<int>> result;
    for (int i = 0; i < m_threadNumb; ++i) {
        QList<int> emptyList;
        result.append (emptyList);
    }

    for (int i = 0; i < m_oriData.size(); i+=m_threadNumb) {
        for (int j = 0; j < m_threadNumb && i + j < m_oriData.size(); ++j) {
            result[i].append (m_oriData[i+j]);
        }
    }
    return result;
}

void Master::startProcessData (){
    QList<QList<int>> oriThreadData = allocateThreadData ();
    for (int i = 0; i < oriThreadData.size (); ++i) {
        DataContainer* curDataContainer = new DataContainer(oriThreadData[i]);
        connect (curDataContainer, SIGNAL(sendProcessedDataToMaster(QList<int>)), this, SLOT(receiveProcessedData(QList<int>)));
        curDataContainer->start ();
        m_dataContainerList.append (curDataContainer);

//        QThread curThread;
//        Worker curWorker;
//        curWorker.moveToThread (&curThread);

//        connect (this, SIGNAL(sendOriData(QList<int>)), &curWorker, SLOT(processData(QList<int>)));
//        connect (&curWorker, SIGNAL(sendProcessedData(QList<int>)), this, SLOT(receiveProcessedData(QList<int>)) );
//        curThread.start ();
    }
}

void Master::receiveProcessedData (QList<int> subThreadData) {
    QMutexLocker locker(&m_mutex);
    m_processedData += subThreadData;
    ++m_processedThreadCount;
    qDebug() << "m_processedThreadCount: " << m_processedThreadCount;
    if (m_processedThreadCount == m_threadNumb) {
        qDebug() << "All subThread process over";
        qDebug() << "Processed Data: " << m_processedData;
    }
}

Master::~Master () {

}
