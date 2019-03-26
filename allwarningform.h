#ifndef ALLWARNINGFORM_H
#define ALLWARNINGFORM_H

#include <QWidget>

namespace Ui {
class AllWarningForm;
}

class AllWarningForm : public QWidget
{
    Q_OBJECT

public:
    explicit AllWarningForm(QWidget *parent = 0);
    ~AllWarningForm();

private:
    Ui::AllWarningForm *ui;
};

#endif // ALLWARNINGFORM_H
