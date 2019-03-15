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
    DataProcess(bool isRealTime, bool isBuySalePortfolio,
                QMap<QString, QList<QStringList>>& oridata,
                QMap<QString, double> buyCount, QString hedgeIndexCode,
                QList<int> macdTime,
                QObject *parent = Q_NULLPTR);

    DataProcess(bool isRealTime, bool isBuySalePortfolio,
                QMap<QString, QList<QStringList>>& oridata,
                QMap<QString, double> buyStrategy, QMap<QString, double> saleStrategy,
                QList<int> macdTime,
                QObject *parent = Q_NULLPTR);

    DataProcess(QMap<QString, QList<QStringList>>& oridata,
                QMap<QString, double>& portfolioMap, QString hedgeIndexCode,
                QList<int> aveNumbList, QList<bool> isEMAList,
                int mainAveNumb, int subAveNumb, int energyAveNumb,
                double css12Rate, double mainCssRate1, double mainCssRate2,
                double energyCssRate1, double energyCssRate2,
                double maxCSS, double minCSS,
                QObject *parent = Q_NULLPTR);

    ~DataProcess();

    void initIndexHedgeMetaInfo();
    void filterHedgeIndexData();

    QList<QList<double>> computeAllData();
    QList<QList<double>> computeHedgedData();
    QList<QList<double>> computeVotData();
    QList<QList<double>> computeMacdData();
    QList<QList<double>> computeSnapshootData();

public slots:
    void receiveOrigianlHistoryData(QString dataType);

signals:
    void getProcessedData_signal(QList<QList<double>> allData);
    void sendStrategyData(QList<QList<double>> strategyData);
    void sendVotData(QList<QList<double>> votData);
    void sendMACDData(QList<QList<double>> macdData);

private:
    bool                                 m_isCSSChart;
    bool                                 m_isRealTime;
    bool                                 m_isBuySalePortfolio;

    QMap<QString, QList<QStringList>>    m_oriPortfilioData;
    
    QStringList                          m_secodeNameList;

    QMap<QString, double>                   m_portfolio;
    QMap<QString, double>                   m_buyPortfolio;
    QMap<QString, double>                   m_salePortfolio;

    QMap<QString, double>                   m_indexPriceMap;

    QMap<QString, QStringList>           m_indexHedgeData;
    QString                              m_hedgeIndexCode;

    QList<int>                           m_macdTime;
    QList<double>                        m_hedgedData;
    QList<double>                        m_votData;
    QList<double>                        m_macdData;
    QList<MACD>                          m_MACDData;
    QList<double>                        m_timeData;
    QList<double>                        m_indexCodeData;

    QList<double>                        m_mainList;
    QList<double>                        m_subValueList;
    QList<double>                        m_energyValueList;
    QList<QList<double>>                 m_aveList;

    int                                  m_mainAveNumb;
    int                                  m_subAveNumb;
    int                                  m_energyAveNumb;
    double                               m_mainCssRate1;
    double                               m_mainCssRate2;
    double                               m_energyCssRate1;
    double                               m_energyCssRate2;

    double                               m_css12Rate;
    double                               m_maxCSS;
    double                               m_minCSS;

    QList<int>                           m_aveNumbList;
    QList<bool>                          m_isEMAList;

};


#endif // DATAPROCESS_H
