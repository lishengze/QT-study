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
    QWidget(parent),
    ui(new Ui::UpdateRealtimeDataForm)
{
    initCommonData();
    initDatabase();
    if (isDatabaseWorking()) {
        m_workingState = false;
        QMessageBox::warning(this, "Error", "已有客户端在下载");
        this->close();
    } else {
        m_workingState = true;
        ui->setupUi(this);
        initWidget();
        initTableView();

        m_realtimeDatabase->setDatabaseWorkingState(m_programInfoTableName, 1);
        registeParams();
        connectSignal();
    }
}

void UpdateRealtimeDataForm::initCommonData() {
    m_realtimeDatabase = NULL;
    m_realTimeDataRead = NULL;
    m_realTimeDataWrite = NULL;
    m_realTimeDataWriteThread = NULL;
    m_realTimeDataReadThread = NULL;
}

void UpdateRealtimeDataForm::initWidget() {

}

void UpdateRealtimeDataForm::initDatabase() {
    m_dbName ="MarketData_RealTime";
    m_dbHost = "192.168.211.165";
    m_dbConnID = "0";
    m_realtimeDatabase = new RealTimeDatabase(m_dbConnID, m_dbName, m_dbHost);
    m_programInfoTableName = "ProgramInfo";
}

bool UpdateRealtimeDataForm::isDatabaseWorking() {
    if (1 == m_realtimeDatabase->getDatabaseWorkingFlag(m_programInfoTableName)) {
        return true;
    } else {
        return false;
    }
}

void UpdateRealtimeDataForm::registeParams() {
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>>");
    qRegisterMetaType<QMap<QString, QStringList>>("QMap<QString, QStringList>");
}

void UpdateRealtimeDataForm::initTableView () {
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
    m_realTimeDataRead = new RealTimeDataRead(ui->regularMsgTableView, ui->errorMsgTableView,
                                              "1", m_dbHost);
    m_realTimeDataReadThread= new QThread();
    m_realTimeDataRead->moveToThread(m_realTimeDataReadThread);

    connect(m_realTimeDataReadThread, SIGNAL(finished()),
            m_realTimeDataRead, SLOT(deleteLater()));

    connect(this, SIGNAL(loginWind_signal()),
            m_realTimeDataRead, SLOT(loginWind_slot()));

    connect(m_realTimeDataRead, SIGNAL(loginWindFailed_signal()),
            this, SLOT(loginWindFailed_slot()));

    connect(m_realTimeDataRead, SIGNAL(startReadRealTimeData_signal()),
            this, SLOT(startReadRealTimeData_slot()));

    connect(m_realTimeDataRead, SIGNAL(stopReadRealTimeData_signal()),
            this, SLOT(stopReadRealTimeData_slot()));

    m_realTimeDataWrite = new RealTimeDataWrite(ui->regularMsgTableView, ui->errorMsgTableView,
                                                "2", m_dbHost);

    m_realTimeDataWriteThread = new QThread();
    m_realTimeDataWrite->moveToThread(m_realTimeDataWriteThread);

    connect(m_realTimeDataWriteThread, SIGNAL(finished()),
            m_realTimeDataWrite, SLOT(deleteLater()));

    connect(m_realTimeDataRead, SIGNAL(setSecodeList_signal(QList<QString>)),
            m_realTimeDataWrite, SLOT(setSecodeList_slot(QList<QString>)));

    connect(m_realTimeDataRead, SIGNAL(writePreCloseData_signal(QMap<QString,QStringList>)),
            m_realTimeDataWrite, SLOT(writePreCloseData_slot(QMap<QString, QStringList>)));

    connect(m_realTimeDataRead, SIGNAL(writeRealTimeData_signal(QMap<QString,QStringList>)),
            m_realTimeDataWrite, SLOT(writeRealTimeData_slot(QMap<QString, QStringList>)));

    connect(m_realTimeDataWrite, SIGNAL(setRealTimeData_signal()),
            m_realTimeDataRead, SLOT(setRealTimeData_slot()));

    connect(m_realTimeDataWrite, SIGNAL(writeComplete_signal()),
            m_realTimeDataRead, SLOT(writeComplete_slot()));

    m_realTimeDataWriteThread->start();
    m_realTimeDataReadThread->start();

    emit loginWind_signal();

    updateProgramInfo(ui->regularMsgTableView, "正在登陆万得");
}

void UpdateRealtimeDataForm::loginWindFailed_slot() {
    QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "登陆万得失败, 再次尝试登陆?",
                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes) {
        emit loginWind_signal();
    }
}

void UpdateRealtimeDataForm::stopReadRealTimeData_slot() {
    qDebug() << "UpdateRealtimeDataForm::stopTimer()";
    m_realTimeDataRead->stopTimer();
}

void UpdateRealtimeDataForm::startReadRealTimeData_slot() {
    qDebug() << "UpdateRealtimeDataForm::startTimer()";
    m_realTimeDataRead->startTimer();
}

UpdateRealtimeDataForm::~UpdateRealtimeDataForm()
{
    qDebug() << 0;
    if (m_workingState) {
        m_realTimeDataRead->stopTimer();
    }

    qDebug() << 1;
    if (NULL != m_realTimeDataReadThread && !m_realTimeDataReadThread->isFinished()) {
        m_realTimeDataReadThread->quit();
    }

    qDebug() << 2;
    if (NULL != m_realTimeDataWriteThread && !m_realTimeDataWriteThread->isFinished()) {
        m_realTimeDataWriteThread->quit();
    }

    qDebug() << 3;
    if (NULL != m_realTimeDataRead) {
        m_realTimeDataRead = NULL;
        delete m_realTimeDataRead;
    }

    qDebug() << 4;
    if (NULL != m_realTimeDataWrite) {
        m_realTimeDataWrite = NULL;
        delete m_realTimeDataWrite;
    }

    qDebug() << 5;
    if (NULL != m_realTimeDataReadThread) {
        m_realTimeDataReadThread = NULL;
        delete m_realTimeDataReadThread;
    }

    qDebug() << 6;
    if (NULL != m_realTimeDataWriteThread) {
        m_realTimeDataWriteThread = NULL;
        delete m_realTimeDataWriteThread;
    }

    qDebug() << 7;
    delete ui;
    if (true == m_workingState) {
        m_realtimeDatabase->setDatabaseWorkingState(m_programInfoTableName, 0);
    }

    qDebug() << 8;
    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
    qDebug() << 9;
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
