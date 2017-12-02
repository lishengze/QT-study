#ifndef TESTFORM_H
#define TESTFORM_H

#include <QWidget>
#include "qmychartview.h"
#include <QChart>
#include <QEvent>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class TestForm;
}

class TestForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestForm(QWidget *parent = 0);
    ~TestForm();

protected:
    void mouseMoveEvent (QMouseEvent *event);
    bool eventFilter (QObject *watched, QEvent *event);

private:
    Ui::TestForm *ui;
    QChart* m_chart;
    QMyChartView* m_chartView;
};

#endif // TESTFORM_H
