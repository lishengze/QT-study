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
#include <QTableView>
#include "strategymodel.h"
#include "database.h"
#include "excel.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    void setTableView();
    void setDataFrequency();
    void setMacdTime();

    ~Widget();

//    void resizeEvent(QResizeEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void timerEvent(QTimerEvent* event);

public slots:
//    void keepCallout();
//    void tooltip(QPointF point, bool state);

private slots:
    void on_historyData_clicked();

    void on_chooseStartDate_editingFinished();

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::Widget *ui;
    QList<QWidget*> m_chartViews;
    QDialog* m_chartDialog;
    QChart* m_chart;
    QChartView* m_chartView;
    QGraphicsSimpleTextItem *m_coordX;
    QGraphicsSimpleTextItem *m_coordY;
    Callout *m_tooltip;
    QList<Callout *> m_callouts;

    StrategyModel* m_strategyModel;
    QTableView* m_strategyTalbeView;
    QString m_strategyFileDir;
    QList<strategy_ceil> m_currStrategy;

    Excel* m_excel;
};

#endif // WIDGET_H
