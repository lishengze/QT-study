#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>

class DataProcess: public QObject
{
    Q_OBJECT
public:
    DataProcess(QList<int> oridata, QObject *parent = Q_NULLPTR);
    ~DataProcess();

    QList<int> computeAllData();
    QList<int> computeStrategyData();
    QList<int> computeVotData();
    QList<int> computeMACDData();

public slots:
    void receiveStartProcessData(QString dataType);

signals:
    void sendAllData(QList<int> allData);
    void sendStrategyData(QList<int> strategyData);
    void sendVotData(QList<int> votData);
    void sendMACDData(QList<int> macdData);

public:
    QList<int> m_oriData;
};


#endif // DATAPROCESS_H
