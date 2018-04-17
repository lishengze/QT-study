#include <QDebug>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QPersistentModelIndex>
#include <QAbstractItemModel>
#include "test.h"
#include "toolfunc.h"
#include "updaterealtimedataform.h"
#include "ui_updaterealtimedataform.h"

UpdateRealtimeDataForm::UpdateRealtimeDataForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateRealtimeDataForm)
{
    initCommonData();

    initWidget();
    registeParams();
}

void UpdateRealtimeDataForm::initCommonData() {
    m_realtimeDatabase = NULL;
    m_realTimeDataRead = NULL;
    m_realTimeDataWrite = NULL;
    m_realTimeDataWriteThread = NULL;
    m_realTimeDataReadThread = NULL;
}

void UpdateRealtimeDataForm::initDatabase() {
    m_dbName ="MarketData_RealTime";
    m_dbConnID = "1";
    m_realtimeDatabase = new RealTimeDatabase(m_dbConnID, m_dbName, m_dbHost);
    m_programInfoTableName = "ProgramInfo";
    m_realtimeDatabase->setDatabaseWorkingState(m_programInfoTableName, 1);
}

void UpdateRealtimeDataForm::initReadWriteObj() {
    m_realTimeDataRead = new RealTimeDataRead(ui->regularMsgTableView, ui->errorMsgTableView,
                                              "2", m_dbHost);
    m_realTimeDataReadThread= new QThread();
    m_realTimeDataRead->moveToThread(m_realTimeDataReadThread);

    connect(m_realTimeDataReadThread, SIGNAL(finished()),
            m_realTimeDataRead, SLOT(deleteLater()));

    connect(m_realTimeDataRead, SIGNAL(startMonitorTimer_signal()),
            this, SLOT(startReadMonitorTimer_slot()));

    connect(m_realTimeDataRead, SIGNAL(stopMonitorTimer_signal()),
            this, SLOT(stopReadMonitorTimer_slot()));

    connect(this, SIGNAL(loginWind_signal()),
            m_realTimeDataRead, SLOT(loginWind_slot()));

    connect(m_realTimeDataRead, SIGNAL(loginWindFailed_signal()),
            this, SLOT(loginWindFailed_slot()));

    connect(m_realTimeDataRead, SIGNAL(startWaitTradeTimer_signal()),
            this, SLOT(startReadWaitTradeTimer_slot()));

    connect(m_realTimeDataRead, SIGNAL(stopWaitTradeTimer_signal()),
            this, SLOT(stopReadWaitTradeTimer_slot()));

    m_realTimeDataWrite = new RealTimeDataWrite(ui->regularMsgTableView, ui->errorMsgTableView,
                                                "3", m_dbHost);

    m_realTimeDataWriteThread = new QThread();
    m_realTimeDataWrite->moveToThread(m_realTimeDataWriteThread);

    connect(m_realTimeDataWriteThread, SIGNAL(finished()),
            m_realTimeDataWrite, SLOT(deleteLater()));

    connect(m_realTimeDataWrite, SIGNAL(startMonitorTimer_signal()),
            this, SLOT(startWriteMonitorTimer_slot()));

    connect(m_realTimeDataWrite, SIGNAL(stopMonitorTimer_signal()),
            this, SLOT(stopWriteMonitorTimer_slot()));

    connect(m_realTimeDataRead, SIGNAL(setTableList_signal(QList<QString>, QList<QString>)),
            m_realTimeDataWrite, SLOT(setTableList_slot(QList<QString>,QList<QString>)));

    connect(m_realTimeDataWrite, SIGNAL(setTableListComplete_signal()),
            m_realTimeDataRead, SLOT(setTableListComplete_slot()));

    connect(m_realTimeDataRead, SIGNAL(writePreCloseData_signal(QMap<QString,QStringList>)),
            m_realTimeDataWrite, SLOT(writePreCloseData_slot(QMap<QString, QStringList>)));

    connect(m_realTimeDataWrite, SIGNAL(setPrecloseDataComplete_signal()),
            m_realTimeDataRead, SLOT(setPrecloseDataComplete_slot()));

    connect(m_realTimeDataRead, SIGNAL(writeRealTimeData_signal(QMap<QString,QStringList>, QMap<QString,QStringList>)),
            m_realTimeDataWrite, SLOT(writeRealTimeData_slot(QMap<QString,QStringList>, QMap<QString,QStringList>)));

    connect(m_realTimeDataWrite, SIGNAL(writeComplete_signal(int)),
            m_realTimeDataRead, SLOT(writeComplete_slot(int)));

    m_realTimeDataWriteThread->start();
    m_realTimeDataReadThread->start();
}

