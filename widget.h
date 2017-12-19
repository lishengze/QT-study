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

#include "strategymodel.h"
#include "database.h"
#include "excel.h"

#include "realtimedataread.h"
#include "realtimedataprocess.h"

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

    ~Widget();

signals:
    void loginWind();
    void startWsq(QStringList secodeList);

public slots:
    void loginWindFailed(int errcode);
    void loginWindSucc();
    void startWsqFailed(int errcode);
    void startWsqSucc();

private slots:
    void on_historyData_clicked();

    void on_chooseStartDate_editingFinished();

    void on_tableView_clicked(const QModelIndex &index);

    void on_realDateTime_pushButton_clicked();

private:
    Ui::Widget *ui;
    QList<QWidget*> m_chartViews;

    StrategyModel* m_strategyModel;
    QTableView* m_strategyTalbeView;
    QString m_strategyFileDir;
    QString m_strategyName;
    QList<strategy_ceil> m_currStrategy;

    bool m_loginWind;
    QThread m_windWorkThread;
    RealTimeDataRead* m_readRealTimeData;
    QMap<QString, QStringList> m_realTimeData;

    Excel* m_excel;
};

#endif // WIDGET_H
