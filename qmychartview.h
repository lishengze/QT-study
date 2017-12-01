#ifndef QMYCHARTVIEW_H
#define QMYCHARTVIEW_H
#include <QChartView>
#include <QWidget>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
class QChartView;
QT_END_NAMESPACE

class QMyChartView
{
public:
    QMyChartView(QWidget *parent = Q_NULLPTR);

protected:

    void mouseMoveEvent(QMouseEvent *event);

};

#endif // QMYCHARTVIEW_H
