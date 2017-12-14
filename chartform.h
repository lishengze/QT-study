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

#include <QThread>
#include <QObject>
#include <QMutexLocker>
#include <QMutex>

#include <QTableView>
#include "dataread.h"
#include "dataprocess.h"

#include "strategy.h"
#include "callout.h"
#include "database.h"
#include "macd.h"
#include "qmychartview.h"

namespace Ui {
class ChartForm;
}

class ChartForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartForm(QWidget *parent = 0);
    ~ChartForm();
    ChartForm(QWidget *parent, QTableView* programInfoTableView, int charViewID,
              QString startDate, QString endDate, QString timeType,
              QList<strategy_ceil> strategy, QString strategyName,
              QString hedgeIndexCode, int hedgeIndexCount,
              QList<int> macdTime, int threadNumb = 8,
              QString databaseName="MarketData");

    void registSignalParamsType();
    void initData(QString databaseName, QString timeType,
                  QList<strategy_ceil> strategyList);

    QList<QStringList> allocateThreadData();

    void startReadData();
    void startProcessData();

    void releaseDataReaderSrc();
    void releaseDataProcessSrc();

    void setLayout ();
    void setVotRunoverChartView();
    void setStrategyChartView();
    void setMACDChartView();

    QCategoryAxis* getAxisX ();
    void setTheme();

    void setTestView();
    void setMouseMoveValue(int currIndex);

    QList<double> getChartYvalueRange(QList<QPointF> pointList );
    QList<double> getChartYvalueRange(QList<double> yValueList );

public slots:
    void receiveOriginalData(QMap<QString, QList<QStringList>> subThreadData);
    void receiveAllProcessedData(QList<QList<double>> allData);

signals:
    void sendStartReadDataSignal(QString dataType);
    void sendStartProcessDataSignal(QString dataType);

protected:
    bool eventFilter (QObject *watched, QEvent *event);

private:
    Ui::ChartForm *ui;


private:
    int m_chartViewID;
    QTableView* m_programInfoTableView;
    QList<strategy_ceil> m_strategy;
    QString m_strategyName;
    QString m_hedgeIndexCode;
    int m_hedgeIndexCount;

    QList<int> m_macdTime;
    QString m_startDate;
    QString m_endDate;
    QStringList m_keyValueList;
    QString m_timeType;
    QString m_timeTypeFormat;
    QString m_databaseName;
    QString m_dbhost;

    int m_threadNumb;

private:
    mutable QMutex m_mutex;

    QMap<QString, int> m_seocdebuyCountMap;
    QStringList m_secodeNameList;

    QList<DataProcess*> m_dataProcessList;
    QList<QThread*> m_dataProcessThreadList;

    QList<DataRead*> m_dataReaderList;
    QList<QThread*> m_dataReaderThreadList;
    int m_readDataThreadCount;

    QMap<QString, QList<QStringList>> m_completeTableData;

    QList<double> m_timeData;
    QList<double> m_strategyData;
    QList<double> m_votData;
    QList<MACD> m_macdData;

    int m_dataNumb;
    QString m_title;
    int m_chartXaxisTickCount;

    QMyChartView* m_strategyChartView;
    QChart* m_strategyChart;
    Callout *m_strategyTooltip;

    QMyChartView* m_votrunoverChartView;
    QChart* m_votrunoverChart;
    Callout *m_votrunoverTooltip;

    QMyChartView* m_macdChartView;
    QChart* m_macdChart;
    Callout *m_macdTooltip;

    QChartView* m_testChartView;
    QChart* m_testChart;
    Callout *m_testTooltip;

};

//    QList<QPointF> m_strategyData;
//    QList<QPointF> m_strategyHedgeData;
//    QList<QPointF> m_votrunoverData;

//    QList<QPointF> computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList);
//    void setStrategyData();
//    void setVotRunoverData();
//    void setMacdData();

#endif // CHARTFORM_H
