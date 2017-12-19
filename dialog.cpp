#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include "test.h"
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

using namespace std;

#include "dialog.h"
#include "ui_dialog.h"
#include "WAPIWrapperCpp.h"

QMap<QString, QList<QStringList>> g_wsqData;
QMutex g_wsqMutex;


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_updateTime(3000),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    m_testObj = new Test();
    m_testObj->moveToThread(&m_thread);
    connect(this, SIGNAL(startLogin()), m_testObj, SLOT(login()));
    connect(this, SIGNAL(startWsq()), m_testObj, SLOT(testWsq()));
    connect(this, SIGNAL(startWset()), m_testObj, SLOT(testWset()));
    connect(this, SIGNAL(startWsd()), m_testObj, SLOT(testWsd()));
    m_thread.start();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkData()));
//    timer->start(m_updateTime);

    initData();
}


void Dialog::initData() {
    m_secodeNameList<< "600000.SH" << "000001.SZ" << "000002.SZ";
    QStringList empty;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        m_realTimeData.insert(m_secodeNameList[i], empty);
    }
}

void Dialog::checkData() {
    qDebug() << "checkData";
    QMutexLocker locker(&g_wsqMutex);
    bool dataChange = false;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        QList<QStringList> currData = g_wsqData[secode];
        QStringList latestData = currData[currData.size()-1];
       if (m_realTimeData[secode].size() == 0 || latestData[1].toDouble() > m_realTimeData[secode][1].toDouble()) {
            dataChange = true;
            m_realTimeData[secode] = latestData;
        }
    }
    if(dataChange) {
//        refreshChart();
        dataChange = false;
        qDebug() << "Append New Data";
        qDebug() << "m_realTimeData: " << m_realTimeData;
    }
}

void Dialog::on_testWset_clicked()
{
    emit startWset();
}

void Dialog::on_loginButton_clicked()
{
//    Test* ptestObj = new Test();
//    QThread* pthread = new QThread();
//    ptestObj->moveToThread(pthread);

//    connect(this, SIGNAL(startLogin()), ptestObj, SLOT(login()));
//    connect(this, SIGNAL(startWsq()), ptestObj, SLOT(testWsq()));
//    connect(this, SIGNAL(startWset()), ptestObj, SLOT(testWset()));
//    connect(this, SIGNAL(startWsd()), ptestObj, SLOT(testWsd()));

//    pthread->start();
//    m_testObjList.append(ptestObj);
//    m_threadList.append(pthread);

    emit startLogin();
}

void Dialog::on_testWsdButton_clicked()
{
    emit startWsd();
}

void Dialog::on_testWsqButton_clicked()
{
    emit startWsq();
}

Dialog::~Dialog()
{
    m_thread.quit();
    m_thread.wait();

//    for (int i = 0; i < m_threadList.size(); ++i) {
//        m_threadList[i]->quit();
//        m_threadList[i]->wait();
//    }
//    for (int i = 0; i < m_threadList.size(); ++i) {
//        if (NULL != m_threadList[i]) {
//            delete m_threadList[i];
//            m_threadList[i] = NULL;
//        }
//    }
    delete ui;
}

