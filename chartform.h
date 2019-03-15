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
#include "datastruct.h"

#include "datastruct.h"
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

    // ChartForm(QWidget *parent, QTableView* programInfoTableView,
    //           int chartViewID, QString dbhost, bool isBuySalePortfolio,
    //           QString hedgeIndexCode, int hedgeIndexCount, QList<int> macdTime,
    //           QMap<QString, double> strategyMap = EmpytQStringDoubleMap(), QString strategyName="",
    //           QMap<QString, double> buyStrategyList = EmpytQStringDoubleMap(),
    //           QMap<QString, double> saleStrategyList = EmpytQStringDoubleMap(),
    //           bool isRealTime = true, QString startDate ="", QString endDate="", QString timeType ="");

    ChartForm(int windowID, DatabaseParam dbParam, HedgedParam hedgedParam, QWidget *parent = nullptr);

    ~ChartForm();

    void registSignalParamsType();
    void initCommonData();
    void initHistoryData();
    void initRealTimeData();

    void initHistdataThread();

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);
    void writeExcelData();

    void releaseHistWorker();

    void updateData();
    void updateChart();
    virtual void updateAxis();
    virtual void updateSeries();
    virtual void updateMousePos();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();

    void setWindowName();
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
    void sendPreCloseData_slot(double);
    void sendHistPortfolioData_slot(QList<QList<double>> allData);
    void sendRealtimeSpreadMACDData_slot(ChartData curChartData);

    void getPreCloseData_timer_slot();
    void receiveTableViewInfo_slot(QString msg);

signals:    
    void updateProgramInfo_signal(QString info, bool isWarning);
    void windowClose_signal(int windowID, QString windowName);

    void getPreCloseData_signal();
    void getHistPortfolioData_Signal();
    void getRealtimeData_signal();

    void processRealtimeDataComplete_signal();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::ChartForm *ui;

private:
    bool                 m_isTest;

    int                  m_windowID;
    HedgedParam          m_hedgedParam;
    DatabaseParam        m_dbParam;

    QString              m_databaseName;
    QString              m_dbhost;

    QString              m_startDate;
    QString              m_endDate;
    QString              m_weightDate;

    QMap<QString, double>   m_oriPortfolio;    
    QMap<QString, double>   m_hedgedPortfolio;

    QString              m_hedgedCode;

    QString              m_portfolioName;    
    int                  m_hedgedType;

    bool                 m_isRealTime;
    bool                 m_isPortfolioHedge;

    QList<int>           m_macdTime;
    QStringList          m_codeList;

    QString              m_timeType;

    QTableView*          m_programInfoTableView;
    QStringList          m_keyValueList;
    QString              m_secodeStyle;
    QString              m_timeTypeFormat;
    int                  m_threadNumb;
    int                  m_updateTime;

    QThread              m_MonitorThread;
    HistoryData*         m_histdataWorker;
    QThread              m_histdataThread;
    double               m_preSpread;

    QList<double>        m_timeData;
    QList<double>        m_earningList;
    QList<double>        m_votList;
    QList<MACD>          m_MACDList;
    QList<double>        m_indexCodeData;

    QTime                m_startTime;
    QTime                m_endTime;

private:
    QString                     m_windowName;
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

    QLineSeries*                m_votLineSeries;
    QMyChartView*               m_votrunoverChartView;
    QChart*                     m_votrunoverChart;

    QLineSeries*                m_diffSeries;
    QLineSeries*                m_deaSeries;
    QLineSeries*                m_macdUpLineSeries;
    QLineSeries*                m_macdDownLineSeries;
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

    bool                        m_isclosed;

    QDateTime                   m_initTime;
    QDateTime                   m_getHistDataTime;
    QDateTime                   m_getRealDataTime;
    QDateTime                   m_showRealDataTime;
    
    bool                        m_isBarSet;
    bool                        m_isSingleLine;
};

#endif // CHARTFORM_H
