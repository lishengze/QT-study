#include <QScatterSeries>
#include <QDialog>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QDate>
#include <QTime>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QPersistentModelIndex>
#include <QAbstractItemModel>
#include <QVector>
#include <QTimer>

#include "toolfunc.h"
#include "widget.h"
#include "ui_widget.h"
#include "QChartView"
#include "setdata.h"
#include "strategymodel.h"
#include "chartform.h"
#include "realtimedataread.h"

QT_CHARTS_USE_NAMESPACE

QMap<QString, QList<QStringList>> g_wsqData;
QMutex g_wsqMutex;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    m_loginWind(false),
    m_readRealTimeData(NULL),
    m_strategyTalbeView(NULL),
    m_strategyModel(NULL),
    m_strategyFileDir(""),
    m_bTestRealTime(true),
    ui(new Ui::Widget)
{
    m_excel = new Excel();
    ui->setupUi(this);
    setCalendarValue();
    setHedgeValue();
    setMacdTime();
    setStrategyTableView();
    setProgramInfoTableView();
    setDataFrequency();
    initReadRealTimeData();
}

void Widget::initReadRealTimeData() {
//    qDebug() << "Widget::initReadRealTimeData(): " << QThread::currentThread();
    if (!m_bTestRealTime) {
        m_updateRealDataTimeMS = 3000;
        m_readRealTimeData = new RealTimeDataRead(ui->programInfo_tableView);
        m_readRealTimeData->moveToThread(&m_windWorkThread);
        connect(&m_windWorkThread, SIGNAL(finished()), m_readRealTimeData, SLOT(deleteLater()));
        connect(this, SIGNAL(loginWind()), m_readRealTimeData, SLOT(loginWind()));
        connect(this, SIGNAL(startWsq(QStringList, int)), m_readRealTimeData, SLOT(startWsq(QStringList, int)));

        connect(this, SIGNAL(cancelWsqRequest(int)), m_readRealTimeData, SLOT(cancelWsqRequest(int)));
        connect(this, SIGNAL(cancelAllWsqRequest()), m_readRealTimeData, SLOT(cancelAllWsqRequest()));

        connect(m_readRealTimeData, SIGNAL(loginWindFailed(int)), this, SLOT(loginWindFailed(int)));
        connect(m_readRealTimeData, SIGNAL(loginWindSucc()), this, SLOT(loginWindSucc()));

        connect(m_readRealTimeData, SIGNAL(startWsqFailed(int, int)), this, SLOT(startWsqFailed(int, int)));
        connect(m_readRealTimeData, SIGNAL(startWsqSucc()), this, SLOT(startWsqSucc()));

        m_windWorkThread.start();
        emit loginWind();
        updateProgramInfo(ui->programInfo_tableView, "正在登陆万得");
    } else {
        m_updateRealDataTimeMS = 3000;
        setTestRealTimeData();
    }

    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QList<QString>>("QMap<QString, double>");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
}

void Widget::setCalendarValue () {
    ui->chooseStartDate->setCalendarPopup (true);
    ui->chooseEndDate->setCalendarPopup (true);

    ui->chooseStartDate->setDate (QDate(2017, 12, 8));
    ui->chooseEndDate->setDate (QDate(2017, 12, 20));
}

void Widget::setTestRealTimeData() {
    connect(&m_testRealTimer, SIGNAL(timeout()), this, SLOT(addTestRealTimeData()));
    m_testRealTimer.start(m_updateRealDataTimeMS);
}

