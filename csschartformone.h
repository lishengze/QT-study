﻿#ifndef CSSCHARTFORMONE_H
#define CSSCHARTFORMONE_H

#include <QWidget>
#include <QLineSeries>
#include <QChart>
#include <QCategoryAxis>

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

    explicit CSSChartFormOne(QString dbhost, QString databaseName,
                            QString startDate, QString endDate, QString codeName,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent = 0);
    ~CSSChartFormOne();

    void initHistoryData();
    void registSignalParamsType();

    void startGetData();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();

    virtual void initExtractKeyValueList();
    virtual QList<QMyChartView*> getChartViewList();
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);

    virtual void updateAxis();
    virtual void updateSeries();
    virtual void updateMousePos();

    virtual void setPropertyValue(int index);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event);
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();
signals:
    void getCSSData_signal();

public slots:
    void sendCSSData_slot(QList<QString>, QList<QList<double>>,  QList<QList<double>>);

protected:
    virtual bool eventFilter (QObject *watched, QEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    Ui::CSSChartFormOne *              ui;

    QString                            m_dbhost;
    QString                            m_databaseName;
    QString                            m_startDate;
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

    QList<QList<double>>               m_aveList;
    QList<QList<double>>               m_cssList;
    QList<QString>                     m_timeList;

    QList<QLineSeries*>                m_aveLineSeries;
    QList<QLineSeries*>                m_cssLineSeries;
    QMyChartView*                      m_aveChartView;
    QChart*                            m_aveChart;
    QMyChartView*                      m_cssChartView;
    QChart*                            m_cssChart;

    HistoryData*                       m_histdataWorker;
    QThread                            m_histdataThread;
};

#endif // CSSCHARTFORMONE_H
