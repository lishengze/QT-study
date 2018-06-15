#include "workprogressdialog.h"
#include "ui_workprogressdialog.h"

WorkProgressDialog::WorkProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkProgressDialog)
{
    ui->setupUi(this);
}

void WorkProgressDialog::reset() {
    ui->wrok_progressBar->reset();
}

void WorkProgressDialog::setRange(int maxValue, int minValue) {
    ui->wrok_progressBar->setMaximum(maxValue);
    ui->wrok_progressBar->setMinimum(minValue);
}

void WorkProgressDialog::setWorkInfo(QString workInfo) {
    ui->info_label->setText(workInfo);
}

void WorkProgressDialog::updateWorkState(int currValue) {
    ui->wrok_progressBar->setValue(currValue);
}

void WorkProgressDialog::disableStopButton() {
    ui->stop_pushButton->setDisabled(true);
}

WorkProgressDialog::~WorkProgressDialog()
{
    delete ui;
}

void WorkProgressDialog::on_stop_pushButton_clicked()
{
    this->close();
}

void WorkProgressDialog::closeEvent(QCloseEvent * event) {
    emit stopWork_signal();
}
