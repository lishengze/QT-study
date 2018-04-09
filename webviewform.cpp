#include "webviewform.h"
#include "ui_webviewform.h"

WebViewForm::WebViewForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebViewForm)
{
    ui->setupUi(this);
}

WebViewForm::~WebViewForm()
{
    delete ui;
}
