#ifndef CHARTTABLEFORM_H
#define CHARTTABLEFORM_H

#include <QWidget>

namespace Ui {
class ChartTableForm;
}

class ChartTableForm : public QWidget
{
    Q_OBJECT

public:
    explicit ChartTableForm(QWidget *parent = 0);
    ~ChartTableForm();

private:
    Ui::ChartTableForm *ui;
};

#endif // CHARTTABLEFORM_H
