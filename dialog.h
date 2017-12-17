#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include "test.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    int login();
    int testWset();
    ~Dialog();

private slots:
    void on_testWset_clicked();

    void on_loginButton_clicked();

signals:
    void startLogin();

private:
    Ui::Dialog *ui;
    QThread m_thread;
    Test* m_testObj;
};

#endif // DIALOG_H
