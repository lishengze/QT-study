#ifndef CSSCHARTFORMONE_H
#define CSSCHARTFORMONE_H

#include <QWidget>
#include <QLineSeries>
#include <QChart>
#include <QCategoryAxis>
#include <QLabel>
#include <QStackedBarSeries>
#include <QBarCategoryAxis>

#include "basechart.h"
#include "qmychartview.h"
#include "historydata.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class CSSChartFormOne;
}

class CSSChartFormOne :  public BaseChart
{
    Q_OBJECT

public:
//    explicit CSSChartFormOne(QWidget *parent = 0);

    explicit CSSChartFormOne(int chartID, QString dbhost, QString timeType,
                            QString startDate, QString endDate, QString codeName,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent = 0);
    ~CSSChartFormOne();

    void initCommonData();
    void initHistoryData();
    void registSignalParamsType();
    void startGetData();
    void addPropertyLabel();

    void setLineColor();

    QCategoryAxis* getCCSAxisY ();
    QBarCategoryAxis* getQBarCategoryAxisAxisY();

    void updateLableSeries(int index);

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);

    virtual void setPropertyValue(int index);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

signals:
    void getCSSData_signal();


public slots:
    void sendCSSData_slot(QList<QString>, QList<QList<double>>,  QList<QList<double>>, int);

    void connectMarkers();
    void disconnectMarkers();
    void handleMarkerClicked();

protected:

private:
    Ui::CSSChartFormOne *              ui;

    QString                            m_dbhost;
    QString                            m_databaseName;
    QString                            m_timeType;
    QString                            m_startDate;
    QString                            m_newStartDate;
    QString                            m_endDate;
    int                                m_chartXaxisTickCount;

    QString                            m_singleCodeName;
    int                                m_mainAveNumb;
    int                                m_subAveNumb;
    int                                m_energyAveNumb;
    double                             m_css12Rate;
    double                             m_mainCssRate1;
    double                             m_mainCssRate2;
    double                             m_energyCssRate1;
    double                             m_energyCssRate2;
    double                             m_maxCSS;
    double                             m_minCSS;
    QList<int>                         m_aveNumbList;
    QList<bool>                        m_isEMAList;
    QList<double>                      m_cssMarkValueList;

    QList<QList<double>>               m_aveList;
    QList<QList<double>>               m_cssList;
    QList<QString>                     m_timeList;

    QList<QLineSeries*>                m_aveLineSeries;
    QList<QLineSeries*>                m_cssLineSeries;
    QList<QLineSeries*>                m_cssMarkLineSeries;
    QList<QStackedBarSeries*>          m_energySeriesList;

    QMyChartView*                      m_aveChartView;
    QChart*                            m_aveChart;
    QMyChartView*                      m_cssChartView;
    QChart*                            m_cssChart;
    QList<QLabel*>                     m_labelList;

    QStackedBarSeries*                 m_aveChartLabelSeries;
    QStackedBarSeries*                 m_cssChartLabelSeries;

//    QLineSeries*                       m_aveChartLabelSeries;
//    QLineSeries*                       m_cssChartLabelSeries;
    QList<QBarSet*>                    m_LabelSetList;

    int                                m_oldLabelIndex;
    double                             m_aveChartMaxValue;
    double                             m_aveChartMinValue;

    HistoryData*                       m_histdataWorker;
    QThread                            m_histdataThread;

    QPoint                             m_mouseInitPos;
    double                             m_oldPointDistance;
    int                                m_currTimeIndex;
    int                                m_keyMoveCount;
    bool                               m_isKeyMove;
};

#endif // CSSCHARTFORMONE_H
