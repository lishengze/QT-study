#include <QDebug>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QChartView>
#include <QPointF>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QScatterSeries>
#include <QGridLayout>
#include "chartform.h"
#include "ui_chartform.h"
#include "toolfunc.h"
#include "macd.h"
#include "callout.h"

ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget (ui->gridLayoutWidget);
}

ChartForm::ChartForm(QWidget *parent, int chartViewID,
                     QString startDate,QString endDate,
                     QString timeType, QList<strategy_ceil> strategyList,
                     int EVA1Time, int EVA2Time, int DIFFTime,
                     QString databaseName):
    QWidget(parent),
    m_startDate(startDate), m_endDate(endDate),
    m_timeType(timeType), m_chartViewID(chartViewID),
    m_EVA1Time(EVA1Time), m_EVA2Time(EVA2Time), m_DIFFTime(DIFFTime),
    m_database(NULL), m_macdTooltip(NULL), m_strategyTooltip(NULL), m_votrunoverTooltip(NULL),
    ui(new Ui::ChartForm)
{
    initData(strategyList, databaseName, timeType);
    setOpenPriceChartView ();
    setVotRunoverChartView ();
    setStrategyChartView();
    setMACDChartView();
    setLayout();
    setThemeBox ();
}

ChartForm::~ChartForm()
{
    delete ui;
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
    if (NULL != m_macdTooltip) {
        delete m_macdTooltip;
        m_macdTooltip = NULL;
    }
    if (NULL != m_strategyTooltip) {
        delete m_strategyTooltip;
        m_strategyTooltip = NULL;
    }
    if (NULL != m_votrunoverTooltip) {
        delete m_votrunoverTooltip;
        m_votrunoverTooltip = NULL;
    }
}

void ChartForm::initData (QList<strategy_ceil> strategyList, QString databaseName, QString timeType) {
    m_strategy = strategyList;
    m_dbhost = "192.168.211.165";
//    m_dbhost = "localhost";
    m_database = new Database(QString(m_chartViewID), m_dbhost);
    m_databaseName = databaseName + "_" + timeType;
    setStrategyData();
}

void ChartForm::setLayout () {
    ui->setupUi(this);
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget (ui->gridLayoutWidget);

    ui->gridLayout->addWidget (m_strategyChartView, 1, 0);
    ui->gridLayout->addWidget (m_votrunoverChartView, 2, 0);
    ui->gridLayout->addWidget (m_macdChartView, 3, 0);
}

void ChartForm::setThemeBox () {
    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
    ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);
}

void ChartForm::setStrategyData() {
    for (int i = 0; i < m_strategy.size (); ++i) {
        QString tableName = m_strategy[i].m_secode;
        int buyCount = m_strategy[i].m_buyCount;
        QList<QPointF> curTableData = m_database->getOriChartData (m_startDate, m_endDate, "TCLOSE", tableName, m_databaseName);
        m_tableDataList.append (sortPointFList(curTableData));
        m_buyCountList.append (buyCount);
        qDebug() << "tableName: " << tableName << " datacount: " << curTableData.size ();
    }
    m_strategyData = computeStrategyData(m_tableDataList, m_buyCountList);
}

