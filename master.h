#ifndef MASTER_H
#define MASTER_H
#include <QThread>
#include <QList>
#include <QObject>
#include <QMutexLocker>
#include <QMutex>
#include "datacontainer.h"

class Master: public QObject
{
    Q_OBJECT
public:
    Master(int dataNumb=100, int threadNumb=12, QObject *parent = Q_NULLPTR);
    ~Master();
    void setOriData();
    void startProcessData();

    QList<QList<int>> allocateThreadData();

signals:
    void sendOriData(QList<int>);

public slots:
    void receiveProcessedData(QList<int> subThreadData);

private:
    QList<DataContainer*> m_dataContainerList;
    QList<int> m_oriData;
    QList<int> m_processedData;
    int m_dataNumb;
    int m_threadNumb;
    int m_processedThreadCount;
    mutable QMutex m_mutex;
};

#endif // MASTER_H
