#include <QDebug>
#include <QMetaType>
#include <QMutexLocker>
#include <QMessageBox>
#include "dataread.h"
#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::Dialog),
    m_processedThreadCount(0),  m_threadNumb(8)
{
    ui->setupUi(this);
}

Dialog::Dialog(int threadNumb, QWidget *parent):
    QDialog(parent), ui(new Ui::Dialog),
    m_processedThreadCount(0), m_threadNumb(threadNumb)
{
    ui->setupUi(this);
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
    setOriData();
}

void Dialog::setOriData () {
    m_oriData << "SH600000" << "SH600004" << "SH600006" << "SH600007" << "SH600008" << "SH600009"
              << "SH600010" << "SH600011" << "SH600012" << "SH600015" << "SH600016" << "SH600017";
    for (int i = 0; i < m_oriData.size (); ++i) {
        m_secodeBuyCount.insert (m_oriData[i], 1);
    }
}

QList<QStringList> Dialog::allocateThreadData() {
    QList<QStringList> result;
    for (int i = 0; i < m_threadNumb; ++i) {
        QStringList emptyList;
        result.append (emptyList);
    }

    for (int i = 0; i < m_oriData.size(); i+=m_threadNumb) {
        for (int j = 0; j < m_threadNumb && i + j < m_oriData.size(); ++j) {
            result[j].append (m_oriData[i+j]);
        }
    }
    return result;
}

void Dialog::startReadData (){
    QList<QStringList> oriThreadData = allocateThreadData ();
    QString dbHost = "localhost";
    QString dbName = "MarketData_day";
    QString startDate = "20141210";
    QString endDate = "20171210";
    QStringList keyValueList;
    keyValueList << "TCLOSE" << "VOTRUNOVER";

    qDebug() << "oriThreadData: " << oriThreadData;

    for (int i = 0; i < oriThreadData.size (); ++i) {
        DataRead* curDataReader = new DataRead(QString("%1").arg(i), dbHost, dbName, oriThreadData[i], startDate, endDate, keyValueList);
        QThread* curThread = new QThread();
        curDataReader->moveToThread(curThread);

        connect (curThread, SIGNAL(finished()), curDataReader, SLOT(deleteLater()));
        connect (this, SIGNAL(sendStartReadSignal(QString)), curDataReader, SLOT(receiveStartReadData(QString)));

//        connect (curDataReader, SIGNAL(sendProcessedDataToMaster(QList<int>)), this, SLOT(receiveProcessedData(QList<int>)), Qt::DirectConnection);
        connect (curDataReader, SIGNAL(sendHistoryData(QMap<QString, QList<QStringList>>)), this, SLOT(receiveProcessedData(QMap<QString, QList<QStringList>>)), Qt::QueuedConnection);

        curThread->start();
        m_dataReaderList.append(curDataReader);
        m_dataReaderThreadList.append(curThread);
    }

    qDebug() << "Master::sendStartReadSignal: " << QThread::currentThreadId();
    emit sendStartReadSignal("HistoryData");
}

void Dialog::receiveProcessedData (QMap<QString, QList<QStringList>> subThreadData) {
    qDebug() << "Master::processData: " << QThread::currentThreadId();
    QMutexLocker locker(&m_mutex);
    m_completeTableData.unite (subThreadData);
    ++m_processedThreadCount;
    qDebug() << "m_processedThreadCount: " << m_processedThreadCount << "\n";

    if (m_processedThreadCount == m_threadNumb) {
        releaseDataReaderSrc ();
        m_processedThreadCount = 0;
        qDebug() << "m_completeTableData.size: " <<  m_completeTableData.size ();
        qDebug() << "SH600000 dataNumb: " << m_completeTableData["SH600000"].size ();
        startProcessData();
        m_completeTableData.clear ();
    }
}

void Dialog::startProcessData () {
    DataProcess* curDataProcess = new DataProcess(m_completeTableData, m_secodeBuyCount);
    QThread* curDataProcessThread = new QThread();
    curDataProcess->moveToThread (curDataProcessThread);

    connect (curDataProcessThread, SIGNAL(finished()), curDataProcess, SLOT(deleteLater()));
    connect (this, SIGNAL(sendStartProcessData(QString)), curDataProcess, SLOT(receiveStartProcessData(QString)));
    connect(curDataProcess, SIGNAL(sendAllData(QList<QList<double>>)), this, SLOT(receiveAllProcessedData (QList<QList<double>>)));

    curDataProcessThread->start ();
    m_dataProcessList.append (curDataProcess);
    m_dataProcessThreadList.append (curDataProcessThread);
    emit sendStartProcessData("all");
}

void Dialog::receiveAllProcessedData (QList<QList<double>> allData) {
    qDebug() << "Dialog::receiveAllProcessedData: " << QThread::currentThreadId();
    QList<double> timeData = allData[0];
    QList<double> strategyData = allData[1];
    QList<double> votData = allData[2];
    QList<double> macdData = allData[3];

    releaseDataProcessSrc ();
}

void Dialog::releaseDataReaderSrc () {
    qDebug() << " m_dataReaderThreadList.size(): " <<  m_dataReaderThreadList.size();
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         m_dataReaderThreadList[i]->quit ();
    }
//    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
//         m_dataReaderThreadList[i]->quit ();
//         delete m_dataReaderThreadList[i];
//         m_dataReaderThreadList[i] = NULL;
//    }
//    m_dataReaderThreadList.clear ();
}

void Dialog::releaseDataProcessSrc () {
    qDebug() << " m_dataProcessThreadList.size(): " <<  m_dataProcessThreadList.size();
    for (int i = 0; i < m_dataProcessThreadList.size (); ++i) {
        m_dataProcessThreadList[i]->quit ();
    }
//    m_dataProcessThread->terminate ();
//    if (NULL != m_dataProcessThread) {
//        delete m_dataProcessThread;
//        m_dataProcessThread = NULL;
//    }
}

Dialog::~Dialog () {
    qDebug() << "~Dialog";
//    for (int i = 0; i < m_dataReaderList.size(); ++i) {
//         m_dataReaderThreadList[i]->quit();
//         m_dataReaderThreadList[i]->wait();
//    }
//    qDebug() << "m_dataReaderThreadList wait end!";
//    qDebug() << "m_dataProcessThread wait end!";

    for (int i = 0; i < m_dataProcessThreadList.size(); ++i) {
         delete m_dataProcessThreadList[i];
         m_dataProcessThreadList[i] = NULL;
    }
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         delete m_dataReaderThreadList[i];
         m_dataReaderThreadList[i] = NULL;
    }

    delete ui;
}


void Dialog::on_startProcessData_clicked()
{
    startReadData ();
}