void Widget::addTestRealTimeData() {
    QList<QString> curGlobalSecodeList = g_wsqData.keys();
    for (int i = 0; i < curGlobalSecodeList.size(); ++i) {
        QString secode = curGlobalSecodeList[i];

        QStringList newData;
        QString date = QDate::currentDate().toString("yyyyMMdd");
        QString time = QTime::currentTime().toString("hhmmss");
        QString last = QString("%1").arg(qrand()%10);
        QString preClost = QString("%1").arg(qrand()%10);
        QString amt = QString("%1").arg(qrand()%10000);
//        QString last = QString("%1").arg(1);
//        QString preClost = QString("%1").arg(1);
//        QString amt = QString("%1").arg(100);
        newData << date << time << last << preClost << amt;
//        QList<QStringList> curData = g_wsqData[secode];
//        QStringList latestData;
//        if (curData.size() > 0) {
//            QStringList latestData = curData[curData.size()-1];
//        }
        QStringList latestData;
        if (g_wsqData[secode].size() > 0) {
            latestData = g_wsqData[secode][g_wsqData[secode].size() -1];
        }
        g_wsqData[secode].clear();
        if (latestData.size() > 0) {
            g_wsqData[secode].append(latestData);
        }
        g_wsqData[secode].append(newData);
    }
}

void Widget::setHedgeValue() {
    ui->hedgeTarget_comboBox->setCurrentText ("沪深300");
    ui->hedgeTarget_comboBox->addItem ("沪深300",  "SH000300");
    ui->hedgeTarget_comboBox->addItem ("上证50",   "SH000016");
    ui->hedgeTarget_comboBox->addItem ("中证100",  "SZ399903");
    ui->hedgeTarget_comboBox->addItem ("中证200",  "SH000904");
    ui->hedgeTarget_comboBox->addItem ("中证500",  "SH000905");
    ui->hedgeTarget_comboBox->addItem ("中证800",  "SH000906");
    ui->hedgeTarget_comboBox->addItem ("中证1000", "SH000852");
    ui->hedgeCount_spinBox->setValue (2);
}

void Widget::setStrategyTableView () {
//    m_strategyFileDir = "D:/strategy";
    m_strategyFileDir = "//192.168.211.182/1分钟数据 20160910-20170910";
    m_strategyModel = new StrategyModel(m_strategyFileDir);
    QStandardItemModel* tableModel = m_strategyModel->getTableModel ();

    ui->tableView->setModel (tableModel);
    ui->tableView->setColumnWidth (0, 300);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setShowGrid (false);
}

void Widget::setProgramInfoTableView () {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QObject::tr("备注")));
    ui->programInfo_tableView->setModel (initModel);
    ui->programInfo_tableView->setColumnWidth (0, 150);
    ui->programInfo_tableView->setColumnWidth (1, 600);
    ui->programInfo_tableView->setColumnWidth (2, 150);
    ui->programInfo_tableView->setShowGrid (false);
}

void Widget::setMacdTime () {
    ui->EMA1TimeSpinBox->setValue (12);
    ui->EMA2TimeSpinBox->setValue (26);
    ui->DIFFTimeSpinBox->setValue (9);
}

void Widget::setDataFrequency () {
    QStringList timeFre;
    timeFre << "1m" << "5m" << "10m" << "20m" << "30m" << "60m" << "120m" << "day" << "week" << "month";
    ui->dataFrequency->addItems (timeFre);
    ui->dataFrequency->setCurrentText ("day");
}

void Widget::loginWindFailed(int errcode) {
    updateProgramInfo(ui->programInfo_tableView, QString("登录万得失败, 错误代码是: %1").arg(errcode));
    QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "登陆万得失败, 再次尝试登陆?",
                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes) {
        emit loginWind();
    }
}

void Widget::loginWindSucc() {
    m_loginWind = true;
    updateProgramInfo(ui->programInfo_tableView, "登陆万得成功");
}

void Widget::startWsqFailed(int errcode, int reqID) {
    updateProgramInfo(ui->programInfo_tableView, QString("订阅实时消息失败, 错误代码是: %1").arg(errcode));
    QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "订阅实时消息失败, 再次尝试订阅?",
                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (rb == QMessageBox::Yes) {
        QStringList secodeList;
        for (int i = 0; i < m_currStrategy.size (); ++i) {
            secodeList.append(m_currStrategy[i].m_secode);
        }
        emit startWsq(secodeList, reqID);
    }
}

