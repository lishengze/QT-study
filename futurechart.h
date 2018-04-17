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

class FutureChart : public QWidget
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

    void initLayout();
    void initTheme();
    void initSpreadChartView();

    void transOriFutureData(QList<double> histFutureData);

    void updateAxis();
    void updateSeries();
    void updateMousePos();
    void setRealTimeLabel(int index);

    void monitorSpread(double spread, QDateTime time);

    QList<QDateTime> getExtendedFutureTime(QList<QDateTime> oriTime,
                                           int chartXaxisTickCount, int updateTime);

    QCategoryAxis* getAxisX(QList<QDateTime> m_valueList, int tickCount);


    void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    void KeyReleaseFunc(QEvent *event);
    void moveMouse(int step);
    double getPointXDistance();

public slots:
    void tradeOver_slot();
    void sendHistFutureData_slot(QList<double>);
    void sendFutureData_slot(QList<double>);

protected:
    bool eventFilter (QObject *watched, QEvent *event);
    void closeEvent(QCloseEvent *event);

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

    int                     m_updateTime;
    int                     m_chartXaxisTickCount;

    QPoint                  m_mouseInitPos;
    double                  m_oldPointDistance;
    int                     m_keyMoveCount;
    int                     m_currTimeIndex;
    bool                    m_isKeyMove;

};

#endif // FUTURECHART_H
