#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QtCharts/QChart"
#include <QChartView>
#include "callout.h"
#include <QGraphicsSimpleTextItem>
#include <QList>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QDialog>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    void timerEvent(QTimerEvent* event);
    void setChartView();
    ~Widget();

//    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:
//    void keepCallout();
//    void tooltip(QPointF point, bool state);

private slots:
    void on_historyData_clicked();

    void on_chooseStartDate_editingFinished();

private:
    Ui::Widget *ui;
    QWidget* m_chartWidegt;
    QDialog* m_chartDialog;
    QChart* m_chart;
    QChartView* m_chartView;
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;
};

#endif // WIDGET_H
