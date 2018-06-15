#include "choosesecodewindow.h"
#include "ui_choosesecodewindow.h"

ChooseSecodeWindow::ChooseSecodeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChooseSecodeWindow)
{
    ui->setupUi(this);
}

ChooseSecodeWindow::~ChooseSecodeWindow()
{
    delete ui;
}
