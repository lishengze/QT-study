#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>

class DataContainer: public QObject
{
    Q_OBJECT
public:
    DataContainer(QList<int> oridata, QObject *parent = Q_NULLPTR);
    ~DataContainer();

    QList<int> processData();
    void start();

public slots:
    void receiveTestData(QList<int>);

signals:
    void sendProcessedDataToMaster(QList<int>);

public:
    QList<int> m_oriData;
};


#endif // DATACONTAINER_H
