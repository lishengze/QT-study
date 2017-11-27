#include <QDebug>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QChartView>
#include <QPointF>
#include <QDateTimeAxis>
#include <QScatterSeries>
#include <QDialog>
#include "setdata.h"
#include "chartdialog.h"
#include "ui_chartdialog.h"

chartDialog::chartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chartDialog)
{
    ui->setupUi(this);
    setTestChartView();
    setTimeChartView();
//    setTimeChartViewCom();
    ui->gridLayout->addWidget (m_testChartView, 1, 0);
    ui->gridLayout->addWidget (m_timeChartView, 2, 0);
}

chartDialog::chartDialog(QWidget *parent, QString startDate, QString endDate):
    QDialog(parent),
    m_startDate(startDate),
    m_endDate(endDate),
    ui(new Ui::chartDialog)
{
    ui->setupUi(this);
    setTestChartView();
    setTimeChartView();
    ui->gridLayout->addWidget (m_testChartView, 1, 0);
    ui->gridLayout->addWidget (m_timeChartView, 2, 0);
}

chartDialog::chartDialog(QWidget *parent, int chartViewID, QString startDate, QString endDate, QList<strategy_ceil> strategyList):
    QDialog(parent),
    m_startDate(startDate),
    m_endDate(endDate),
    m_chartViewID(chartViewID),
    ui(new Ui::chartDialog)
{
    m_strategy = strategyList;
    ui->setupUi(this);
    setTestChartView();
    setTimeChartView();
    ui->gridLayout->addWidget (m_testChartView, 1, 0);
    ui->gridLayout->addWidget (m_timeChartView, 2, 0);
}


void chartDialog::setTestChartView () {
    m_testChart = new QChart();
    m_testChart->setTitle("Test Show Database Data");
    m_testChart->legend()->hide();

    m_testChartView = new QChartView(m_testChart);
    m_testChartView->setRenderHint(QPainter::Antialiasing);

    QLineSeries* series = new QLineSeries;
//    series->append (getOpenPrice());
    series->append (getTimeData(m_startDate, m_endDate, "TOPEN", "SH600000"));

    m_testChart->addSeries (series);

//    QScatterSeries *series = new QScatterSeries(chart);
//    QList<QPointF> datalist = series->points ();
//    qDebug() << "series points.count: " << datalist.count();
//    qDebug() << datalist[0] << endl;

//    QValueAxis* axisX = new QValueAxis();
//    axisX->setTitleText ("Time");
//    axisX->setRange(0,1000);

//    QValueAxis* axisY = new QValueAxis();
//    axisY->setTitleText ("TOPEN");
//    axisY->setRange(12,13);

//    m_testChart->setAxisX (axisX, series);
//    m_testChart->setAxisY (axisY, series);

    QValueAxis* axisX = new QValueAxis;
    axisX->setTitleText ("Time");
    m_testChart->addAxis (axisX, Qt::AlignBottom);
    series->attachAxis (axisX);

    QValueAxis* axisY = new QValueAxis;
    axisY->setTitleText ("TOPEN");
    m_testChart->addAxis (axisY,Qt::AlignLeft);
    series->attachAxis (axisY);
}

void chartDialog::setTimeChartView () {
    QLineSeries* series = new QLineSeries();
    series->append (getTimeData(m_startDate, m_endDate, "TOPEN", "SH600000"));
//    testSetTimeData(series);

    m_timeChart = new QChart();
    m_timeChart->addSeries (series);
    m_timeChart->setTitle("Test Show Time XAxis");
    m_timeChart->legend()->hide();

    m_timeChartView = new QChartView(m_timeChart);
    m_timeChartView->setRenderHint(QPainter::Antialiasing);

//    series->append (getTimeData ());

    QList<QPointF> datalist = series->points ();
    qDebug() << "timedata series points.count: " << datalist.count();
//    qDebug() << datalist[0] << endl;

//    QDateTimeAxis* axisX = new QDateTimeAxis;
//    axisX->setTitleText ("Time");
//    axisX->setFormat("yyyy-MM--dd h:mm");
    //    QValueAxis* axisY = new QValueAxis();
    //    axisY->setTitleText ("TOPEN");
    //    axisY->setRange(12,13);

    //    m_timeChart->setAxisX (axisX, series);
    //    m_timeChart->setAxisY (axisY, series);
    //    m_timeChart->createDefaultAxes ();

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd h:mm");
    axisX->setTitleText("Date");
    m_timeChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Sunspots count");
    m_timeChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void chartDialog::setTimeChartViewCom () {
    QLineSeries* lineSeries = new QLineSeries();
    lineSeries->append (getTimeData ());

    QScatterSeries* scatterSeries = new QScatterSeries();
    scatterSeries->append (getTimeData());

    m_timeChart = new QChart();
    m_timeChart->addSeries (lineSeries);
    m_timeChart->addSeries (scatterSeries);
    m_timeChart->setTitle("Test show two charts vertically!");
    m_timeChart->legend()->hide();

    m_timeChartView = new QChartView(m_timeChart);
    m_timeChartView->setRenderHint(QPainter::Antialiasing);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("yyyy-MM-dd h:mm");
    axisX->setTitleText("Date");
    m_timeChart->addAxis(axisX, Qt::AlignBottom);
    lineSeries->attachAxis(axisX);
    scatterSeries->attachAxis (axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("OPEN");
    m_timeChart->addAxis(axisY, Qt::AlignLeft);
    lineSeries->attachAxis(axisY);

    QValueAxis *axisY2 = new QValueAxis;
    axisY2->setTitleText("OPENB");
    m_timeChart->addAxis(axisY2, Qt::AlignLeft);
    scatterSeries->attachAxis(axisY2);
}

chartDialog::~chartDialog()
{
    delete ui;
}

void chartDialog::on_changeTheme_clicked()
{

}