void ChartForm::setOpenPriceChartView () {
    QLineSeries* series = new QLineSeries();
    series->append (m_database->getOriChartData (m_startDate, m_endDate, "TOPEN", "SH600000", m_databaseName));

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

void ChartForm::setStrategyChartView () {
    QLineSeries* series = new QLineSeries();
    if (m_strategyData.size () == 0) {
        ErrorMessage ("No strategy Data!");
    }
    series->append (m_strategyData);

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

    connect (series, SIGNAL(hovered(QPointF,bool)), this, SLOT(strategyToolTip(QPointF,bool)));
}

void ChartForm::setVotRunoverChartView () {
    QLineSeries* series = new QLineSeries;
    series->append (m_database->getOriChartData (m_startDate, m_endDate, "VOTRUNOVER", "SH600000", m_databaseName));

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

    connect (series, SIGNAL(hovered(QPointF,bool)), this, SLOT(votRunoverToolTip(QPointF,bool)));
}

void ChartForm::setMACDChartView () {
    QList<double> oriData;
    for (int i = 0; i<m_strategyData.size (); ++i) {
        oriData.append (m_strategyData.at(i).y ());
    }
    QList<MACD> macdData = computeMACD (oriData, m_EVA1Time, m_EVA2Time, m_DIFFTime);
    QList<QPointF> diffPoint;
    QList<QPointF> deaPoint;
    QList<QPointF> macdPoint;

    for (int i = 0; i<macdData.size (); ++i) {
        diffPoint.append (QPointF(m_strategyData.at(i).x (), macdData.at(i).m_diff));
        deaPoint.append (QPointF(m_strategyData.at(i).x (), macdData.at(i).m_dea));
        macdPoint.append (QPointF(m_strategyData.at(i).x (), macdData.at(i).m_macd));
    }

    QLineSeries* diffSeries = new QLineSeries();
    diffSeries->setName("DIFF");
    QLineSeries* deaSeries = new QLineSeries();
    deaSeries->setName("DEA");
    QLineSeries* macdSeries = new QLineSeries();
    macdSeries->setName("MACD");

    diffSeries->append (diffPoint);
    deaSeries->append (deaPoint);
    macdSeries->append (macdPoint);

    m_macdChart = new QChart();
    m_macdChart->addSeries (diffSeries);
    m_macdChart->addSeries (deaSeries);
    m_macdChart->addSeries (macdSeries);
    m_macdChart->setTitle(QString::fromLocal8Bit("MACD Chart"));
    m_macdChart->legend()->setAlignment(Qt::AlignRight);

    m_macdChartView = new QChartView(m_macdChart);
    m_macdChartView->setRenderHint(QPainter::Antialiasing);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd h:mm");
    axisX->setTitleText("Date");
    m_macdChart->addAxis(axisX, Qt::AlignBottom);

    diffSeries->attachAxis(axisX);
    deaSeries->attachAxis (axisX);
    macdSeries->attachAxis (axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Yield ");
    QList<double>  axisYRange = getMACDRange(macdData);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    m_macdChart->addAxis(axisY, Qt::AlignLeft);

    macdSeries->attachAxis (axisY);
    diffSeries->attachAxis(axisY);
    deaSeries->attachAxis (axisY);

    connect (diffSeries, SIGNAL(hovered(QPointF,bool)), this, SLOT(macdToolTip(QPointF,bool)));
    connect (deaSeries, SIGNAL(hovered(QPointF,bool)), this, SLOT(macdToolTip(QPointF,bool)));
    connect (macdSeries, SIGNAL(hovered(QPointF,bool)), this, SLOT(macdToolTip(QPointF,bool)));
    this->setMouseTracking(true);

//    m_macdChart->createDefaultAxes ();
}

void ChartForm::macdToolTip (QPointF point, bool state)
{
//    qDebug() << "hover MACDToolTip" << endl;
    if (m_macdTooltip == 0)
        m_macdTooltip = new Callout(m_macdChart);

    if (state) {
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(point.x());
        m_macdTooltip->setText(QString("X: %1 \nY: %2 ").arg(curDatetime.toString ("yyyy-MM-dd h:mm")).arg(point.y()));
        m_macdTooltip->setAnchor(point);
        m_macdTooltip->setZValue(11);
        m_macdTooltip->updateGeometry();
        m_macdTooltip->show();
    } else {
        m_macdTooltip->hide();
    }
}

void ChartForm::strategyToolTip (QPointF point, bool state)
{
//    qDebug() << "hover MACDToolTip" << endl;
    if (m_strategyTooltip == 0)
        m_strategyTooltip = new Callout(m_strategyChart);

    if (state) {
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(point.x());
        m_strategyTooltip->setText(QString("X: %1 \nY: %2 ").arg(curDatetime.toString ("yyyy-MM-dd h:mm")).arg(point.y()));
        m_strategyTooltip->setAnchor(point);
        m_strategyTooltip->setZValue(11);
        m_strategyTooltip->updateGeometry();
        m_strategyTooltip->show();
    } else {
        m_strategyTooltip->hide();
    }
}

void ChartForm::votRunoverToolTip (QPointF point, bool state)
{
//    qDebug() << "hover MACDToolTip" << endl;
    if (m_votrunoverTooltip == 0)
        m_votrunoverTooltip = new Callout(m_votrunoverChart);

    if (state) {
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(point.x());
        m_votrunoverTooltip->setText(QString("X: %1 \nY: %2 ").arg(curDatetime.toString ("yyyy-MM-dd h:mm")).arg(point.y()));
        m_votrunoverTooltip->setAnchor(point);
        m_votrunoverTooltip->setZValue(11);
        m_votrunoverTooltip->updateGeometry();
        m_votrunoverTooltip->show();
    } else {
        m_votrunoverTooltip->hide();
    }
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
