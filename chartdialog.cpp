#include <QDebug>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QChartView>
#include <QScatterSeries>
#include <QDialog>
#include <QPointF>
#include "setdata.h"
#include "chartdialog.h"
#include "ui_chartdialog.h"

chartDialog::chartDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chartDialog)
{
    qDebug() << "chartDialog: chartDialog";
    ui->setupUi(this);
    setChartView();
    ui->gridLayout->addWidget (m_chartView, 1, 0);
}

chartDialog::chartDialog(QWidget *parent, QChartView* chartView) :
    QDialog(parent),
    ui(new Ui::chartDialog)
{
    ui->setupUi(this);
    setChartView();
    ui->gridLayout->addWidget (chartView, 1, 0);
    qDebug() << "add chart";
}

void chartDialog::setChartView () {
    qDebug() << "chartDialog: setChartView";
    m_chart = new QChart();
    m_chart->setTitle("Test Show Database Data");
    m_chart->legend()->hide();

    m_chartView = new QChartView(m_chart);

//    QLineSeries* series = new QLineSeries;
    QScatterSeries *series = new QScatterSeries(m_chart);
    m_chart->addSeries (series);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);

//    int pointNumb = 100;
//    for(int i=0;i<pointNumb;++i){
//       series->append(i,i);
//    }

    series->append (getDatabaseData());
//    setSeriresData(series);

    QList<QPointF> datalist = series->points ();
    qDebug() << "series points.count: " << datalist.count();
    qDebug() << datalist[0] << endl;

//    m_coordX = new QGraphicsSimpleTextItem(m_chart);
//    m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height());
//    m_coordX->setText("X: ");
//    m_coordY = new QGraphicsSimpleTextItem(m_chart);
//    m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height());
//    m_coordY->setText("Y: ");

    connect(series, SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
    connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));

    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText ("Time");
    axisX->setRange(0,1000);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText ("TOPEN");
    axisY->setRange(12,13);

    m_chart->setAxisX (axisX, series);
    m_chart->setAxisY (axisY, series);

//    m_chart->createDefaultAxes();
}

chartDialog::~chartDialog()
{
    delete ui;
}

void chartDialog::on_changeTheme_clicked()
{

}
