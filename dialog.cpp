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
    timer->start(m_updateTime);

    initData();
}

Dialog::~Dialog()
{
    m_thread.quit();
    m_thread.wait();
    delete ui;
}

void Dialog::initData() {
    m_secodeNameList<< "600000.SH" << "000001.SZ" << "000002.SZ";
    QList<QStringList> empty;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        m_wsqData.insert(m_secodeNameList[i], empty);
    }
}

void Dialog::checkData() {
    qDebug() << "checkData";
    QMutexLocker locker(&g_wsqMutex);
    bool dataChange = false;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (g_wsqData[secode].size() > m_wsqData[secode].size()) {
            dataChange = true;
            for (int j = m_wsqData[secode].size(); j < g_wsqData[secode].size(); ++j) {
                m_wsqData[secode].append( g_wsqData[secode][j]);
            }
        }
    }
    if(dataChange) {
        dataChange = false;
        qDebug() << "Append New Data";
        qDebug() << "m_wsqData: " << m_wsqData;
    }
}

void Dialog::on_testWset_clicked()
{
    emit startWset();
}

void Dialog::on_loginButton_clicked()
{
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