void UpdateRealtimeDataForm::registeParams() {
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>>");
    qRegisterMetaType<QMap<QString, QStringList>>("QMap<QString, QStringList>");
    qRegisterMetaType<QMap<QString, QStringList>>("QMap<QString, QStringList>");
}

void UpdateRealtimeDataForm::initWidget() {
    ui->setupUi(this);
    initTableView();
    initDatasourceCombox();
}

void UpdateRealtimeDataForm::initDatasourceCombox() {
    m_dbhostList << "192.168.211.162"
                 << "192.168.211.165";

    ui->dataSource_ComboBox->addItems(m_dbhostList);
    ui->dataSource_ComboBox->setCurrentIndex(0);
}

void UpdateRealtimeDataForm::initTableView () {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QString::fromLocal8Bit("备注")));
    ui->regularMsgTableView->setModel(initModel);
    ui->regularMsgTableView->setColumnWidth (0, 150);
    ui->regularMsgTableView->setColumnWidth (1, 450);
    ui->regularMsgTableView->setColumnWidth (2, 150);
    ui->regularMsgTableView->setShowGrid (false);

    QStandardItemModel* errMsgModel = new QStandardItemModel();
    errMsgModel -> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("时间")));
    errMsgModel -> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("消息")));
    errMsgModel -> setHorizontalHeaderItem (2, new QStandardItem(QString::fromLocal8Bit("备注")));
    ui->errorMsgTableView->setModel (errMsgModel);
    ui->errorMsgTableView->setColumnWidth (0, 150);
    ui->errorMsgTableView->setColumnWidth (1, 400);
    ui->errorMsgTableView->setColumnWidth (2, 150);
    ui->errorMsgTableView->setShowGrid (false);
}

void UpdateRealtimeDataForm::startWriteMonitorTimer_slot() {
    m_realTimeDataWrite->startMonitorTimer();
}

void UpdateRealtimeDataForm::stopWriteMonitorTimer_slot() {
    m_realTimeDataWrite->stopMonitorTimer();
}

void UpdateRealtimeDataForm::startReadMonitorTimer_slot() {
    m_realTimeDataRead->startMonitorTimer();
}

void UpdateRealtimeDataForm::stopReadMonitorTimer_slot() {
    m_realTimeDataRead->stopMonitorTimer();
}

void UpdateRealtimeDataForm::on_startDownloadData_Button_clicked()
{
    m_dbHost = ui->dataSource_ComboBox->currentText();
    updateProgramInfo(ui->regularMsgTableView, QString::fromLocal8Bit("当前选定更新的数据库为: %1").arg(m_dbHost));
    initDatabase();
    initReadWriteObj();
    emit loginWind_signal();
    updateProgramInfo(ui->regularMsgTableView, QString::fromLocal8Bit("正在登陆万得"));
}

void UpdateRealtimeDataForm::loginWindFailed_slot() {
    QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "登陆万得失败, 再次尝试登陆?",
                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes) {
        emit loginWind_signal();
    }
}

void UpdateRealtimeDataForm::stopReadWaitTradeTimer_slot() {
    qDebug() << "UpdateRealtimeDataForm::stopTimer()";
    m_realTimeDataRead->stopTimer();
}

void UpdateRealtimeDataForm::startReadWaitTradeTimer_slot() {
    qDebug() << "UpdateRealtimeDataForm::startTimer()";
    m_realTimeDataRead->startTimer();
}

UpdateRealtimeDataForm::~UpdateRealtimeDataForm()
{
    delete ui;

    if (NULL != m_realtimeDatabase) {
        m_realtimeDatabase->setDatabaseWorkingState(m_programInfoTableName, 0);
    }

    if (NULL != m_realTimeDataReadThread && !m_realTimeDataReadThread->isFinished()) {
        m_realTimeDataReadThread->quit();
    }

    if (NULL != m_realTimeDataWriteThread && !m_realTimeDataWriteThread->isFinished()) {
        m_realTimeDataWriteThread->quit();
    }

    if (NULL != m_realTimeDataRead) {
        m_realTimeDataRead->stopTimer();
        m_realTimeDataRead = NULL;
        delete m_realTimeDataRead;
    }

    if (NULL != m_realTimeDataWrite) {
        m_realTimeDataWrite = NULL;
        delete m_realTimeDataWrite;
    }

    if (NULL != m_realTimeDataReadThread) {
        m_realTimeDataReadThread = NULL;
        delete m_realTimeDataReadThread;
    }

    if (NULL != m_realTimeDataWriteThread) {
        m_realTimeDataWriteThread = NULL;
        delete m_realTimeDataWriteThread;
    }

    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
}

