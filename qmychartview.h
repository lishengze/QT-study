#ifndef QMYCHARTVIEW_H
#define QMYCHARTVIEW_H
#include <QChartView>
#include <QWidget>
#include <QChart>
#include <QMouseEvent>
#include <qchartview.h>
//#include "chartform.h"

class ChartForm;

class QMyChartView: public QT_CHARTS_NAMESPACE::QChartView
{
public:
    QMyChartView(QWidget *parent = Q_NULLPTR);
    QMyChartView(QT_CHARTS_NAMESPACE::QChart *chart, QWidget *parent = Q_NULLPTR);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QWidget* m_parent;
};

#endif // QMYCHARTVIEW_H
