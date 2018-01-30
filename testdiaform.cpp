#include "testdiaform.h"
#include "ui_testdiaform.h"

TestDiaForm::TestDiaForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestDiaForm)
{
    ui->setupUi(this);
}

TestDiaForm::~TestDiaForm()
{
    delete ui;
}
