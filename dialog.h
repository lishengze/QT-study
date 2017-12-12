#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include <QList>
#include <QObject>
#include <QMutexLocker>
#include <QMutex>

#include "dataread.h"
#include "dataprocess.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    Dialog(int threadNumb, QWidget *parent = 0);
    ~Dialog();

    void setOriData();
    void startReadData();
    void startProcessData();

    void releaseDataReaderSrc();
    void releaseDataProcessSrc();
    QList<QStringList> allocateThreadData();

public slots:
    void receiveProcessedData(QMap<QString, QList<QStringList>> subThreadData);
    void receiveAllProcessedData(QList<QList<double>> allData);

signals:
    void sendStartReadSignal(QString dataType);
    void sendStartProcessData(QString dataType);

private slots:
    void on_startProcessData_clicked();

private:
    Ui::Dialog *ui;
    QList<DataProcess*> m_dataProcessList;
    QList<QThread*> m_dataProcessThreadList;

    QList<DataRead*> m_dataReaderList;
    QList<QThread*> m_dataReaderThreadList;

    QStringList m_oriData;
    QMap<QString, int> m_secodeBuyCount;
    QMap<QString, QList<QStringList>> m_completeTableData;

    int m_dataNumb;
    int m_threadNumb;
    int m_processedThreadCount;
    mutable QMutex m_mutex;
};

#endif // DIALOG_H
