#include <QScatterSeries>
#include <QDialog>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QTime>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QMessageBox>
#include <QAbstractItemModel>

#include "toolfunc.h"
#include "widget.h"
#include "ui_widget.h"
#include "QChartView"
#include "setdata.h"
#include "strategymodel.h"
#include "chartform.h"

QT_CHARTS_USE_NAMESPACE

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    m_strategyTalbeView(NULL),
    m_strategyModel(NULL),
    m_strategyFileDir(""),
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
}

void Widget::setCalendarValue () {
    ui->chooseStartDate->setCalendarPopup (true);
    ui->chooseEndDate->setCalendarPopup (true);

    ui->chooseStartDate->setDate (QDate(2016, 1, 1));
    ui->chooseEndDate->setDate (QDate(2017, 1, 1));
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
    m_strategyFileDir = "D:/strategy";
//    m_strategyFileDir = "//192.168.211.182/1分钟数据 20160910-20170910";
    m_strategyModel = new StrategyModel(m_strategyFileDir);
    QStandardItemModel* tableModel = m_strategyModel->getTableModel ();

    ui->tableView->setModel (tableModel);
    ui->tableView->setColumnWidth (0, 150);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setShowGrid (false);
//    QAbstractItemModel* testMode = ui->tableView->model ();
//    qDebug() << "testMode: " << testMode;
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
    }
    qDebug() << "hedgeIndexCode: " << hedgeIndexCode << " hedgeIndexCount: " << hedgeIndexCount;
    qDebug() << "timeType: " << timeType;
    qDebug() << "EVA1Time: " << EVA1Time << ", EVA2Time: " << EVA2Time << ", DIFFTime: " << DIFFTime;
    updateProgramInfo (ui->programInfo_tableView, QString("策略名称: %1, 策略中股票数目为: %2").arg(m_strategyName).arg(m_currStrategy.size ()));
    updateProgramInfo (ui->programInfo_tableView, QString("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6")
                       .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));
    updateProgramInfo (ui->programInfo_tableView, QString("对冲目标: %1, 对冲笔数: %2").arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));
    updateProgramInfo (ui->programInfo_tableView, QString("读取数据"));
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
        if (NULL != m_chartViews.at (i)) {
            delete m_chartViews.at (i);
            m_chartViews[i] = NULL;
        }
    }
}



