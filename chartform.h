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
#include "basechart.h"
#include "dataread.h"
#include "dataprocess.h"
#include "historydata.h"

#include "database.h"
#include "macd.h"
#include "qmychartview.h"
#include "monitorrealtimedata.h"
#include "extractdatawindow.h"

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

class ChartForm : public BaseChart
{
    Q_OBJECT

public:

    ChartForm(QWidget *parent, QTableView* programInfoTableView,
              int chartViewID, QString dbhost, bool isBuySalePortfolio,
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

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);
    void writeExcelData();

    void updateData();
    void updateChart();
    virtual void updateAxis();
    virtual void updateSeries();
    virtual void updateMousePos();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();

    void setVotRunoverChartView();
    void setStrategyChartView();
    void setMACDChartView();
    void setLineColor();

    QCategoryAxis* getAxisX ();
    QCategoryAxis* getTimeAxis();
    void setTimeAxisUpdateData();
    void updateLableSeries(int index);

    virtual void setPropertyValue(int index);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

public slots:
    void receivePreCloseData(double preSpread);
    void receiveRealTimeData(ChartData curChartData);

    void receiveTableViewInfo(QString msg);
    void receiveMarketHistData_slot(QList<QList<double>> allData);

    void receiveTradeOver();
    void updatePrecloseData();

signals:
    void getHistDataSignal();
    void sendStartReadDataSignal(QString dataType);
    void sendstartProcessHistoryDataSignal(QString dataType);
    void sendCloseSignal(int ChartViewID);
    void getPreData(QList<QString> secodeList);
    void getPreCloseSpread();
    void startMonitorRealTimeData();

protected:
    virtual void closeEvent(QCloseEvent *event);

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

    int                  m_mainAveNumb;
    int                  m_subAveNumb;
    int                  m_energyAveNumb;
    double               m_mainCssRate1;
    double               m_mainCssRate2;
    double               m_energyCssRate1;
    double               m_energyCssRate2;

    double               m_css12Rate;
    double               m_maxCSS;
    double               m_minCSS;

    QString              m_strategyName;
    QMap<QString, int>   m_buyStrategyMap;
    QMap<QString, int>   m_saleStrategyMap;
    QMap<QString, int>   m_portfolioMap;
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
    QList<double>        m_hedgedData;
    QList<double>        m_votData;
    QList<MACD>          m_macdData;
    QList<double>        m_indexCodeData;

    QList<double>        m_mainList;
    QList<double>        m_subValueList;
    QList<double>        m_energyValueList;

    QTime                m_startTime;
    QTime                m_endTime;

private:
    QString                     m_title;
    bool                        m_isLayoutSetted;
    double                      m_timeAxisUpdatePercent;
    QList<double>               m_timeAxisUpdateData;
    int                         m_chartXaxisTickCount;

    QPoint                      m_mouseInitPos;
    double                      m_oldPointDistance;
    int                         m_currTimeIndex;
    int                         m_keyMoveCount;
    bool                        m_isKeyMove;

    QString                     m_mouseChartName;
    bool                        m_setMouseTimeIndex;

    QLineSeries*                m_strategySeries;
    QMyChartView*               m_strategyChartView;
    QChart*                     m_strategyChart;
    QLineSeries*                m_preSpreadSeries;

    QStackedBarSeries*          m_votBarSeries;
    QMyChartView*               m_votrunoverChartView;
    QChart*                     m_votrunoverChart;

    QLineSeries*                m_diffSeries;
    QLineSeries*                m_deaSeries;
    QStackedBarSeries*          m_macdSeries;
    QMyChartView*               m_macdChartView;
    QChart*                     m_macdChart;

    QLineSeries*                m_strategyLabelSeries;
    QLineSeries*                m_votLabelSeries;
    QLineSeries*                m_macdLabelSeries;
    QList<double>               m_strategyChartRange;
    QList<double>               m_votChartRange;
    QList<double>               m_macdChartRange;
    int                         m_oldLabelIndex;

    QTimer                      m_getPreCloseDataTimer;
    int                         m_getPreCloseDataUpdateTime;
    QTime                       m_getPreCloseDataTime;

//    QStringList                 m_extractKeyValueList;
//    QList<ExtractDataWindow*>   m_extractWindowList;
    bool                        m_isclosed;
};

#endif // CHARTFORM_H
