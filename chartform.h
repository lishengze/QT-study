#ifndef CHARTFORM_H
#define CHARTFORM_H

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QVector>
#include <QString>
#include <QList>
#include <QPointF>
#include "strategy.h"
#include "callout.h"
#include "database.h"

namespace Ui {
class ChartForm;
}

class ChartForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartForm(QWidget *parent = 0);
    ~ChartForm();
    ChartForm(QWidget *parent, int charViewID,
                QString startDate, QString endDate,
                QList<strategy_ceil> strategy);

    void setOpenPriceChartView();
    void setVotRunoverChartView();
    void setStrategyChartView();

    QList<QPointF> computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList);

private:
    Ui::ChartForm *ui;

    QChartView* m_openPriceChartView;
    QChart* m_openPriceChart;

    QChartView* m_strategyChartView;
    QChart* m_strategyChart;

    QChartView* m_votrunoverChartView;
    QChart* m_votrunoverChart;

    QString m_startDate;
    QString m_endDate;

    int m_chartViewID;
    QList<strategy_ceil> m_strategy;
    QList<QList<QPointF>> m_tableDataList;
    QList<int> m_buyCountList;

    Database* m_database;
};

#endif // CHARTFORM_H
