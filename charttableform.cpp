#include "charttableform.h"
#include "ui_charttableform.h"

ChartTableForm::ChartTableForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartTableForm)
{
    ui->setupUi(this);
}

ChartTableForm::~ChartTableForm()
{
    delete ui;
}
