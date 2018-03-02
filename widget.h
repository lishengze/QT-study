#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QtCharts/QChart"
#include <QChartView>
#include "callout.h"
#include <QGraphicsSimpleTextItem>
#include <QList>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QDialog>
#include <QTableView>
#include <QThread>
#include <QTimer>

#include "strategymodel.h"
#include "announcementform.h"
#include "database.h"
#include "excel.h"
#include "generateportfolioform.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    void initReadRealTimeData();
    void setCalendarValue();
    void setHedgeValue();
    void setStrategyTableView();
    void setBuySaleStrategyTableView();
    void setProgramInfoTableView();
    void setDataFrequency();
    void setMacdTime();

    void setTestRealTimeData();
    ~Widget();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);
    void cancelWsqRequest(int reqID);
    void cancelAllWsqRequest();

public slots:
    void receiveChartCLoseSignal(int chartViewID);

private slots:
    void on_chooseStartDate_editingFinished();

    void on_hedgeIndexStrategyTable_clicked(const QModelIndex &index);

    void on_buySaleStrategyTable_clicked(const QModelIndex &index);

    void on_Annoucnement_Button_clicked();

    void on_realTimeBuySaleChart_clicked();

    void on_historyHedgeIndexChart_clicked();

    void on_realTimeHedgeIndexChart_clicked();

    void on_historyBuySaleChart_clicked();

//    void on_genePortfolioButton_clicked();

private:
    Ui::Widget *ui;
    QList<QWidget*> m_chartViews;

    QTimer m_testRealTimer;
    StrategyModel* m_strategyModel;
    QTableView* m_strategyTalbeView;
    QString m_strategyFileDir;
    QString m_strategyName;
    QList<strategy_ceil> m_currStrategy;
    QMap<QString, int> m_strategyMap;

    StrategyModel* m_buySaleStrategyModel;
    QString m_buySaleStrategyFileDir;
    QMap<QString, int> m_buyStrategyMap;
    QMap<QString, int> m_saleStrategyMap;
    QString m_buySaleStrategyFileName;

    QMap<QString, int> m_seocdebuyCountMap;
    QStringList m_secodeNameList;
    QString m_hedgeIndexCode;
    int m_hedgeIndexCount;

    bool m_loginWind;
    QThread m_windWorkThread;
    QMap<QString, QStringList> m_realTimeData;

    AnnouncementForm* m_announcementView;
    GeneratePortfolioForm* m_genePortfolioWindow;

    Excel* m_excel;
};

#endif // WIDGET_H
