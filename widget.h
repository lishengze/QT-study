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
    void addTestRealTimeData();
    void receiveChartCLoseSignal(int chartViewID);

private slots:
    void on_historyData_clicked();

    void on_chooseStartDate_editingFinished();

    void on_tableView_clicked(const QModelIndex &index);

    void on_realDateTime_pushButton_clicked();

    void on_Annoucnement_Button_clicked();

private:
    Ui::Widget *ui;
    QList<QWidget*> m_chartViews;

    QTimer m_testRealTimer;
    StrategyModel* m_strategyModel;
    QTableView* m_strategyTalbeView;
    QString m_strategyFileDir;
    QString m_strategyName;
    QList<strategy_ceil> m_currStrategy;

    QMap<QString, int> m_seocdebuyCountMap;
    QStringList m_secodeNameList;
    QString m_hedgeIndexCode;
    int m_hedgeIndexCount;

    bool m_loginWind;
    QThread m_windWorkThread;
    QMap<QString, QStringList> m_realTimeData;

    AnnouncementForm* m_announcementView;

    Excel* m_excel;
    int m_databaseReadThreadNumb;
    bool m_bTestRealTime;
};

#endif // WIDGET_H
