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
#include "announcementform.h"

QT_CHARTS_USE_NAMESPACE

QMap<QString, QList<QStringList>> g_wsqData;
QMutex g_wsqMutex;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    m_loginWind(false),
    m_strategyTalbeView(NULL),
    m_strategyModel(NULL),
    m_excel(NULL),
    m_strategyFileDir(""),
    m_bTestRealTime(false),
    m_databaseReadThreadNumb(48),
    m_announcementView(NULL),
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
    if (m_bTestRealTime) {
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

    ui->chooseStartDate->setDate (QDate(2014, 1, 1));
    ui->chooseEndDate->setDate (QDate(2017, 12, 20));

    QDate today = QDate::currentDate();
    QDate sevenDayAgo = today.addDays(-7);
    ui->AnnouncementStartDate->setDate(sevenDayAgo);
    ui->AnnouncementEndDate->setDate(today);
}

void Widget::setTestRealTimeData() {
    connect(&m_testRealTimer, SIGNAL(timeout()), this, SLOT(addTestRealTimeData()));
    m_testRealTimer.start(3000);
}

void Widget::addTestRealTimeData() {
//    qDebug() << "Widget::addTestRealTimeData";
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
    m_strategyFileDir = "//192.168.211.182/1分钟数据 20160910-20170910/strategy";
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
    ui->programInfo_tableView->setColumnWidth (0, 250);
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
    timeFre << "1m" << "5m" << "10m" << "30m" << "60m" << "120m" << "day" << "week" << "month";
    ui->dataFrequency->addItems (timeFre);
    ui->dataFrequency->setCurrentText ("day");
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
                                          macdTime, m_databaseReadThreadNumb);
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
    if (m_currStrategy.size () == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择策略");
        return;
    } else {
        int updateTime = 3000;
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
        QWidget* charView = new ChartForm(0, ui->programInfo_tableView, chartViewID,
                                          m_currStrategy, m_strategyName,
                                          hedgeIndexCode, hedgeIndexCount,
                                          updateTime, macdTime, m_bTestRealTime, m_databaseReadThreadNumb);
//        connect(charView,SIGNAL(sendCloseSignal(int)), this, SLOT(receiveChartCLoseSignal(int)));

        charView->show ();
        m_chartViews.append (charView);
        updateProgramInfo (ui->programInfo_tableView, QString("策略名称: %1, 策略中股票数目为: %2").arg(m_strategyName).arg(m_currStrategy.size ()));
        updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3").arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));
        updateProgramInfo (ui->programInfo_tableView, QString("对冲目标: %1, 对冲笔数: %2").arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));
        updateProgramInfo (ui->programInfo_tableView, QString("读取数据"));
    }
}

void Widget::on_Annoucnement_Button_clicked()
{
    QString startDate = ui->AnnouncementStartDate->date ().toString ("yyyyMMdd");
    QString endDate = ui->AnnouncementEndDate->date ().toString ("yyyyMMdd");
    m_announcementView = new AnnouncementForm(startDate, endDate);
    m_announcementView->show();
}

void Widget::receiveChartCLoseSignal(int chartViewID) {
    qDebug() << "Widget::receiveChartCLoseSignal chartViewID: " << chartViewID;
}

void Widget::closeEvent(QCloseEvent *event) {
    event;
    if (m_bTestRealTime) {
        m_testRealTimer.stop();
    }

    for(int i = 0; i < m_chartViews.size(); ++i) {
//        if (m_chartViews[i]->isActiveWindow()) {
//            m_chartViews[i] -> close();
//        }
        m_chartViews[i] -> close();
    }
    if (NULL != m_announcementView) {
        m_announcementView->close();
    }
}

void Widget::on_testSale_clicked()
{
//    QString dirName = "//192.168.211.182/1分钟数据 20160910-20170910/买入卖出组合";
//    qDebug() << "dirName: " << dirName;
//    QList<QString> fileNameList = getExcelFileName(dirName);
//    qDebug() << "fileNameList: " << fileNameList;

//    QString buyStrategyFileName = "\\192.168.211.182\1分钟数据 20160910-20170910\买入卖出组合\第1账户_1_买入组合_因子学习.xlsx";
//    QString saleStrategyFileName = "\\192.168.211.182\1分钟数据 20160910-20170910\买入卖出组合\第1账户_2_卖出组合_因子学习.xlsx";
    QString startDate = ui->chooseStartDate->date ().toString ("yyyyMMdd");
    QString endDate = ui->chooseEndDate->date ().toString ("yyyyMMdd");
    QString timeType = ui->dataFrequency->currentText();

    int EVA1Time = ui->EMA1TimeSpinBox->value ();
    int EVA2Time = ui->EMA2TimeSpinBox->value ();
    int DIFFTime = ui->DIFFTimeSpinBox->value ();
    QList<int> macdTime;
    macdTime << EVA1Time << EVA2Time << DIFFTime;

    QString buyStrategyFileName = "D:/strategy/buy.xlsx";
    QString saleStrategyFileName = "D:/strategy/sale.xlsx";
    QList<strategy_ceil> buyStrategy = m_excel->readStrategyDataFromExcel (buyStrategyFileName);
    QList<strategy_ceil> saleStrategy = m_excel->readStrategyDataFromExcel (saleStrategyFileName);
    QWidget* charView = new ChartForm(0, ui->programInfo_tableView, m_chartViews.count(),
                                      buyStrategy, saleStrategy,
                                      macdTime, false, 0,
                                      startDate, endDate, timeType);
//    qDebug() << "buyStrategy";
//    for (int i  = 0; i < buyStrategy.size(); ++i) {
//        qDebug() << buyStrategy[i].m_secode << ", " << buyStrategy[i].m_buyCount;
//    }

//    qDebug() << "saleStrategy";
//    for (int i  = 0; i < saleStrategy.size(); ++i) {
//        qDebug() << saleStrategy[i].m_secode << ", " << saleStrategy[i].m_buyCount;
//    }
}

Widget::~Widget()
{
//    qDebug() << "~Widget: " << 0;
    delete ui;

    if (NULL != m_strategyModel) {
        delete m_strategyModel;
        m_strategyModel = NULL;
    }

    if (NULL != m_excel) {
        delete m_excel;
        m_excel = NULL;
    }

//    qDebug() << "~Widget: " << 1;
    for (int i = 0; i < m_chartViews.size (); ++i) {
        qDebug() << i;
        if (NULL != m_chartViews[i]) {
            delete m_chartViews[i];
            m_chartViews[i] = NULL;
        }
    }

    if (NULL != m_announcementView) {
        delete m_announcementView;
        m_announcementView = NULL;
    }
//    qDebug() << "~Widget: " << 2;
}




