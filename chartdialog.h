#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QDialog>
#include <QChartView>
#include <QChart>
#include <QVector>
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
    ~chartDialog();

    void setTestChartView();
    void setTimeChartView();
    void setTimeChartViewCom();

private slots:
    void on_changeTheme_clicked();

private:
    Ui::chartDialog *ui;
    QChartView* m_testChartView;
    QChart* m_testChart;
    QChartView* m_timeChartView;
    QChart* m_timeChart;
};

#endif // CHARTDIALOG_H
