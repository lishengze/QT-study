#include "allwarningform.h"
#include "ui_allwarningform.h"

AllWarningForm::AllWarningForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AllWarningForm)
{
    ui->setupUi(this);
}

AllWarningForm::~AllWarningForm()
{
    delete ui;
}
