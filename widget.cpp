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

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    m_loginWind(false),
    m_strategyTalbeView(NULL),
    m_strategyModel(NULL),
    m_excel(NULL),
    m_strategyFileDir(""),
    m_announcementView(NULL),
    ui(new Ui::Widget)
{
    m_excel = new Excel();
    ui->setupUi(this);
    setCalendarValue();
    setHedgeValue();
    setMacdTime();
    setStrategyTableView();
    setBuySaleStrategyTableView();
    setProgramInfoTableView();
    setDataFrequency();
    initReadRealTimeData();
}

void Widget::initReadRealTimeData() {
    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QList<QString>>("QMap<QString, double>");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
}

void Widget::setCalendarValue () {
    ui->chooseStartDate->setCalendarPopup (true);
    ui->chooseEndDate->setCalendarPopup (true);

    ui->chooseStartDate->setDate (QDate::currentDate().addDays(-30));
    ui->chooseEndDate->setDate (QDate::currentDate());

    QDate today = QDate::currentDate();
    QDate sevenDayAgo = today.addDays(-7);
    ui->AnnouncementStartDate->setDate(sevenDayAgo);
    ui->AnnouncementEndDate->setDate(today);
}

void Widget::setHedgeValue() {
    ui->hedgeTarget_comboBox->setCurrentText ("沪深300");
    ui->hedgeTarget_comboBox->addItem ("沪深300",  "000300");
    ui->hedgeTarget_comboBox->addItem ("上证50",   "000016");
    ui->hedgeTarget_comboBox->addItem ("中证100",  "399903");
    ui->hedgeTarget_comboBox->addItem ("中证200",  "000904");
    ui->hedgeTarget_comboBox->addItem ("中证500",  "000905");
    ui->hedgeTarget_comboBox->addItem ("中证800",  "000906");
    ui->hedgeTarget_comboBox->addItem ("中证1000", "000852");
    ui->hedgeCount_spinBox->setValue (2);
}

void Widget::setBuySaleStrategyTableView () {
    m_buySaleStrategyFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/买入卖出组合/");
    m_buySaleStrategyModel = new StrategyModel(m_buySaleStrategyFileDir);
    QStandardItemModel* tableModel = m_buySaleStrategyModel->getTableModel ();

    ui->buySaleStrategyTable->setModel (tableModel);
    ui->buySaleStrategyTable->setColumnWidth (0, 300);
    ui->buySaleStrategyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->buySaleStrategyTable->setShowGrid (false);
}

void Widget::setStrategyTableView () {
    m_strategyFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/strategy");
    m_strategyModel = new StrategyModel(m_strategyFileDir);
    QStandardItemModel* tableModel = m_strategyModel->getTableModel ();

    ui->hedgeIndexStrategyTable->setModel (tableModel);
    ui->hedgeIndexStrategyTable->setColumnWidth (0, 300);
    ui->hedgeIndexStrategyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->hedgeIndexStrategyTable->setShowGrid (false);
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

void Widget::on_hedgeIndexStrategyTable_clicked(const QModelIndex &index)
{
    int intIndex = index.row ();
    m_strategyName = m_strategyModel->getTableModel () ->item (intIndex)->text ();
    updateProgramInfo (ui->programInfo_tableView, QString("读取策略: %1").arg(m_strategyName));

    QString strategyFullFileName = m_strategyModel->getStrategyFullFileName (intIndex);
    m_currStrategy = m_excel->readStrategyDataFromExcel (strategyFullFileName);
    m_strategyMap = m_excel->readStrategyData(strategyFullFileName);

    updateProgramInfo (ui->programInfo_tableView, QString("策略中股票数目为: %1").arg(m_currStrategy.size ()));
}

void Widget::on_buySaleStrategyTable_clicked(const QModelIndex &index)
{
    int intIndex = index.row ();
    m_buySaleStrategyFileName = m_buySaleStrategyModel->getTableModel () ->item (intIndex)->text ();
    updateProgramInfo (ui->programInfo_tableView, QString("读取买卖策略: %1").arg(m_buySaleStrategyFileName));

    QString strategyFullFileName = m_buySaleStrategyModel->getStrategyFullFileName (intIndex);
//    qDebug() << "strategyFullFileName: " << strategyFullFileName;
    qDebug() << "m_buySaleStrategyFileName: " << m_buySaleStrategyFileName;

    m_buyStrategyMap = m_excel->readStrategyData(strategyFullFileName, 1);
    m_saleStrategyMap = m_excel->readStrategyData(strategyFullFileName, 2);
//    qDebug() << "m_buyStrategyMap: " << m_buyStrategyMap;
//    qDebug() << "m_saleStrategyMap: " << m_saleStrategyMap;

    updateProgramInfo (ui->programInfo_tableView, QString("策略中股票数目为: %1").
                       arg(m_buyStrategyMap.size () + m_saleStrategyMap.size()));

}

void Widget::on_historyHedgeIndexChart_clicked()
{
    if (m_strategyMap.size () == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择策略");
        return;
    } else {
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
                                              hedgeIndexCode, hedgeIndexCount, macdTime,
                                              m_strategyMap, m_strategyName,
                                              EmpytQStringIntMap(), EmpytQStringIntMap(),
                                              false, startDate, endDate, timeType);
            charView->show ();
            m_chartViews.append (charView);
            updateProgramInfo (ui->programInfo_tableView, QString("策略名称: %1, 策略中股票数目为: %2").arg(m_strategyName).arg(m_currStrategy.size ()));
            updateProgramInfo (ui->programInfo_tableView, QString("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6")
                               .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));
            updateProgramInfo (ui->programInfo_tableView, QString("对冲目标: %1, 对冲笔数: %2").arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));
            updateProgramInfo (ui->programInfo_tableView, QString("读取数据"));
        }
    }
}

