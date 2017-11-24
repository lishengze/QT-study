#include "chartform.h"
#include "ui_chartform.h"

ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
}

ChartForm::~ChartForm()
{
    delete ui;
}
