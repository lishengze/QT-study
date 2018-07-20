#ifndef INDEXCHART_H
#define INDEXCHART_H

#include <QMainWindow>

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

class IndexChart : public QMainWindow
{
    Q_OBJECT

public:
    explicit IndexChart(QWidget *parent = 0);
    IndexChart(int chartViewID, QString dbhost, QString databaseName,
                QString selectIndex, QString hedgedIndex,
                QString startDate, QString endDate,
                int aveNumb, double css12Rate,
                double cssRate1, double cssRate2,
                double m_maxCSS, double m_minCSS,
                QWidget *parent = 0);

    ~IndexChart();

    void initHistDataWorker();
    void initMonitorWorker();
    void registSignalParamsType();

public slots:
    void sendHistIndexData_slot(QList<QStringList>);
    void sendHistIndexError_slot(QString);
    void sendRealTimeIndexData_slot(QString time, double earningValue);

signals:
     void getIndexHistData_signal();

private:
    Ui::IndexChart *ui;

    int                                m_chartViewID;
    QString                            m_dbhost;
    QString                            m_databaseName;
    QString                            m_startDate;
    QString                            m_endDate;

    QString                            m_selectIndex;
    QString                            m_hedgedIndex;
    int                                m_aveNumb;
    double                             m_css12Rate;
    double                             m_cssRate1;
    double                             m_cssRate2;
    double                             m_maxCSS;
    double                             m_minCSS;

    HistoryData*                       m_histDataWorker;
    QThread                            m_histDataThread;
    MonitorRealTimeData*               m_monitorWorker;
    QThread                            m_MonitorThread;
};

#endif // INDEXCHART_H
