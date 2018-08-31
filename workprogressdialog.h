#ifndef WORKPROGRESSDIALOG_H
#define WORKPROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class WorkProgressDialog;
}

class WorkProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WorkProgressDialog(QWidget *parent = 0);
    void reset();
    void setRange(int maxValue, int minValue);
    void setWorkInfo(QString workInfo);
    void updateWorkState(int currValue);
    void disableStopButton();
    void activateButton();

    void setButtonText(QString);

    ~WorkProgressDialog();

signals:
    void stopWork_signal();

private slots:

    void on_stop_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::WorkProgressDialog *ui;
};

#endif // WORKPROGRESSDIALOG_H