//void UpdateRealtimeDataForm::initDatabase() {
////    m_realtimeDatabase = new RealTimeDatabase("0", "Test");
//}

//void UpdateRealtimeDataForm::on_startGetRealtimeData_clicked()
//{
//    qDebug() << "on_startGetRealtimeData_clicked thread: " << QThread::currentThreadId();
//    m_realTimeDataRead->startTimer();
//    ui->startGetRealtimeData->setEnabled(false);
//    ui->stopGetRealtimeData->setEnabled(true);
//    updateProgramInfo(ui->regularMsgTableView, QString("获取实时数据"));
//}

//void UpdateRealtimeDataForm::on_stopGetRealtimeData_clicked()
//{
//    m_realTimeDataRead->stopTimer();
//    ui->startGetRealtimeData->setEnabled(true);
//    ui->stopGetRealtimeData->setEnabled(false);
//    updateProgramInfo(ui->regularMsgTableView, QString("停止获取实时数据"));
//}

//bool UpdateRealtimeDataForm::setSecodeList() {
////    qDebug() << "UpdateRealtimeDataForm::setSecodeList, m_initSecodeList: " << m_initSecodeList;
//    bool isNewSecodeIn = false;
//    m_strategyModel = new StrategyModel(m_hedgeIndexStrategyFileDir);
//    QList<QString> new_hedgeIndexStrategyFileList = m_strategyModel->getStrategyFileList();
//    for (int i = 0; i < new_hedgeIndexStrategyFileList.size(); ++i) {
//        QString file = new_hedgeIndexStrategyFileList[i];
//        if (m_strategyFileList.indexOf(file) < 0) {
//            if(!m_initSecodeList) {
//                updateProgramInfo(ui->regularMsgTableView,QString("增加新的指数对冲策略"));
//            }
//            m_strategyFileList.append(file);
//            QList<QString> appendSecodeList = m_excel.getSecodeFromeExcel(file);
//            for (int j = 0; j < appendSecodeList.size(); ++j) {
//                QString secode = completeSecode(appendSecodeList[j], "wind");
//                if (m_secodeList.indexOf(secode) < 0) {
//                    m_secodeList.append(secode);
//                    isNewSecodeIn = true;
//                }
//            }
//        }
//    }

//    StrategyModel* buSaleStrategyModel = new StrategyModel(m_buySaleStrategyFileDir);
//    QList<QString> new_buySaleStrategyFileList = buSaleStrategyModel->getStrategyFileList() ;
//    for (int i = 0; i < new_buySaleStrategyFileList.size(); ++i) {
//        QString file = new_buySaleStrategyFileList[i];
//        if (m_strategyFileList.indexOf(file) < 0) {
//            if(!m_initSecodeList) {
//                updateProgramInfo(ui->regularMsgTableView,QString("增加新的买卖对冲策略"));
//            }
//            m_strategyFileList.append(file);
//            QList<QString> appendSecodeList = m_excel.getSecodeFromeExcel(file, 1) + m_excel.getSecodeFromeExcel(file, 2);
//            for (int j = 0; j < appendSecodeList.size(); ++j) {
//                QString secode = completeSecode(appendSecodeList[j], "wind");
//                if (m_secodeList.indexOf(secode) < 0) {
//                    m_secodeList.append(secode);
//                    isNewSecodeIn = true;
//                }
//            }
//        }
//    }

////    qDebug() << "UpdateRealtimeDataForm::setSecodeList, isNewSecodeIn: " << isNewSecodeIn;

//    if (m_initSecodeList) {
//        m_secodeList += getIndexCode("wind");
//        updateProgramInfo(ui->regularMsgTableView, QString("完成读取策略股票代码"));
//        m_initSecodeList = false;
//    }
//    return isNewSecodeIn;
//}

//void UpdateRealtimeDataForm::startCheckSecodeTimer() {
//    m_timer.start(m_updateTime);
//}

//void UpdateRealtimeDataForm::stopCheckSecodeTimer() {
//    if (m_timer.isActive()) {
//        qDebug() << "UpdateRealtimeDataForm::stopTimer()";
//        m_timer.stop();
//    }
//}

//void UpdateRealtimeDataForm::checkSecodeList() {
////    qDebug() << "checkSecodeList";
//    if (setSecodeList()) {
//        m_realTimeDataRead->setSecodeList(m_secodeList);
//    }
//}


