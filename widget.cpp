#include "widget.h"
#include "ui_widget.h"
#include "QtCharts/QChart"
#include "QLineSeries"
#include "QValueAxis"
#include "QTimer"
#include "QTime"
#include "QList"
#include "qmath.h"
#include "QPointF"
#include "QDebug"
#include "QChartView"
#include "setdata.h"
#include <QScatterSeries>
#include <QDialog>
#include <QDateTime>
#include "chartdialog.h"

QT_CHARTS_USE_NAMESPACE
const int MAXSIZE = 500;
const int TIMEINTERVAL = 100;
QChart *g_chart;
QLineSeries *g_series;
int g_timeId;


QChartView * getChartView(){
    g_chart = new QChart;
    QChartView *chartView = new QChartView(g_chart);
//    v.setRubberBand(QChartView::HorizontalRubberBand);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    g_series = new QLineSeries;
    g_chart->addSeries(g_series);

    for(int i=0;i<MAXSIZE;++i){
       g_series->append(i,0);
    }
    g_series->setUseOpenGL(true);//openGl 加速
//    qDebug()<<g_series->useOpenGL();

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0,MAXSIZE);
//    axisX->setLabelFormat("%g");
    axisX->setTitleText("axisX");

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-1.5,1.5);
    axisY->setTitleText("axisY");

    g_chart->setAxisX(axisX,g_series);
    g_chart->setAxisY(axisY,g_series);
    g_chart->legend()->hide();
    g_chart->setTitle("demo");
    return chartView;
}

void Widget::setChartView(){
    m_chart = new QChart();
    m_chart->setTitle("Test Show Database Data");
    m_chart->legend()->hide();

    m_chartView = new QChartView(m_chart);

    QLineSeries* series = new QLineSeries;
//    QScatterSeries *series = new QScatterSeries(m_chart);
    m_chart->addSeries (series);
    m_chartView->setRubberBand(QChartView::RectangleRubberBand);

//    int pointNumb = 100;
//    for(int i=0;i<pointNumb;++i){
//       series->append(i,i);
//    }

    series->append (getOpenPrice());
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

double getData(double time){
    double s = qCos( time * M_PI * 2 ) ;
    return s;
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    m_coordX(0),
    m_coordY(0),
    m_chart(0),
    m_tooltip(0),
    m_chartWidegt(NULL),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->chooseStartDate->setCalendarPopup (true);
    ui->chooseEndDate->setCalendarPopup (true);
//    setChartView ();
//    ui->gridLayout->addWidget (m_chartView, 1, 0);
    ui->gridLayout->addWidget (getChartView(), 2, 0);
//    g_timeId = startTimer(TIMEINTERVAL);
}

void Widget::timerEvent (QTimerEvent *event) {
    if(event->timerId()==g_timeId){//定时器到时间,//模拟数据填充
        static QTime dataTime(QTime::currentTime());
        long int eltime = dataTime.elapsed();
        static int lastpointtime = 0;
        int size = (eltime - lastpointtime);//数据个数
//        qDebug()<<"size-->"<<size;
        if(isVisible()){
            QVector<QPointF> oldPoints = g_series->pointsVector();//Returns the points in the series as a vector
            QVector<QPointF> points;

            for(int i=size;i<oldPoints.count();++i){
                points.append(QPointF(i-size ,oldPoints.at(i).y()));//替换数据用
            }
            qint64 sizePoints = points.count();
            for(int k=0;k<size;++k){
                points.append(QPointF(k+sizePoints,getData((((double)lastpointtime+k+1)/1000))));
            }
            g_series->replace(points);
            lastpointtime = eltime;
       }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "mouseMoveEvent" << endl;
    m_coordX->setText(QString("X: %1").arg(m_chart->mapToValue(event->pos()).x()));
    m_coordY->setText(QString("Y: %1").arg(m_chart->mapToValue(event->pos()).y()));
//    QGraphicsView::mouseMoveEvent(event);
}

//void Widget::keepCallout()
//{
//    qDebug() << "clicked keepCallout" << endl;
//    m_callouts.append(m_tooltip);
//    m_tooltip = new Callout(m_chart);
//}

//void Widget::tooltip(QPointF point, bool state)
//{
//    qDebug() << "hover tooltip" << endl;
//    if (m_tooltip == 0)
//        m_tooltip = new Callout(m_chart);

//    if (state) {
//        m_tooltip->setText(QString("X: %1 \nY: %2 ").arg(point.x()).arg(point.y()));
//        m_tooltip->setAnchor(point);
//        m_tooltip->setZValue(11);
//        m_tooltip->updateGeometry();
//        m_tooltip->show();
//    } else {
//        m_tooltip->hide();
//    }
//}


Widget::~Widget()
{
    delete ui;
}

void Widget::on_historyData_clicked()
{
    m_chartDialog = new chartDialog(this);
//    m_chartDialog = new chartDialog(this, m_chartView);
    m_chartDialog->show ();
}

void Widget::on_chooseStartDate_editingFinished()
{
    QDate startDate = ui->chooseStartDate->date ();
    qDebug() << "startDate: " << startDate;
}