void Widget::startWsqSucc() {
    updateProgramInfo(ui->programInfo_tableView, "订阅实时消息成功");
}

void Widget::on_chooseStartDate_editingFinished()
{
    QDate startDate = ui->chooseStartDate->date ();
    QString strDate = startDate.toString ("yyyyMMdd");
    qDebug() << "strDate: " << strDate;
}

void Widget::on_tableView_clicked(const QModelIndex &index)
{
    ui->historyData->setEnabled (false);
    int intIndex = index.row ();
    m_strategyName = m_strategyModel->getTableModel () ->item (intIndex)->text ();
    updateProgramInfo (ui->programInfo_tableView, QString("读取策略: %1").arg(m_strategyName));

    QString strategyFullFileName = m_strategyModel->getStrategyFullFileName (intIndex);
    m_currStrategy = m_excel->readStrategyDataFromExcel (strategyFullFileName);

    updateProgramInfo (ui->programInfo_tableView, QString("策略中股票数目为: %1").arg(m_currStrategy.size ()));
    qDebug() << "m_currStrategy.size: " << m_currStrategy.size ();

    ui->historyData->setEnabled(true);

    if (m_bTestRealTime) {
        QString hedgeIndexCode = ui->hedgeTarget_comboBox->currentData ().toString ();
        QList<QStringList> emptyList;
        QList<QString> curGlobalSecodeList = g_wsqData.keys();
        if(curGlobalSecodeList.indexOf(getWindSecode(hedgeIndexCode)) < 0) {
            g_wsqData.insert(getWindSecode(hedgeIndexCode), emptyList);
        }
        for (int i = 0; i < m_currStrategy.size (); ++i) {
            if (curGlobalSecodeList.indexOf(getWindSecode(m_currStrategy[i].m_secode)) < 0) {
                g_wsqData.insert(getWindSecode(m_currStrategy[i].m_secode), emptyList);
            }
        }
    }
}

void Widget::on_historyData_clicked()
{
    QString startDate = ui->chooseStartDate->date ().toString ("yyyyMMdd");
    QString endDate = ui->chooseEndDate->date ().toString ("yyyyMMdd");
    QString timeType = ui->dataFrequency->currentText();
    int EVA1Time = ui->EMA1TimeSpinBox->value ();
    int EVA2Time = ui->EMA2TimeSpinBox->value ();
    int DIFFTime = ui->DIFFTimeSpinBox->value ();
    QList<int> macdTime;
    macdTime << EVA1Time << EVA2Time << DIFFTime;

    QString hedgeIndexCode = ui->hedgeTarget_comboBox->currentData ().toString ();
    int hedgeIndexCount = ui->hedgeCount_spinBox->value ();

    if (startDate.toInt () >= endDate.toInt ()) {
        QMessageBox::critical(NULL, "Error", "起始时间晚于截止时间");
        return;
    } else if (m_currStrategy.size () == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择策略");
        return;
    } else {
        QWidget* charView = new ChartForm(0, ui->programInfo_tableView, m_chartViews.count(),
                                          startDate, endDate, timeType,
                                          m_currStrategy, m_strategyName,
                                          hedgeIndexCode, hedgeIndexCount,
                                          macdTime);
        charView->show ();

        m_chartViews.append (charView);
//        qDebug() << "hedgeIndexCode: " << hedgeIndexCode << " hedgeIndexCount: " << hedgeIndexCount;
//        qDebug() << "timeType: " << timeType;
//        qDebug() << "EVA1Time: " << EVA1Time << ", EVA2Time: " << EVA2Time << ", DIFFTime: " << DIFFTime;
        updateProgramInfo (ui->programInfo_tableView, QString("策略名称: %1, 策略中股票数目为: %2").arg(m_strategyName).arg(m_currStrategy.size ()));
        updateProgramInfo (ui->programInfo_tableView, QString("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6")
                           .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));
        updateProgramInfo (ui->programInfo_tableView, QString("对冲目标: %1, 对冲笔数: %2").arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));
        updateProgramInfo (ui->programInfo_tableView, QString("读取数据"));
    }
}

