#include "updaterealtimedataform.h"
#include "ui_updaterealtimedataform.h"
#include "toolfunc.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPersistentModelIndex>
#include <QAbstractItemModel>
#include "test.h"

UpdateRealtimeDataForm::UpdateRealtimeDataForm(QWidget *parent) :
    QWidget(parent), m_readRealTimeData(NULL), m_realtimeDatabase(NULL), m_strategyModel(NULL),
    m_strategyFileDir( "//192.168.211.182/1分钟数据 20160910-20170910/strategy"),
    ui(new Ui::UpdateRealtimeDataForm)
{
    ui->setupUi(this);
    initDatabase();
    initWidget();
    setTableView();
    setSecodeList();
    registeParams();
    connectSignal();
}

void UpdateRealtimeDataForm::initDatabase() {
//    m_realtimeDatabase = new RealTimeDatabase("0", "Test");
}

void UpdateRealtimeDataForm::initWidget() {
    ui->updateTimeSpinBox->setValue(3);
}

void UpdateRealtimeDataForm::registeParams() {
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>>");
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
    int updateTime = ui->updateTimeSpinBox->value() * 1000 - 800;
    if (updateTime < 0) updateTime = 2200;
    qDebug() << "updatetime: " << updateTime;
    m_readRealTimeData = new RealTimeDataRead(ui->regularMsgTableView, ui->errorMsgTableView,
                                              m_strategyFileDir, m_secodeList, &m_excel, updateTime);
    m_readRealTimeData->moveToThread(&m_windWorkThread);

    connect(&m_windWorkThread, SIGNAL(finished()), m_readRealTimeData, SLOT(deleteLater()));
    connect(this, SIGNAL(loginWind()), m_readRealTimeData, SLOT(loginWind()));
    connect(m_readRealTimeData, SIGNAL(sendStopTimerSignal()), this, SLOT(stopTimer()));

    connect(m_readRealTimeData, SIGNAL(loginWindFailed(int)), this, SLOT(loginWindFailed(int)));
    connect(m_readRealTimeData, SIGNAL(loginWindSucc()), this, SLOT(loginWindSucc()));

    m_windWorkThread.start();
    emit loginWind();
    updateProgramInfo(ui->regularMsgTableView, "正在登陆万得");
}

void UpdateRealtimeDataForm::setSecodeList() {
//    m_secodeList << "000001.SZ" << "600000.SH";
    m_strategyModel = new StrategyModel(m_strategyFileDir);
    QList<QString> new_strategyFileList = m_strategyModel->getStrategyFileList();
//    qDebug() << "new_strategyFileList: " << new_strategyFileList;
    bool isNewFileIn = false;
    for (int i = 0; i < new_strategyFileList.size(); ++i) {
        QString file = new_strategyFileList[i];
        if (m_strategyFileList.indexOf(file) < 0) {
            m_strategyFileList.append(file);
            QList<QString> appendSecodeList = m_excel.getSecodeFromeExcel(file);
            for (int j = 0; j < appendSecodeList.size(); ++j) {
                QString secode = completeSecode(appendSecodeList[j], "wind");
                if (m_secodeList.indexOf(secode) < 0) {
                    m_secodeList.append(secode);
                }
            }
            isNewFileIn = true;
        }
    }
    m_secodeList += getIndexCode("wind");
//    qDebug() << "m_secodeList: " << m_secodeList;
    updateProgramInfo(ui->regularMsgTableView,QString("完成读取策略股票代码"));
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

void UpdateRealtimeDataForm::stopTimer() {
    qDebug() << "UpdateRealtimeDataForm::stopTimer()";
    m_readRealTimeData->stopTimer();
}

void UpdateRealtimeDataForm::on_startGetRealtimeData_clicked()
{
//    testRealtimeDatabaseApi();
    m_readRealTimeData->startTimer();
    ui->startGetRealtimeData->setEnabled(false);
    ui->stopGetRealtimeData->setEnabled(true);
    updateProgramInfo(ui->regularMsgTableView, QString("获取实时数据"));
}

void UpdateRealtimeDataForm::on_stopGetRealtimeData_clicked()
{
    m_readRealTimeData->stopTimer();
    ui->startGetRealtimeData->setEnabled(true);
    ui->stopGetRealtimeData->setEnabled(false);
    updateProgramInfo(ui->regularMsgTableView, QString("停止获取实时数据"));
}

UpdateRealtimeDataForm::~UpdateRealtimeDataForm()
{
    m_readRealTimeData->stopTimer();
    m_windWorkThread.quit();
    m_windWorkThread.wait();
    delete ui;

    if (NULL != m_strategyModel) {
        delete m_strategyModel;
        m_strategyModel = NULL;
    }

    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
}
