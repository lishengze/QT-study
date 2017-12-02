#include <QChartView>
#include "chartform.h"
#include "qmychartview.h"
#include <QDebug>
#include <QPoint>

QMyChartView::QMyChartView(QT_CHARTS_NAMESPACE::QChart *chart, QWidget *parent):
    QT_CHARTS_NAMESPACE::QChartView(chart, parent) {
    m_parent = parent;
}

QMyChartView::QMyChartView(QWidget *parent):
    QChartView(parent) {
    m_parent = parent;
}

void QMyChartView::mouseMoveEvent (QMouseEvent *event) {
    QPoint curPoint = event->pos ();
//    qDebug() <<  "MyChartView MouseMove Event: " << curPoint;
    event->ignore ();
}
