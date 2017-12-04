#ifndef CHARTFORM_H
#define CHARTFORM_H

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QVector>
#include <QString>
#include <QList>
#include <QPointF>
#include <QComboBox>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QResizeEvent>
#include "strategy.h"
#include "callout.h"
#include "database.h"
#include "macd.h"
#include "qmychartview.h"

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
              QString timeType, QList<strategy_ceil> strategy, QString strategyName,
              int EVA1Time, int EVA2Time, int DIFFTime,
              QString databaseName="MarketData");

    void initData(QList<strategy_ceil> strategyList, QString databaseName, QString timeType);
    void setStrategyData();
    void setVotRunoverData();
    void setMacdData();

    void setLayout ();
    void setVotRunoverChartView();
    void setStrategyChartView();
    void setMACDChartView();

    void setTheme();

    void setTestView();
//    void setThemeBox();
//    void setTextItem();
    void setMouseMoveValue(int currIndex);

    QList<QPointF> computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList);
    QList<double> getChartYvalueRange(QList<QPointF> pointList );


protected:
//    void resizeEvent(QResizeEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter (QObject *watched, QEvent *event);

public slots:
//    void macdToolTip(QPointF point, bool state);
//    void strategyToolTip(QPointF point, bool state);
//    void votRunoverToolTip(QPointF point, bool state);

private:
    Ui::ChartForm *ui;
    QString m_title;

    int m_chartXaxisTickCount;
    QChartView* m_openPriceChartView;
    QChart* m_openPriceChart;

//    QChartView* m_strategyChartView;
    QMyChartView* m_strategyChartView;
    QChart* m_strategyChart;
    Callout *m_strategyTooltip;
    QList<QPointF> m_strategyData;

    QMyChartView* m_votrunoverChartView;
    QChart* m_votrunoverChart;
    Callout *m_votrunoverTooltip;
    QList<QPointF> m_votrunoverData;

    QMyChartView* m_macdChartView;
    QChart* m_macdChart;
    Callout *m_macdTooltip;
    QList<MACD> m_macdData;

    QChartView* m_testChartView;
//    QMyChartView* m_testChartView;
    QChart* m_testChart;
    Callout *m_testTooltip;

    QString m_startDate;
    QString m_endDate;

    int m_chartViewID;

    QList<strategy_ceil> m_strategy;
    QString m_strategyName;
    QList<QList<QPointF>> m_tableDataList;
    QList<int> m_buyCountList;

    QString m_timeType;
    QString m_timeTypeFormat;
    QString m_databaseName;
    Database* m_database;
    QString m_dbhost;

    int m_EVA1Time;
    int m_EVA2Time;
    int m_DIFFTime;

    QGraphicsSimpleTextItem* m_timeItem;
    QGraphicsSimpleTextItem* m_targetItem;
    QGraphicsSimpleTextItem* m_votrunoverItem;
    QGraphicsSimpleTextItem* m_diffItem;
    QGraphicsSimpleTextItem* m_deaItem;
    QGraphicsSimpleTextItem* m_macdItem;

};

#endif // CHARTFORM_H
