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
#include <QGroupBox>
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
    explicit CSSChartForm(int chartID, QString dbhost, QStringList timeTypeList,
                            QString startDate, QString endDate,
                            QString selectCodeName, QString hedgedCodeName,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent = 0);

    explicit CSSChartForm(int chartID, QString dbhost, QStringList timeTypeList,
                            QString startDate, QString endDate,
                            QMap<QString, int> portfolioMap,  QString portfolioName,
                            QString hedgeIndexCode, int hedgeIndexCount,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent = 0);

    ~CSSChartForm();

    void initCommonData();
    void initSecodeList();
    void initListData();
    void initHistoryData();
    void initLabelRowColNumb();
    void registSignalParamsType();
    void setRealtimeFlag();
    void startGetHistData();
    void startGetRealtimeData();

    void initColors();

    void extendRealtimeList(int dataID);

    virtual void initLayout();
    virtual void initTheme();

    void setChart(int dataID);
    void setAVEChart(int dataID);
    void setCSSChart(int dataID);
    void setChartView();
    void setLabels();
    void setColors();
    void setWindowTitleName();
    void updateLabelSeries(int index, int dataID);

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);

    void setPropertyValue(int index, int dataID, bool isUpdateLableLine=true);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

signals:
    void getRealtimeData_signal();
    void getIndexCssData_signal();

public slots:
    void sendHistCSSData_slot(QList<QString>, QList<QList<double>>,
                          QList<QList<double>>, int);

    void sendRealTimeCSSData_slot(QList<double>, QList<double>, int, bool);
    void connectMarkers();
    void handleMarkerClicked();
    void getChoosenInfo_slot(QStringList keyValueList, QString fileDir, bool bOpenDesFile);

protected:

private:
    Ui::CSSChartForm *                      ui;

    QString                                 m_dbhost;
    QStringList                             m_timeTypeList;
    QStringList                             m_databaseNameList;
    QString                                 m_startDate;
    QString                                 m_endDate;
    int                                     m_chartXaxisTickCount;
    int                                     m_maxColNumb;
    int                                     m_currColNumb;

    bool                                    m_isPortfolio;
    bool                                    m_isRealtime;
    QMap<QString, int>                      m_portfolioMap;
    QString                                 m_portfolioName;
    QString                                 m_hedgeIndexCode;
    int                                     m_hedgeIndexCount;
    QString                                 m_secodeStyle;
    QList<QString>                          m_secodeNameList;

    QString                                 m_selectCodeName;
    QString                                 m_hedgedCodeName;

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
    int                                     m_currChartIndex;

    QList<QList<QList<double>>>             m_aveList;
    QList<QList<QList<double>>>             m_cssList;
    QList<QList<QString>>                   m_timeList;
    QList<QList<double>>                    m_indexDataList;
    QList<QList<double>>                    m_hedgedDataList;

    QList<QList<QList<double>>>             m_aveListStore;
    QList<QList<QList<double>>>             m_cssListStore;
    QList<QList<QString>>                   m_timeListStore;
    QList<QList<double>>                    m_indexDataListStore;
    QList<QList<double>>                    m_hedgedDataListStore;
    QString                                 m_realStartTime;
    QString                                 m_realEndTime;
    QString                                 m_titleName;

    QList<int>                              m_currDataIDList;

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
    QList<QList<QGroupBox*>>                m_groupBoxListList;
    QList<QList<QLabel*>>                   m_aveChartlabelListList;
    QList<QList<QLabel*>>                   m_cssChartlabelListList;

    QList<QLineSeries*>                     m_aveChartLabelSeriesList;
    QList<QLineSeries*>                     m_cssChartLabelSeriesList;
    QList<int>                              m_oldLabelIndexList;
    QList<QList<double>>                    m_aveChartRange;

    QList<QColor>                           m_aveChartColorList;
    QList<QColor>                           m_cssChartColorList;

    QList<HistoryData*>                     m_histdataWorkerList;
    QList<QThread*>                         m_histdataThreadList;
    int                                     m_labelRowNumb;
    int                                     m_labelColNumb;

    QMutex                                  m_mutex;

    QList<int>                              m_keyMoveCountList;
    QList<int>                              m_currTimeIndexList;
    QList<bool>                             m_isKeyMoveList;
    QList<QPoint>                           m_mouseInitPosList;
    int                                     m_currFoucusChartID;

    // 用于更新实时图像的数据结构
    QList<int>                              m_updateCountList;
};

#endif // CSSCHARTFORM_H
