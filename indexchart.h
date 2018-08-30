#ifndef INDEXCHART_H
#define INDEXCHART_H

#include <QWidget>
#include <QLineSeries>
#include <QChart>
#include <QCategoryAxis>

#include "basechart.h"
#include "qmychartview.h"
#include "historydata.h"
#include "monitorrealtimedata.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class IndexChart;
}

class IndexChart : public BaseChart
{
    Q_OBJECT

public:
//    explicit IndexChart(QWidget *parent = 0);
    IndexChart(int chartViewID, QString dbhost, QString databaseName,
                QString selectIndex, QString hedgedIndex,
                QString startDate, QString endDate,
                int aveNumb, double css12Rate=0.014,
                double cssRate1=0.3333, double cssRate2=0.6667,
                double m_maxCSS=300, double m_minCSS=-300,
                QWidget *parent = 0);

    ~IndexChart();
    void initCommonData();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();

    void setLineColor();
    void setRealtimeFlag();
    void setDataFrequency();
    void initHistDataWorker();
    void initMonitorWorker();
    void registSignalParamsType();

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);

    void updateLabelSeries(int index);

    void updateNextAddedTime();
    void updateNewValue(QStringList selectData, QStringList hedgedData);
    bool isAddedTime(int time);
    void computeNewValue(QStringList selectData, QStringList hedgedData);

    QCategoryAxis* getTimeAxisX (QList<QString> timeList, int tickCount);

    QList<QString> getExtendedFutureTime(QList<QString> oriTime,
                                         int chartXaxisTickCount,
                                         int updateTime);
    virtual void updateAxis();
    virtual void updateSeries();
    virtual void updateMousePos();
    virtual void setPropertyValue(int index);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

public slots:
    void sendLatestHistIndexData_slot(QString, double, double, QList<int>);
    void sendHistIndexData_slot(QList<QString>, QList<double>, QList<double>,
                                QList<double>, QList<double>, QList<double>);
    void sendHistIndexError_slot(QString);
    void sendRealTimeIndexData_slot(QStringList, QStringList);
    void sendRealTimeIndexDataError_slot(QString);

    void tradeOver_slot();

protected:
    virtual void closeEvent(QCloseEvent *event);

signals:
     void getIndexHistData_signal();
     void getIndexRealtimeData_signal();

private:
    Ui::IndexChart *ui;

    int                                m_chartViewID;
    QString                            m_dbhost;
    QString                            m_databaseName;
    QString                            m_startDate;
    QString                            m_endDate;

    QLineSeries*                       m_earningLineSeries;
    QLineSeries*                       m_CSSLineSeries;
    QMyChartView*                      m_ChartView;
    QChart*                            m_Chart;

    QString                            m_selectIndex;
    QString                            m_hedgedIndex;
    int                                m_aveNumb;
    double                             m_csst12Rate;
    double                             m_csstRate1;
    double                             m_csstRate2;
    double                             m_maxCSS;
    double                             m_minCSS;
    bool                               m_isRealTime;


    QList<QString>                     m_extendedTimeList;
    QList<QString>                     m_timeList;
    QList<double>                      m_earningList;
    QList<double>                      m_typeList;
    QList<double>                      m_CSSTList;
    QList<double>                      m_preCSSList;
    QList<double>                      m_CSSList;

    QList<int>                         m_standardTimeList;
    double                             m_latestSelectIndexPrice;
    double                             m_latestHedgedIndexPrice;
    int                                m_nextAddedTime;
    int                                m_latestTime;
    bool                               m_isAddedTime;
    double                             m_newEarningValue;
    double                             m_newCSSValue;
    QString                            m_newTime;
    QString                            m_timeType;

    int                                m_updateTime;
    int                                m_timeFrequency;
    int                                m_chartXaxisTickCount;
    QPoint                             m_mouseInitPos;
    double                             m_oldPointDistance;
    int                                m_keyMoveCount;
    int                                m_currTimeIndex;

    bool                               m_isKeyMove;
    HistoryData*                       m_histDataWorker;
    QThread                            m_histDataThread;
    MonitorRealTimeData*               m_monitorWorker;
    QThread                            m_MonitorThread;

    double                             m_testCSSValue;

    QLineSeries*                       m_labelSeries;
    QList<double>                      m_labelRange;
};

#endif // INDEXCHART_H
