#ifndef CSSCHARTFORM_H
#define CSSCHARTFORM_H

#include <QWidget>
#include <QLineSeries>
#include <QChart>
#include <QCategoryAxis>
#include <QLabel>
#include <QStackedBarSeries>
#include <QBarCategoryAxis>
#include <QGridLayout>
#include <QMutex>

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
class CSSChartForm;
}

class CSSChartForm : public BaseChart
{
    Q_OBJECT

public:
//    explicit CSSChartForm(QWidget *parent = 0);
    explicit CSSChartForm(int chartID, QString dbhost, QStringList timeTypeList,
                            QString startDate, QString endDate, QString codeName,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent = 0);
    ~CSSChartForm();

    void initCommonData();
    void initListData();
    void initHistoryData();
    void registSignalParamsType();
//    void startGetData();
//    void addPropertyLabel();

    virtual void initLayout();
//    virtual void initTheme();
//    virtual void initChartView();

    void setChartView(int dataID);

//    virtual void initExtractKeyValueList();
//    virtual QList<QMyChartView*> getChartViewList();
//    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
//    virtual QList<QStringList> getExcelData(QStringList keyValueList);

//    virtual void setPropertyValue(int index);
//    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
//    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
//    virtual void KeyReleaseFunc(QEvent *event);
//    virtual void moveMouse(int step);
//    virtual double getPointXDistance();

signals:
    void getCSSData_signal();

public slots:
    void sendCSSData_slot(QList<QString>, QList<QList<double>>,
                          QList<QList<double>>, int);


private:
    Ui::CSSChartForm *                      ui;

    QString                                 m_dbhost;
    QStringList                             m_timeTypeList;
    QStringList                             m_databaseNameList;
    QString                                 m_startDate;
    QString                                 m_endDate;
    int                                     m_chartXaxisTickCount;
    QList<int>                              m_dataIDList;

    QString                                 m_singleCodeName;
    int                                     m_mainAveNumb;
    int                                     m_subAveNumb;
    int                                     m_energyAveNumb;
    double                                  m_css12Rate;
    double                                  m_mainCssRate1;
    double                                  m_mainCssRate2;
    double                                  m_energyCssRate1;
    double                                  m_energyCssRate2;
    double                                  m_maxCSS;
    double                                  m_minCSS;
    QList<int>                              m_aveNumbList;
    QList<bool>                             m_isEMAList;
    QList<double>                           m_cssMarkValueList;

    QList<QList<QList<double>>>             m_aveList;
    QList<QList<QList<double>>>             m_cssList;
    QList<QList<QString>>                   m_timeList;

    QList<QGridLayout*>                     m_gridLayoutList;
    QGridLayout*                            m_mainGridLayout;

    QList<QList<QLineSeries*>>              m_aveLineSeriesList;
    QList<QList<QLineSeries*>>              m_cssLineSeriesList;
    QList<QList<QLineSeries*>>              m_cssMarkLineSeriesList;
    QList<QList<QStackedBarSeries*>>        m_energySeriesListList;
    QList<QMyChartView*>                    m_aveChartViewList;
    QList<QChart*>                          m_aveChartList;
    QList<QMyChartView*>                    m_cssChartViewList;
    QList<QChart*>                          m_cssChartList;

    QList<HistoryData*>                     m_histdataWorkerList;
    QList<QThread*>                         m_histdataThreadList;

    QMutex                                  m_mutex;

};

#endif // CSSCHARTFORM_H
