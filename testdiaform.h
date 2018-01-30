#ifndef TESTDIAFORM_H
#define TESTDIAFORM_H

#include <QWidget>

namespace Ui {
class TestDiaForm;
}

class TestDiaForm : public QWidget
{
    Q_OBJECT

public:
    explicit TestDiaForm(QWidget *parent = 0);
    ~TestDiaForm();

private:
    Ui::TestDiaForm *ui;
};

#endif // TESTDIAFORM_H
