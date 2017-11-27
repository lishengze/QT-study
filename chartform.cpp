#include <QDebug>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QChartView>
#include <QPointF>
#include <QDateTimeAxis>
#include <QScatterSeries>
#include <QGridLayout>
#include "chartform.h"
#include "ui_chartform.h"
#include "toolfunc.h"

ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget (ui->gridLayoutWidget);
}

ChartForm::ChartForm(QWidget *parent, int chartViewID, QString startDate,
                     QString endDate, QList<strategy_ceil> strategyList):
    QWidget(parent),
    m_startDate(startDate),
    m_endDate(endDate),
    m_chartViewID(chartViewID),
    m_database(NULL),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
    m_strategy = strategyList;
    m_database = new Database(QString(m_chartViewID));
    setOpenPriceChartView ();
    setVotRunoverChartView ();
    setStrategyChartView();

    ui->gridLayout->addWidget (m_openPriceChartView, 1, 0);
    ui->gridLayout->addWidget (m_votrunoverChartView, 2, 0);
    ui->gridLayout->addWidget (m_strategyChartView, 3, 0);
}

ChartForm::~ChartForm()
{
    delete ui;
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
}

void ChartForm::setOpenPriceChartView () {
    QLineSeries* series = new QLineSeries();
    series->append (m_database->getOriChartData (m_startDate, m_endDate, "TOPEN", "SH600000"));

    QList<QPointF> datalist = series->points ();
    qDebug() << "OpenPrice points.count: " << datalist.count();

    m_openPriceChart = new QChart();
    m_openPriceChart->addSeries (series);
    m_openPriceChart->setTitle("Open Price Chart");
    m_openPriceChart->legend()->hide();

    m_openPriceChartView = new QChartView(m_openPriceChart);
    m_openPriceChartView->setRenderHint(QPainter::Antialiasing);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd h:mm");
    axisX->setTitleText("Date");
    m_openPriceChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("OPEN PRICE");
    m_openPriceChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void ChartForm::setVotRunoverChartView () {
    QLineSeries* series = new QLineSeries;
    series->append (m_database->getOriChartData (m_startDate, m_endDate, "VOTRUNOVER", "SH600000"));

    QList<QPointF> datalist = series->points ();
    qDebug() << "Volume of Transaction points.count: " << datalist.count();

    m_votrunoverChart = new QChart();
    m_votrunoverChart->addSeries (series);
    m_votrunoverChart->setTitle("volume of transaction");
    m_votrunoverChart->legend()->hide();

    m_votrunoverChartView = new QChartView(m_votrunoverChart);
    m_votrunoverChartView->setRenderHint(QPainter::Antialiasing);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd h:mm");
    axisX->setTitleText("Date");
    m_votrunoverChart->addAxis (axisX, Qt::AlignBottom);
    series->attachAxis (axisX);

    QValueAxis* axisY = new QValueAxis;
    axisY->setTitleText ("VOTRUNOVER");
    m_votrunoverChart->addAxis (axisY,Qt::AlignLeft);
    series->attachAxis (axisY);
}

void ChartForm::setStrategyChartView () {
    QLineSeries* series = new QLineSeries();

    for (int i = 0; i < m_strategy.size (); ++i) {
        QString tableName = m_strategy[i].m_secode;
        int buyCount = m_strategy[i].m_buyCount;
        QList<QPointF> curTableData = m_database->getOriChartData (m_startDate, m_endDate, "TCLOSE", tableName);
        m_tableDataList.append (sortPointFList(curTableData));
        m_buyCountList.append (buyCount);
        qDebug() << "tableName: " << tableName << " datacount: " << curTableData.size ();
    }
    QList<QPointF> computedData = computeStrategyData(m_tableDataList, m_buyCountList);

    series->append (computedData);

    QList<QPointF> datalist = series->points ();
    qDebug() << "StrategyChart points.count: " << datalist.count();

    m_strategyChart = new QChart();
    m_strategyChart->addSeries (series);
    m_strategyChart->setTitle("Strategy Chart");
    m_strategyChart->legend()->hide();

    m_strategyChartView = new QChartView(m_strategyChart);
    m_strategyChartView->setRenderHint(QPainter::Antialiasing);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd h:mm");
    axisX->setTitleText("Date");
    m_strategyChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Yield ");
    m_strategyChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}


QList<QPointF> ChartForm::computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList) {
    QList<QPointF> result;
    QList<QPointF> firstStockData = allTableData.at(0);
    int firstBuyCount = buyCountList.at(0);
    for (int i = 0; i < firstStockData.size (); ++i) {
        result.append (QPointF(firstStockData.at (i).x(), firstStockData.at(i).y () * firstBuyCount));
    }

    for (int i = 1; i < allTableData.size (); ++i) {
        QList<QPointF> tmpStockData = allTableData.at(i);
        int tmpBuyCount = buyCountList.at(i);
        result = mergeSortedPointedList (result, 1, tmpStockData, tmpBuyCount);
    }
    return result;
}
