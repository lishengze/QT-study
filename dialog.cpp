#include <QDebug>
#include <QMetaType>
#include <QMutexLocker>
#include <QMessageBox>
#include "datacontainer.h"
#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::Dialog(int dataNumb, int threadNumb, QWidget *parent):
    QDialog(parent),
    ui(new Ui::Dialog),
    m_processedThreadCount(0), m_dataNumb(dataNumb), m_threadNumb(threadNumb)
{
    ui->setupUi(this);
    qRegisterMetaType<QList<int>>("QList<int>");
//    connect (this, SIGNAL(sendCompleteData(QList<int>)), this, SLOT(receiveCompleteData(QList<int>)),  Qt::DirectConnection);
    connect (this, SIGNAL(sendCompleteData(QList<int>)), this, SLOT(receiveCompleteData(QList<int>)),  Qt::QueuedConnection);
    setOriData();

}

void Dialog::setOriData () {
    for (int i = 0; i < m_dataNumb; ++i) {
        m_oriData.append (i);
    }
}

QList<QList<int>> Dialog::allocateThreadData() {
    QList<QList<int>> result;
    for (int i = 0; i < m_threadNumb; ++i) {
        QList<int> emptyList;
        result.append (emptyList);
    }

    for (int i = 0; i < m_oriData.size(); i+=m_threadNumb) {
        for (int j = 0; j < m_threadNumb && i + j < m_oriData.size(); ++j) {
            result[j].append (m_oriData[i+j]);
        }
    }
    return result;
}

void Dialog::startProcessData (){
    QList<QList<int>> oriThreadData = allocateThreadData ();
    for (int i = 0; i < oriThreadData.size (); ++i) {
        DataContainer* curDataContainer = new DataContainer(oriThreadData[i]);
        QThread* curThread = new QThread();
        curDataContainer->moveToThread(curThread);

        connect (curThread, SIGNAL(finished()), curDataContainer, SLOT(deleteLater()));
        connect (this, SIGNAL(sendTestSignal(QList<int>)), curDataContainer, SLOT(receiveTestData(QList<int>)));

//        connect (curDataContainer, SIGNAL(sendProcessedDataToMaster(QList<int>)), this, SLOT(receiveProcessedData(QList<int>)), Qt::DirectConnection);
        connect (curDataContainer, SIGNAL(sendProcessedDataToMaster(QList<int>)), this, SLOT(receiveProcessedData(QList<int>)), Qt::QueuedConnection);

        curThread->start();
        m_dataContainerList.append(curDataContainer);
        m_ThreadList.append(curThread);
    }

    qDebug() << "Master::sendTestSignal: " << QThread::currentThreadId();
    emit sendTestSignal(m_oriData);
}

void Dialog::receiveProcessedData (QList<int> subThreadData) {
    qDebug() << "Master::processData: " << QThread::currentThreadId();
    QMutexLocker locker(&m_mutex);
    m_processedData += subThreadData;
    ++m_processedThreadCount;
    qDebug() << "m_processedThreadCount: " << m_processedThreadCount << "\n";
    if (m_processedThreadCount == m_threadNumb) {
        emit sendCompleteData (m_processedData);
    }
}

void Dialog::receiveCompleteData (QList<int> comData) {
    qDebug() << "Master::receiveCompleteData: " << QThread::currentThreadId();
    qDebug() << "All subThread process over";
    qDebug() << "Processed Data: " << comData;
    QMessageBox::information (this, "Info", "receiveCompleteData");

}

Dialog::~Dialog () {
    qDebug() << "~Master";
    for (int i = 0; i < m_dataContainerList.size(); ++i) {
         m_ThreadList[i]->quit();
         m_ThreadList[i]->wait();
    }
    qDebug() << "wait end!";
    delete ui;
}


void Dialog::on_startProcessData_clicked()
{
    startProcessData();
}
