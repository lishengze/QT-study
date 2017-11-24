#ifndef CHARTFORM_H
#define CHARTFORM_H

#include <QWidget>

namespace Ui {
class ChartForm;
}

class ChartForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartForm(QWidget *parent = 0);
    ~ChartForm();

private:
    Ui::ChartForm *ui;
};

#endif // CHARTFORM_H
