#include "updaterealtimedataform.h"
#include "ui_updaterealtimedataform.h"
#include "toolfunc.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include "test.h"

UpdateRealtimeDataForm::UpdateRealtimeDataForm(QWidget *parent) :
    QWidget(parent), m_readRealTimeData(NULL),
    m_strategyFileDir( "//192.168.211.182/1分钟数据 20160910-20170910"),
    ui(new Ui::UpdateRealtimeDataForm)
{
    ui->setupUi(this);
    initSecodeList();
    setTableView();
    connectSignal();
}

void UpdateRealtimeDataForm::setTableView () {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QObject::tr("备注")));
    ui->regularMsgTableView->setModel(initModel);
    ui->regularMsgTableView->setColumnWidth (0, 150);
    ui->regularMsgTableView->setColumnWidth (1, 450);
    ui->regularMsgTableView->setColumnWidth (2, 150);
    ui->regularMsgTableView->setShowGrid (false);

    QStandardItemModel* errMsgModel = new QStandardItemModel();
    errMsgModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("时间")));
    errMsgModel -> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("消息")));
    errMsgModel -> setHorizontalHeaderItem (2, new QStandardItem(QObject::tr("备注")));
    ui->errorMsgTableView->setModel (errMsgModel);
    ui->errorMsgTableView->setColumnWidth (0, 150);
    ui->errorMsgTableView->setColumnWidth (1, 400);
    ui->errorMsgTableView->setColumnWidth (2, 150);
    ui->errorMsgTableView->setShowGrid (false);
}

void UpdateRealtimeDataForm::connectSignal() {
    m_readRealTimeData = new RealTimeDataRead(ui->regularMsgTableView, m_strategyFileDir, 3000);
    m_readRealTimeData->moveToThread(&m_windWorkThread);

    connect(&m_windWorkThread, SIGNAL(finished()), m_readRealTimeData, SLOT(deleteLater()));
    connect(this, SIGNAL(loginWind()), m_readRealTimeData, SLOT(loginWind()));

    connect(m_readRealTimeData, SIGNAL(loginWindFailed(int)), this, SLOT(loginWindFailed(int)));
    connect(m_readRealTimeData, SIGNAL(loginWindSucc()), this, SLOT(loginWindSucc()));

    m_windWorkThread.start();
    emit loginWind();
    updateProgramInfo(ui->regularMsgTableView, "正在登陆万得");
}

void UpdateRealtimeDataForm::loginWindFailed(int errcode) {

    updateProgramInfo(ui->errorMsgTableView, QString("登录万得失败, 错误代码: %1").arg(errcode));
    QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "登陆万得失败, 再次尝试登陆?",
                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes) {
        emit loginWind();
    }
}

void UpdateRealtimeDataForm::loginWindSucc() {
    m_loginWind = true;
    updateProgramInfo(ui->regularMsgTableView, "登陆万得成功");
    ui->startGetRealtimeData->setDisabled(false);
}

void UpdateRealtimeDataForm::on_startGetRealtimeData_clicked()
{
//    testRealtimeDatabaseApi();
    m_readRealTimeData->startTimer();
}

UpdateRealtimeDataForm::~UpdateRealtimeDataForm()
{
    m_readRealTimeData->stopTimer();

    m_windWorkThread.quit();
    m_windWorkThread.wait();
    delete ui;
}


