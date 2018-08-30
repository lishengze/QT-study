#ifndef FUTURECHART_H
#define FUTURECHART_H

#include <QWidget>
#include <QThread>
#include <QObject>

#include <QChart>
#include <QLineSeries>
#include <QCategoryAxis>
#include <QMessageBox>
#include <QTableView>

#include "basechart.h"
#include "toolfunc.h"
#include "qmychartview.h"
#include "monitorrealtimedata.h"


QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class FutureChart;
}

class FutureChart : public BaseChart
{
    Q_OBJECT

public:
    explicit FutureChart(int chartViewID, QString dbhost, QTableView* programInfoTableView,
                         QString futureName, double maxSpreadValue, double minSpreadValue,
                         QWidget *parent = 0);
    ~FutureChart();

    void initCommonData();
    void initMonitorWorker();
    void registSignalParamsType();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();
    void setLineColor();

    void transOriFutureData(QList<double> histFutureData);

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);

    virtual void updateAxis();
    virtual void updateSeries();
    virtual void updateMousePos();

    void updateLabelSeries(int index);
    void monitorSpread(double spread, QDateTime time);


    QList<QDateTime> getExtendedFutureTime(QList<QDateTime> oriTime,
                                           int chartXaxisTickCount, int updateTime);

    QCategoryAxis* getAxisX(QList<QDateTime> m_valueList, int tickCount);


    virtual void setPropertyValue(int index);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

public slots:
    void tradeOver_slot();
    void sendHistFutureData_slot(QList<double>);
    void sendFutureData_slot(QList<double>);

protected:
    virtual void closeEvent(QCloseEvent *event);

signals:
    void getHistFutureData_signal();

private:
    Ui::FutureChart*        ui;

    int                     m_chartViewID;
    QTableView*             m_programInfoTableView;
    QString                 m_futureName;
    double                  m_maxSpreadValue;
    double                  m_minSpreadValue;
    QString                 m_databaseName;
    QString                 m_dbhost;

    QList<double>           m_futureSpread;
    QList<QDateTime>        m_futureTime;
    QList<QDateTime>        m_extendedFutureTime;
    MonitorRealTimeData*    m_monitorWorker;
    QThread                 m_MonitorThread;
    QList<QMessageBox*>     m_monitorBoxList;
    int                     m_visibleBoxNumb;
    int                     m_allBoxNumb;

    QLineSeries*            m_spreadLineSeries;
    QMyChartView*           m_spreadChartView;
    QChart*                 m_spreadChart;

    QLineSeries*            m_labelSeries;
    QList<double>           m_labelRange;
    int                     m_oldLabelIndex;

    int                     m_updateTime;
    int                     m_chartXaxisTickCount;

    QPoint                  m_mouseInitPos;
    double                  m_oldPointDistance;
    int                     m_keyMoveCount;
    int                     m_currTimeIndex;
    bool                    m_isKeyMove;

};

#endif // FUTURECHART_H
