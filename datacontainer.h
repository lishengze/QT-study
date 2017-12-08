#ifndef DATACONTAINER_H
#define DATACONTAINER_H
#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>
#include "worker.h"

class DataContainer: public QObject
{
    Q_OBJECT
public:
    DataContainer();
    DataContainer(QList<int> oridata, QObject *parent = Q_NULLPTR);
    ~DataContainer();

    void start();

public slots:
    void receiveProcessedData(QList<int> processedData);

signals:
    void sendProcessedDataToMaster(QList<int>);
    void sendOriDataToWorker(QList<int>);

private:
    QThread m_thread;
    QList<int> m_oriData;
};

#endif // DATACONTAINER_H
