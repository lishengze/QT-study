#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QMap>
#include <QStringList>

class DataProcess: public QObject
{
    Q_OBJECT
public:
    DataProcess(QMap<QString, QList<QStringList>> oridata, QMap<QString, int> buyCount, QObject *parent = Q_NULLPTR);
    ~DataProcess();
    QList<QList<double>> computeAllData();
    QList<double> computeStrategyData();
    QList<double> computeVotData();
    QList<double> computeMACDData();
    QList<double> computeTimeData();

public slots:
    void receiveStartProcessData(QString dataType);

signals:
    void sendAllData(QList<QList<double>> allData);
    void sendStrategyData(QList<double> strategyData);
    void sendVotData(QList<double> votData);
    void sendMACDData(QList<double> macdData);
    void sendTimeData(QList<double> timeData);

public:
    QMap<QString, QList<QStringList>> m_oriData;
    QMap<QString, int> m_buyCount;
};


#endif // DATAPROCESS_H
