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
    DataProcess(QMap<QString, QList<QStringList>> oridata, QMap<QString, int> buyCount,
                QList<int> macdTime, QObject *parent = Q_NULLPTR);
    ~DataProcess();
    void filterIndexHedegeData();
    QList<QList<double>> computeAllData();
    QList<QList<double>> computeStrategyData();
    QList<QList<double>> computeVotData();
    QList<QList<double>> computeMACDData();


public slots:
    void receiveStartProcessData(QString dataType);

signals:
    void sendAllData(QList<QList<double>> allData);
    void sendStrategyData(QList<QList<double>> strategyData);
    void sendVotData(QList<QList<double>> votData);
    void sendMACDData(QList<QList<double>> macdData);

private:
    QMap<QString, QList<QStringList>> m_oriData;
    QMap<QString, int> m_buyCount;

    QMap<QString, int> m_indexHedgeMetaInfo;
    QString m_indexHedgeCode;
    QList<QStringList> m_indexHedgeData;
    int m_indexHedgeCount;

    QList<int> m_macdTime;
    QList<double> m_strategyData;
    QList<double> m_votData;
    QList<double> m_macdData;
    QList<double> m_timeData;
};


#endif // DATAPROCESS_H