void Widget::on_realTimeHedgeIndexChart_clicked()
{
    if (m_strategyMap.size () == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择策略");
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

        QWidget* charView = new ChartForm(0, ui->programInfo_tableView, chartViewID,
                                          hedgeIndexCode, hedgeIndexCount, macdTime,
                                          m_strategyMap, m_strategyName);

        charView->show ();
        m_chartViews.append (charView);
        updateProgramInfo (ui->programInfo_tableView, QString("策略名称: %1, 策略中股票数目为: %2").arg(m_strategyName).arg(m_currStrategy.size ()));
        updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3").arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));
        updateProgramInfo (ui->programInfo_tableView, QString("对冲目标: %1, 对冲笔数: %2").arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));
        updateProgramInfo (ui->programInfo_tableView, QString("读取数据"));
    }
}

void Widget::on_historyBuySaleChart_clicked()
{
    if(m_buyStrategyMap.size() == 0 || m_saleStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择买卖组合策略");
    } else {
        QString startDate = ui->chooseStartDate->date ().toString ("yyyyMMdd");
        QString endDate = ui->chooseEndDate->date ().toString ("yyyyMMdd");
        QString timeType = ui->dataFrequency->currentText();

        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;
        qDebug() << "m_buySaleStrategyFileName: " << m_buySaleStrategyFileName;
        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView, m_chartViews.count(),
                                           "", -1, macdTime,
                                           EmpytQStringIntMap(), m_buySaleStrategyFileName,
                                           m_buyStrategyMap, m_saleStrategyMap,
                                           false, startDate, endDate, timeType);
        chartView->show();
        m_chartViews.append(chartView);
    }

}

void Widget::on_realTimeBuySaleChart_clicked()
{
    if(m_buyStrategyMap.size() == 0 || m_saleStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择买卖组合策略");
    } else {
        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;

        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView, m_chartViews.count(),
                                           "", -1, macdTime,
                                           EmpytQStringIntMap(), m_buySaleStrategyFileName,
                                           m_buyStrategyMap, m_saleStrategyMap,true);
        chartView->show();
        m_chartViews.append(chartView);
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
    for(int i = 0; i < m_chartViews.size(); ++i) {
        m_chartViews[i] -> close();
    }
    if (NULL != m_announcementView) {
        m_announcementView->close();
    }
}

Widget::~Widget()
{
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

}

//    QString buyStrategyFileName = "D:/strategy/buy.xlsx";
//    QString saleStrategyFileName = "D:/strategy/sale.xlsx";
//        QString buyStrategyFileName = "//192.168.211.182/1分钟数据 20160910-20170910/买入卖出组合/第1账户_1_买入组合_因子学习.xlsx";
//        QString saleStrategyFileName = "//192.168.211.182/1分钟数据 20160910-20170910/买入卖出组合/第1账户_2_卖出组合_因子学习.xlsx";
//    QString buyStrategyFileName = "//192.168.211.182/1分钟数据 20160910-20170910/买入卖出组合/buy.xlsx";
//    QString saleStrategyFileName = "//192.168.211.182/1分钟数据 20160910-20170910/买入卖出组合/sale.xlsx";

//    QList<strategy_ceil> buyStrategy = m_excel->readStrategyDataFromExcel (buyStrategyFileName);
//    QList<strategy_ceil> saleStrategy = m_excel->readStrategyDataFromExcel (saleStrategyFileName);
//    QWidget* chartView = new ChartForm(0, ui->programInfo_tableView, m_chartViews.count(),
//                                       buyStrategy, saleStrategy,
//                                       macdTime, false, 0,
//                                       startDate, endDate, timeType);

//    QMap<QString, int> buyStrategyMap = m_excel->readStrategyData(buyStrategyFileName);
//    QMap<QString, int> saleStrategyMap = m_excel->readStrategyData(saleStrategyFileName);

//        QString buySaleStrategyFileName = "D:/strategy/test_buy_sale.xlsx";
//        QMap<QString, int> buyStrategyMap = m_excel->readStrategyData(buySaleStrategyFileName, 1);
//        QMap<QString, int> saleStrategyMap = m_excel->readStrategyData(buySaleStrategyFileName, 2);
//        qDebug() << "buyStrategyMap: " << buyStrategyMap;
//        qDebug() << "saleStrategyMap: " << saleStrategyMap;
