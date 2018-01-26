#ifndef CHARTFORM_H
#define CHARTFORM_H

#include <QWidget>
#include <QChartView>
#include <QCategoryAxis>
#include <QChart>
#include <QVector>
#include <QString>
#include <QList>
#include <QPointF>
#include <QComboBox>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QLineSeries>
#include <QStackedBarSeries>
#include <QTimer>
#include <QTime>

#include <QThread>
#include <QObject>
#include <QMutexLocker>
#include <QMutex>

#include <QTableView>
#include "dataread.h"
#include "dataprocess.h"

#include "strategy.h"
#include "callout.h"
#include "database.h"
#include "macd.h"
#include "qmychartview.h"
#include "monitorrealtimedata.h"
#include "chartdata.h"

namespace Ui {
class ChartForm;
}

class ChartForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartForm(QWidget *parent = 0);
    ~ChartForm();
    ChartForm(QWidget *parent, QTableView* programInfoTableView, int chartViewID,
              QString startDate, QString endDate, QString timeType,
              QList<strategy_ceil> strategyList, QString strategyName,
              QString hedgeIndexCode, int hedgeIndexCount,
              QList<int> macdTime, int threadNumb = 8,
              QString databaseName="MarketData");

    ChartForm(QWidget *parent, QTableView* programInfoTableView, int chartViewID,
              QList<strategy_ceil> strategyList, QString strategyName,
              QString hedgeIndexCode, int hedgeIndexCount,
              int updateTime, QList<int> macdTime, bool isTestRealTime,
              int threadNumb = 8);

    ChartForm(QWidget *parent, QTableView* programInfoTableView, int charViewID,
              QList<strategy_ceil> strategyList, QString strategyName,
              QString hedgeIndexCode, int hedgeIndexCount,
              QList<int> macdTime, bool isTestRealTime=false,
              int threadNumb=8, int updateTime=3000,
              QString startDate ="", QString endDate="", QString timeType ="",
              QString databaseName="MarketData");

    void registSignalParamsType();
    void initCommonData();
    void initHistoryData(QString databaseName, QString timeType,
                  QList<strategy_ceil> strategyList);
    void initRealTimeData();

    QList<QStringList> allocateThreadData();

    void startReadData();
    void startProcessHistoryData();
    void releaseDataReaderSrc();
    void releaseDataProcessSrc();

    void initMonitorThread();
    void updateData();
    void updateChart();
    void updateAxis();
    void updateSeries();
    void updateMousePos();

    void setTimeAxisUpdateData();
    QCategoryAxis* getTimeAxis();

    void setPureHistoryDataChart(QList<QList<double>> allData);
    void setRealTimeHistDataChart(QList<QList<double>> allData);

    void setLayout ();
    void setVotRunoverChartView();
    void setStrategyChartView();
    void setMACDChartView();

    QCategoryAxis* getAxisX ();
    void setTheme();

    void setTestView();

    void setMouseMoveValue(int currIndex);
    void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    void KeyReleaseFunc(QEvent *event);
    double getPointXDistance();

public slots:
    void receiveOriginalData(QMap<QString, QList<QStringList>> subThreadData);
    void receiveProcessedHistoryData(QList<QList<double>> allData);

    void receivePreCloseData(double preSpread);
    void receiveRealTimeData(ChartData curChartData);

    void receiveTradeOver();

signals:
    void sendStartReadDataSignal(QString dataType);
    void sendstartProcessHistoryDataSignal(QString dataType);
    void sendCloseSignal(int ChartViewID);
    void getPreData(QList<QString> secodeList);
    void getPreData();
    void startMonitorRealTimeData();

protected:
    bool eventFilter (QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::ChartForm *ui;

private:
    bool m_isRealTime;
    bool m_bTestRealTime;
    bool m_isclosed;
    int m_chartViewID;
    QTableView* m_programInfoTableView;
    QList<strategy_ceil> m_strategy;
    QString m_strategyName;
    QString m_hedgeIndexCode;
    int m_hedgeIndexCount;

    QList<int> m_macdTime;
    QString m_startDate;
    QString m_endDate;
    QStringList m_keyValueList;
    QString m_timeType;
    QString m_timeTypeFormat;
    QString m_databaseName;
    QString m_dbhost;

    int m_threadNumb;

private:
    mutable QMutex m_mutex;
    QPoint m_mouseInitPos;
    QString m_mouseChartName;
    double m_oldPointDistance;
    bool m_setMouseTimeIndex;
    bool m_setKeyMove;
    int m_currTimeIndex;
    int m_keyMoveCount;
    int m_addedTimeNumb;
    int m_changeRefreshCondition;

    QMap<QString, int> m_indexHedgeMetaInfo;
    QMap<QString, int> m_seocdebuyCountMap;
    QStringList m_secodeNameList;

    int m_updateTime;
    QTimer m_timer;
    MonitorRealTimeData* m_monitorWorker;
    QThread m_MonitorThread;
    QMap<QString, QStringList> m_realTimeData;
    double m_preSpread;

    bool m_isLayoutSetted;

    double m_timeAxisUpdatePercent;
    QList<double> m_timeAxisUpdateData;

    QList<DataProcess*> m_dataProcessList;
    QList<QThread*> m_dataProcessThreadList;

    QList<DataRead*> m_dataReaderList;
    QList<QThread*> m_dataReaderThreadList;
    int m_readDataThreadCount;

    QMap<QString, QList<QStringList>> m_completeTableData;

    QList<double> m_timeData;
    QList<double> m_strategyData;
    double strategySpread;
    QList<double> m_votData;
    QList<MACD> m_macdData;

    int m_dataNumb;
    QString m_title;
    int m_chartXaxisTickCount;

    QLineSeries* m_preSpreadSeries;
    QLineSeries* m_strategySeries;
    QMyChartView* m_strategyChartView;
    QChart* m_strategyChart;
    Callout *m_strategyTooltip;

    QStackedBarSeries* m_votBarSeries;
    QMyChartView* m_votrunoverChartView;
    QChart* m_votrunoverChart;
    Callout *m_votrunoverTooltip;

    QLineSeries* m_diffSeries;
    QLineSeries* m_deaSeries;
    QStackedBarSeries* m_macdSeries;
    QMyChartView* m_macdChartView;
    QChart* m_macdChart;
    Callout *m_macdTooltip;

    QChartView* m_testChartView;
    QChart* m_testChart;
    Callout *m_testTooltip;

    QTime m_startTime;
    QTime m_endTime;
};

//    void initMonitorTimer();
//    void initIndexHedgeMetaInfo();
//    void checkRealTimeData();

//    QList<QPointF> m_strategyData;
//    QList<QPointF> m_strategyHedgeData;
//    QList<QPointF> m_votrunoverData;

//    QList<QPointF> computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList);
//    void setStrategyData();
//    void setVotRunoverData();
//    void setMacdData();

#endif // CHARTFORM_H
