#ifndef CHOOSESECODEWINDOW_H
#define CHOOSESECODEWINDOW_H

#include <QMainWindow>

namespace Ui {
class ChooseSecodeWindow;
}

class ChooseSecodeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChooseSecodeWindow(QWidget *parent = 0);
    ~ChooseSecodeWindow();

private:
    Ui::ChooseSecodeWindow *ui;
};

#endif // CHOOSESECODEWINDOW_H
