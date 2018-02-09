#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QMap>
#include <QStringList>
#include "macd.h"

class DataProcess: public QObject
{
    Q_OBJECT
public:
    DataProcess(bool isRealTime, QMap<QString, QList<QStringList>> oridata,
                QMap<QString, int> buyCount, QString hedgeIndexCode,
                QList<int> macdTime, QObject *parent = Q_NULLPTR);

    DataProcess(bool isRealTime, QMap<QString, QList<QStringList>> oridata,
                QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy,
                QList<int> macdTime, QObject *parent = Q_NULLPTR);

    DataProcess(QMap<QString, QList<QStringList>> oridata,
                QMap<QString, int> buyCount, QList<int> macdTime, QObject *parent = Q_NULLPTR);

    ~DataProcess();

    void initIndexHedgeMetaInfo();
    void filterHedgeIndexData();

    QList<QList<double>> computeAllData();
    QList<QList<double>> computeStrategyData();
    QList<QList<double>> computeVotData();
    QList<QList<double>> computeMacdData();

    QList<QList<double>> computeSnapshootData();


public slots:
    void receiveOrigianlHistoryData(QString dataType);

signals:
    void sendAllData(QList<QList<double>> allData);
    void sendStrategyData(QList<QList<double>> strategyData);
    void sendVotData(QList<QList<double>> votData);
    void sendMACDData(QList<QList<double>> macdData);

private:
    bool m_isRealTime;
    QMap<QString, QList<QStringList>> m_oriData;
    QMap<QString, int> m_seocdebuyCountMap;
    QStringList m_secodeNameList;

    QMap<QString, int> m_buyStrategyMap;
    QMap<QString, int> m_saleStrategyMap;

    QMap<QString, int> m_indexHedgeMetaInfo;
    QMap<QString, QStringList> m_indexHedgeData;
    QString m_hedgeIndexCode;    

    QList<int> m_macdTime;
    QList<double> m_strategyData;
    QList<double> m_votData;
    QList<double> m_macdData;
    QList<MACD> m_MACDData;
    QList<double> m_timeData;
};

//    QList<QList<double>> computeSnapshootDataReverse();
//    void computeChartDataReverse(QMap<QString, QStringList> oneTimeData);

//    QList<QList<double>> computeSnapshootData();
//    void computeChartData(QMap<QString, QStringList> oneTimeData);

#endif // DATAPROCESS_H
