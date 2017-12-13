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

#include "widget.h"
#include "ui_widget.h"
#include "QChartView"
#include "setdata.h"
#include "strategymodel.h"
#include "chartform.h"

QT_CHARTS_USE_NAMESPACE
const int MAXSIZE = 500;
const int TIMEINTERVAL = 100;
QChart *g_chart;
QLineSeries *g_series;
int g_timeId;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    m_coordX(0),
    m_coordY(0),
    m_chart(0),
    m_tooltip(0),
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
    setTableView();
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

void Widget::setTableView () {
    m_strategyFileDir = "D:/strategy";
//    m_strategyFileDir = "//192.168.211.182/1分钟数据 20160910-20170910";
    m_strategyModel = new StrategyModel(m_strategyFileDir);
    QStandardItemModel* tableModel = m_strategyModel->getTableModel ();

    ui->tableView->setModel (tableModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

    qDebug() << "hedgeIndexCode: " << hedgeIndexCode << " hedgeIndexCount: " << hedgeIndexCount;
    qDebug() << "timeType: " << timeType;
    qDebug() << "EVA1Time: " << EVA1Time << ", EVA2Time: " << EVA2Time << ", DIFFTime: " << DIFFTime;
    if (startDate.toInt () >= endDate.toInt ()) {
        QMessageBox::critical(NULL, "Error", "起始时间晚于截止时间");
    } else if (m_currStrategy.size () == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择策略");
    } else {
        QWidget* charView = new ChartForm(0, m_chartViews.count(),
                                          startDate, endDate, timeType,
                                          m_currStrategy, m_strategyName,
                                          hedgeIndexCode, hedgeIndexCount,
                                          macdTime);
        charView->show ();
        m_chartViews.append (charView);
    }
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
    QString strategyFullFileName = m_strategyModel->getStrategyFullFileName (intIndex);
    m_currStrategy = m_excel->readStrategyDataFromExcel (strategyFullFileName);
//    for (int i = 0; i < m_currStrategy.size(); ++i) {
//        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
//                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
//    }
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

//void Widget::mouseMoveEvent(QMouseEvent *event)
//{
////    qDebug() << "mouseMoveEvent" << endl;
//    m_coordX->setText(QString("X: %1").arg(m_chart->mapToValue(event->pos()).x()));
//    m_coordY->setText(QString("Y: %1").arg(m_chart->mapToValue(event->pos()).y()));
////    QGraphicsView::mouseMoveEvent(event);
//}

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

//double getData(double time){
//    double s = qCos( time * M_PI * 2 ) ;
//    return s;
//}

//QChartView * getChartView(){
//    g_chart = new QChart;
//    QChartView *chartView = new QChartView(g_chart);
////    v.setRubberBand(QChartView::HorizontalRubberBand);
//    chartView->setRubberBand(QChartView::RectangleRubberBand);

//    g_series = new QLineSeries;
//    g_chart->addSeries(g_series);

//    for(int i=0;i<MAXSIZE;++i){
//       g_series->append(i,0);
//    }
//    g_series->setUseOpenGL(true);//openGl 加速
////    qDebug()<<g_series->useOpenGL();

//    QValueAxis *axisX = new QValueAxis;
//    axisX->setRange(0,MAXSIZE);
////    axisX->setLabelFormat("%g");
//    axisX->setTitleText("axisX");

//    QValueAxis *axisY = new QValueAxis;
//    axisY->setRange(-1.5,1.5);
//    axisY->setTitleText("axisY");

//    g_chart->setAxisX(axisX,g_series);
//    g_chart->setAxisY(axisY,g_series);
//    g_chart->legend()->hide();
//    g_chart->setTitle("demo");
//    return chartView;
//}

//void Widget::setChartView(){
//    m_chart = new QChart();
//    m_chart->setTitle("Test Show Database Data");
//    m_chart->legend()->hide();

//    m_chartView = new QChartView(m_chart);

//    QLineSeries* series = new QLineSeries;
////    QScatterSeries *series = new QScatterSeries(m_chart);
//    m_chart->addSeries (series);
//    m_chartView->setRubberBand(QChartView::RectangleRubberBand);

////    int pointNumb = 100;
////    for(int i=0;i<pointNumb;++i){
////       series->append(i,i);
////    }

//    series->append (getOpenPrice());
////    setSeriresData(series);

//    QList<QPointF> datalist = series->points ();
//    qDebug() << "series points.count: " << datalist.count();
////    qDebug() << datalist[0] << endl;

////    m_coordX = new QGraphicsSimpleTextItem(m_chart);
////    m_coordX->setPos(m_chart->size().width()/2 - 50, m_chart->size().height());
////    m_coordX->setText("X: ");
////    m_coordY = new QGraphicsSimpleTextItem(m_chart);
////    m_coordY->setPos(m_chart->size().width()/2 + 50, m_chart->size().height());
////    m_coordY->setText("Y: ");

//    connect(series, SIGNAL(clicked(QPointF)), this, SLOT(keepCallout()));
//    connect(series, SIGNAL(hovered(QPointF, bool)), this, SLOT(tooltip(QPointF,bool)));

//    QValueAxis* axisX = new QValueAxis();
//    axisX->setTitleText ("Time");
//    axisX->setRange(0,1000);

//    QValueAxis* axisY = new QValueAxis();
//    axisY->setTitleText ("TOPEN");
//    axisY->setRange(12,13);

//    m_chart->setAxisX (axisX, series);
//    m_chart->setAxisY (axisY, series);

////    m_chart->createDefaultAxes();
//}
