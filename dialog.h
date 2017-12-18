#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include <QMap>
#include <QString>
#include <QStringList>
#include "test.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    void initData();
    ~Dialog();

private slots:
    void on_testWset_clicked();

    void on_loginButton_clicked();

    void on_testWsdButton_clicked();

    void on_testWsqButton_clicked();

signals:
    void startLogin();
    void startWsq();
    void startWset();
    void startWsd();

public slots:
    void checkData();


private:
    Ui::Dialog *ui;
    QThread m_thread;
    Test* m_testObj;
    int m_updateTime;
    QStringList m_secodeNameList;
    QMap<QString, QList<QStringList>> m_wsqData;
};

#endif // DIALOG_H
