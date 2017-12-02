#include "testform.h"
#include "ui_testform.h"
#include <QLineSeries>
#include <QChart>
#include <QGridLayout>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

TestForm::TestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestForm)
{
    ui->setupUi(this);
    QLineSeries* series = new QLineSeries;
    int pointNumb = 5;
    for (int i = 0; i < pointNumb; ++i) {
        series->append(i,i);
    }

    m_chart = new QChart;
    m_chart->addSeries(series);
    m_chart->setTitle("Test mouseMoveEvent");
    m_chartView = new QMyChartView(m_chart);
    m_chartView->setMouseTracking(true);
    m_chartView->installEventFilter(this);

    m_chart->createDefaultAxes();

    ui->gridLayout->addWidget(m_chartView,1,0);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(ui->gridLayoutWidget);
}

TestForm::~TestForm()
{
    delete ui;
}

void TestForm::mouseMoveEvent (QMouseEvent *event) {
    QPoint curPoint = event->pos ();
//    qDebug() << "TestForm mouseMoveEvent: " << curPoint;
    QPointF curChartPoint = m_chart->mapToValue (curPoint);
    qDebug() << "TestForm ChartPoint: " << curChartPoint;
}

bool TestForm::eventFilter (QObject *watched, QEvent *event) {
//    qDebug() << "event";
//    qDebug() <<"EventType:" <<  event->type();
//    qDebug() << watched;
    if (event->type () == QEvent::MouseMove) {
        if (watched == m_chartView) {
            qDebug() << "this is chartView";
        }
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QPoint curPoint = mouseEvent->pos ();
//        qDebug() << "EventFilter MouseMove!" << curPoint;
        QPointF curChartPoint = m_chart->mapToValue (curPoint);
        qDebug() << "EventFilter ChartPoint: " << curChartPoint;
    }
    return QWidget::eventFilter (watched, event);
}