void Widget::on_realDateTime_pushButton_clicked()
{
    if (m_bTestRealTime) {
        m_loginWind = true;
    }
    if (true == m_loginWind) {
        if (m_currStrategy.size () == 0) {
            QMessageBox::critical(NULL, "Error", "还未选择策略");
            return;
        } else {
            int EVA1Time = ui->EMA1TimeSpinBox->value ();
            int EVA2Time = ui->EMA2TimeSpinBox->value ();
            int DIFFTime = ui->DIFFTimeSpinBox->value ();
            QList<int> macdTime;
            macdTime << EVA1Time << EVA2Time << DIFFTime;

            QString hedgeIndexCode = ui->hedgeTarget_comboBox->currentData ().toString ();
            int hedgeIndexCount = ui->hedgeCount_spinBox->value ();

            QStringList secodeList;
            for (int i = 0; i < m_currStrategy.size (); ++i) {
                secodeList.append(m_currStrategy[i].m_secode);
            }
            secodeList.append(hedgeIndexCode);

            int chartViewID = m_chartViews.count();
            QWidget* charView = new ChartForm(0, m_readRealTimeData,
                                              ui->programInfo_tableView, chartViewID,
                                              m_currStrategy, m_strategyName,
                                              hedgeIndexCode, hedgeIndexCount,
                                              m_updateRealDataTimeMS, macdTime, m_bTestRealTime);
            connect(charView,SIGNAL(sendCloseSignal(int)), this, SLOT(receiveChartCLoseSignal(int)));

            if (!m_bTestRealTime) {
                emit startWsq(secodeList, chartViewID);
            }
            charView->show ();
            m_chartViews.append (charView);
//            qDebug() << "hedgeIndexCode: " << hedgeIndexCode << " hedgeIndexCount: " << hedgeIndexCount;
//            qDebug() << "EVA1Time: " << EVA1Time << ", EVA2Time: " << EVA2Time << ", DIFFTime: " << DIFFTime;
            updateProgramInfo (ui->programInfo_tableView, QString("策略名称: %1, 策略中股票数目为: %2").arg(m_strategyName).arg(m_currStrategy.size ()));
            updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3").arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));
            updateProgramInfo (ui->programInfo_tableView, QString("对冲目标: %1, 对冲笔数: %2").arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));
            updateProgramInfo (ui->programInfo_tableView, QString("读取数据"));
        }
    } else {
        QMessageBox::StandardButton rb = QMessageBox::critical(this, "Error", "还未登陆万得, 再次尝试登陆?",
                                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (rb == QMessageBox::Yes) {
            emit loginWind();
        }
    }
}

void Widget::receiveChartCLoseSignal(int chartViewID) {
    qDebug() << "Widget::receiveChartCLoseSignal chartViewID: " << chartViewID;
    if (!m_bTestRealTime) {
        emit cancelWsqRequest(chartViewID);
    }
}

void Widget::closeEvent(QCloseEvent *event) {
    event;
    if (!m_bTestRealTime) {
        emit cancelAllWsqRequest();
    } else {
        m_testRealTimer.stop();
    }
    for(int i = 0; i < m_chartViews.size(); ++i) {
        m_chartViews[i] -> close();
    }
}

Widget::~Widget()
{
    if (!m_bTestRealTime) {
        m_windWorkThread.quit();
        m_windWorkThread.wait();
    }

    delete ui;
    if (NULL != m_strategyModel) {
        delete m_strategyModel;
        m_strategyModel = NULL;
    }

    if (NULL != m_excel) {
        delete m_excel;
        m_excel = NULL;
    }

    for (int i = 0; i < m_chartViews.size (); ++i) {
        if (NULL != m_chartViews[i]) {
            delete m_chartViews[i];
            m_chartViews[i] = NULL;
        }
    }
}
