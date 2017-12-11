#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include <QList>
#include <QObject>
#include <QMutexLocker>
#include <QMutex>
#include "datacontainer.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    Dialog(int dataNumb=100, int threadNumb=12, QWidget *parent = 0);
    ~Dialog();

    void setOriData();
    void startProcessData();

    QList<QList<int>> allocateThreadData();

public slots:
    void receiveProcessedData(QList<int> subThreadData);
    void receiveCompleteData(QList<int> comData);

signals:
    void sendTestSignal(QList<int> testData);
    void sendCompleteData(QList<int> comData);

private slots:
    void on_startProcessData_clicked();

private:
    Ui::Dialog *ui;
    QList<DataContainer*> m_dataContainerList;
    QList<QThread*> m_ThreadList;

    QList<int> m_oriData;
    QList<int> m_processedData;

    int m_dataNumb;
    int m_threadNumb;
    int m_processedThreadCount;
    mutable QMutex m_mutex;
};

#endif // DIALOG_H
