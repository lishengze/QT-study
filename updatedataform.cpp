#include "updatedataform.h"
#include "ui_updatedataform.h"
#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include "toolfunc.h"

UpdateDataForm::UpdateDataForm(QString strategyFileDir, QWidget *parent) :
    m_strategyFileDir(strategyFileDir),
    QWidget(parent),
    ui(new Ui::UpdateDataForm),
    m_readRealTimeData(NULL), m_strategyModel(NULL), m_excel(NULL)
{
    qDebug() << "UpdateDataForm";
    ui->setupUi(this);

//    setTableView();

//    connectSignal();
}

void UpdateDataForm::connectSignal() {
    m_readRealTimeData = new RealTimeDataRead(ui->regularMsgTableView);
    m_readRealTimeData->moveToThread(&m_windWorkThread);

    connect(&m_windWorkThread, SIGNAL(finished()), m_readRealTimeData, SLOT(deleteLater()));
    connect(this, SIGNAL(loginWind()), m_readRealTimeData, SLOT(loginWind()));

    connect(m_readRealTimeData, SIGNAL(loginWindFailed(int)), this, SLOT(loginWindFailed(int)));
    connect(m_readRealTimeData, SIGNAL(loginWindSucc()), this, SLOT(loginWindSucc()));

    m_windWorkThread.start();
    emit loginWind();
    updateProgramInfo(ui->regularMsgTableView, "正在登陆万得");
}

void UpdateDataForm::setTableView () {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("消息")));
    ui->regularMsgTableView->setModel (initModel);
    ui->regularMsgTableView->setColumnWidth (0, 150);
    ui->regularMsgTableView->setColumnWidth (1, 400);
    ui->regularMsgTableView->setShowGrid (false);

    ui->errorMsgTableView->setModel (initModel);
    ui->errorMsgTableView->setColumnWidth (0, 150);
    ui->errorMsgTableView->setColumnWidth (1, 400);
    ui->errorMsgTableView->setShowGrid (false);
}

void UpdateDataForm::loginWindFailed(int errcode) {

    updateProgramInfo(ui->errorMsgTableView, QString("登录万得失败, 错误代码: %1").arg(errcode));
    QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "登陆万得失败, 再次尝试登陆?",
                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes) {
        emit loginWind();
    }
}

void UpdateDataForm::loginWindSucc() {
    m_loginWind = true;
    updateProgramInfo(ui->regularMsgTableView, "登陆万得成功");
}

UpdateDataForm::~UpdateDataForm()
{
    m_windWorkThread.quit();
    m_windWorkThread.wait();

    delete ui;

    if (NULL != m_strategyModel) {
        delete m_strategyModel;
        m_strategyModel = NULL;
    }

    if (NULL != m_excel) {
        delete m_excel;
        m_excel = NULL;
    }
}
