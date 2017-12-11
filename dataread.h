#ifndef DATAREAD_H
#define DATAREAD_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>

class DataRead: public QObject
{
    Q_OBJECT
public:
    DataRead(QList<int> oridata, QObject *parent = Q_NULLPTR);
    ~DataProcess();

    QList<int> readHistoryData();
    QList<int> readRealTimeData();

public slots:
    void receiveStartReadData(QString dataType);

signals:
    void sendHistoryData(QList<int> historyData);
    void sendRealTimeData(QList<int> realtimeData);

public:
    QList<int> m_oriData;
};

#endif // DATAREAD_H
