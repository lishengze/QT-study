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
#include "datastruct.h"

#include "basechart.h"
#include "qmychartview.h"
#include "historydata.h"
#include "addwarningform.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class CSSChartForm;
}

class CSSChartForm : public BaseChart
{
    Q_OBJECT

  public:

    CSSChartForm(int windowID, DatabaseParam dbParam, HedgedParam hedgedParam,  
                 CSSParam cssParam,  QWidget *parent = nullptr);

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

    void releaseHistWorker();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initAddWarningView();
    virtual AddWarningForm* getAddWarningForm();

    void setChart(int dataID);
    void setAVEChart(int dataID);
    void setCSSChart(int dataID);
    void setChartView();
    void setLabels();
    void setColors();
    void setWindowTitleName();
    void updateChart(QList<double> &aveList, QList<double> &cssList,
                     int dataID);

    void updateLabelSeries(int index, int dataID);

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView *> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);

    void setPropertyValue(int index, int dataID, bool isUpdateLableLine = true);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

  signals:
    void updateProgramInfo_signal(QString info, bool isWarning);
    void windowClose_signal(int windowID, QString windowName);

    void getHistPortfolioData_signal();
    void getHistIndexData_signal();
    void getRealtimeData_signal();

    void processRealtimeDataComplete_signal();

  public slots:
    void sendHistCSSData_slot(QList<QString>, QList<QList<double>>,
                              QList<QList<double>>, int);

    void sendRealTimeCSSData_slot(QList<double>, QList<double>, int, bool);

    void connectMarkers();

    void handleMarkerClicked();

    void getChoosenInfo_slot(QStringList keyValueList, QString fileDir, bool bOpenDesFile);

protected:
    virtual void closeEvent(QCloseEvent *event);

  private:
    Ui::CSSChartForm *                       ui;
    int                                      m_windowID;
    bool                                     m_isClosed;

    DatabaseParam                            m_dbParam;    
    HedgedParam                              m_hedgedParam;
    CSSParam                                 m_cssParam;

    QStringList                              m_databaseNameList;
    int                                      m_chartXaxisTickCount;
    int                                      m_maxColNumb;
    int                                      m_currColNumb;

    QString                                  m_secodeStyle;

    QString                                  m_realStartTime;
    QString                                  m_realEndTime;
    QString                                  m_titleName;
    QString                                  m_excelFileName;

    AddWarningForm*                          m_pAddWarningForm;
    // Chart;
    QList<double>                            m_cssMarkValueList;
    int                                      m_currChartIndex;

    QList<QList<QList<double>>>              m_aveList;
    QList<QList<QList<double>>>              m_cssList;
    QList<QList<QString>>                    m_timeList;
    QList<QList<double>>                     m_indexDataList;
    QList<QList<double>>                     m_hedgedDataList;

    QList<QList<QList<double>>>              m_aveListStore;
    QList<QList<QList<double>>>              m_cssListStore;
    QList<QList<QString>>                    m_timeListStore;
    QList<QList<double>>                     m_indexDataListStore;
    QList<QList<double>>                     m_hedgedDataListStore;



    QList<int>                               m_currDataIDList;
    QList<QGridLayout *>                     m_gridLayoutList;

    QList<QList<QLineSeries *>>              m_aveLineSeriesList;
    QList<QList<QLineSeries *>>              m_cssLineSeriesList;
    QList<QList<QLineSeries *>>              m_cssMarkLineSeriesList;

    QList<QLineSeries *>                     m_energyUpLineSeriesList;
    QList<QLineSeries *>                     m_energyDownLineSeriesList;

    QList<QMyChartView *>                    m_aveChartViewList;
    QList<QChart *>                          m_aveChartList;
    QList<QMyChartView *>                    m_cssChartViewList;
    QList<QChart *>                          m_cssChartList;
    QList<QList<QGroupBox *>>                m_groupBoxListList;
    QList<QList<QLabel *>>                   m_aveChartlabelListList;
    QList<QList<QLabel *>>                   m_cssChartlabelListList;

    QList<QLineSeries *>                     m_aveChartLabelSeriesList;
    QList<QLineSeries *>                     m_cssChartLabelSeriesList;
    QList<int>                               m_oldLabelIndexList;
    QList<QList<double>>                     m_aveChartRange;

    QList<QColor>                            m_aveChartColorList;
    QList<QColor>                            m_cssChartColorList;

    QList<HistoryData *>                     m_histdataWorkerList;
    QList<QThread *>                         m_histdataThreadList;
    int                                      m_labelRowNumb;
    int                                      m_labelColNumb;

    QMutex                                   m_mutex;

    QList<int>                               m_keyMoveCountList;
    QList<int>                               m_currTimeIndexList;
    QList<bool>                              m_isKeyMoveList;
    QList<QPoint>                            m_mouseInitPosList;
    int                                      m_currFoucusChartID;

    // 用于更新实时图像的数据结构
    QList<int>                               m_updateCountList;

    QList<QDateTime>                         m_initTimeList;
    QList<QDateTime>                         m_getHistDataTimeList;
    QList<QDateTime>                         m_getRealDataTimeList;
    QList<QDateTime>                         m_showRealDataTimeList;
};

#endif // CSSCHARTFORM_H
