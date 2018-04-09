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
#include <QCategoryAxis>
#include "dataread.h"
#include "dataprocess.h"
#include  "historydata.h"

#include "strategy.h"
#include "database.h"
#include "macd.h"
#include "qmychartview.h"
#include "monitorrealtimedata.h"
#include "chartdata.h"
#include "toolfunc.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class ChartForm;
}

class ChartForm : public QWidget
{
    Q_OBJECT

public:
    ChartForm(QWidget *parent, QTableView* programInfoTableView,
              int chartViewID, bool isBuySalePortfolio,
              QString hedgeIndexCode, int hedgeIndexCount, QList<int> macdTime,
              QMap<QString, int> strategyMap = EmpytQStringIntMap(), QString strategyName="",
              QMap<QString, int> buyStrategyList = EmpytQStringIntMap(),
              QMap<QString, int> saleStrategyList = EmpytQStringIntMap(),
              bool isRealTime = true, QString startDate ="", QString endDate="", QString timeType ="");

    ~ChartForm();

    void registSignalParamsType();
    void initCommonData();
    void initSecodeList();
    void initSecodeStyle();
    void initHistoryData();
    void initRealTimeData();

    void initHistdataThread();
    void initMonitorThread();

    void updateData();
    void updateChart();
    void updateAxis();
    void updateSeries();
    void updateMousePos();

    void setLayout ();
    void setTheme();
    void setVotRunoverChartView();
    void setStrategyChartView();
    void setMACDChartView();

    QCategoryAxis* getAxisX ();
    QCategoryAxis* getTimeAxis();
    void setTimeAxisUpdateData();

    void setMouseMoveValue(int currIndex);
    void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    void KeyReleaseFunc(QEvent *event);
    double getPointXDistance();

public slots:
    void receivePreCloseData(double preSpread);
    void receiveRealTimeData(ChartData curChartData);

    void receiveTableViewInfo(QString msg);
    void receiveHistData(QList<QList<double>> allData);

    void receiveTradeOver();

signals:
    void getHistDataSignal();
    void sendStartReadDataSignal(QString dataType);
    void sendstartProcessHistoryDataSignal(QString dataType);
    void sendCloseSignal(int ChartViewID);
    void getPreData(QList<QString> secodeList);
    void getPreCloseSpread();
    void startMonitorRealTimeData();

protected:
    bool eventFilter (QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::ChartForm *ui;

private:
    int                  m_chartViewID;
    bool                 m_isRealTime;
    bool                 m_isBuySalePortfolio;

    QString              m_databaseName;
    QString              m_dbhost;
    QList<int>           m_macdTime;
    QString              m_startDate;
    QString              m_endDate;
    QString              m_timeType;
    QString              m_hedgeIndexCode;
    int                  m_hedgeIndexCount;
    QTableView*          m_programInfoTableView;

    QString              m_strategyName;
    QMap<QString, int>   m_buyStrategyMap;
    QMap<QString, int>   m_saleStrategyMap;
    QMap<QString, int>   m_secodebuyCountMap;
    QStringList          m_secodeNameList;

    QStringList          m_keyValueList;
    QString              m_secodeStyle;
    QString              m_timeTypeFormat;
    int                  m_threadNumb;
    int                  m_updateTime;

    MonitorRealTimeData* m_monitorWorker;
    QThread              m_MonitorThread;
    HistoryData*         m_histdataWorker;
    QThread              m_histdataThread;
    double               m_preSpread;

    QList<double>        m_timeData;
    QList<double>        m_strategyData;
    QList<double>        m_votData;
    QList<MACD>          m_macdData;

    QTime                m_startTime;
    QTime                m_endTime;

private:
    QString              m_title;
    bool                 m_isLayoutSetted;
    double               m_timeAxisUpdatePercent;
    QList<double>        m_timeAxisUpdateData;
    int                  m_chartXaxisTickCount;

    QPoint               m_mouseInitPos;
    QString              m_mouseChartName;
    double               m_oldPointDistance;
    bool                 m_setMouseTimeIndex;
    bool                 m_setKeyMove;
    int                  m_currTimeIndex;
    int                  m_keyMoveCount;

    QLineSeries*         m_strategySeries;
    QMyChartView*        m_strategyChartView;
    QChart*              m_strategyChart;
    QLineSeries*         m_preSpreadSeries;

    QStackedBarSeries*   m_votBarSeries;
    QMyChartView*        m_votrunoverChartView;
    QChart*              m_votrunoverChart;

    QLineSeries*         m_diffSeries;
    QLineSeries*         m_deaSeries;
    QStackedBarSeries*   m_macdSeries;
    QMyChartView*        m_macdChartView;
    QChart*              m_macdChart;

    bool                 m_isclosed;
};

#endif // CHARTFORM_H
