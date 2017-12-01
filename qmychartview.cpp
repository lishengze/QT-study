#include <QChartView>
#include "qmychartview.h"
#include <QDebug>

QMyChartView::QMyChartView(QWidget *parent)
{

}

void QMyChartView::mouseMoveEvent (QMouseEvent *event) {
    qDebug() << event;
    event->ignore ();
}
