#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QDialog>
#include <QChartView>
#include <QChart>
#include "callout.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class chartDialog;
}

class chartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit chartDialog(QWidget *parent = 0);
    chartDialog(QWidget *parent, QChartView* charView);
    ~chartDialog();
    void setChartView();

private slots:
    void on_changeTheme_clicked();

public slots:
    void keepCallout();
    void tooltip(QPointF point, bool state);

private:
    Ui::chartDialog *ui;
    QChartView* m_chartView;
    QChart* m_chart;
    Callout* m_tooltip;
    QList<Callout*> m_callouts;
};

#endif // CHARTDIALOG_H
