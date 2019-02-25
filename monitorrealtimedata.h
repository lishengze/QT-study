#ifndef MONITORREALTIMEDATA_H
#define MONITORREALTIMEDATA_H
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTimer>
#include "toolfunc.h"
#include "macd.h"
#include "chartdata.h"
#include "database.h"
#include "process_data_func.h"

class MonitorRealTimeData: public QObject
{
    Q_OBJECT
public:
    MonitorRealTimeData(QString dbhost, int monitorTime,  
                        bool isBuySalePortfolio, bool isCSSChart,
                        QList<int> macdTime, QString hedgeIndexCode,
                        QMap<QString, int> seocdebuyCountMap=EmpytQStringIntMap(),
                        QStringList secodeNameList=EmpytStringList(),
                        QMap<QString, int> buyStrategyMap = EmpytQStringIntMap(),
                        QMap<QString, int> saleStrategyMap = EmpytQStringIntMap(),
                        QObject* parent = 0);

    MonitorRealTimeData(QString dbhost, int monitorTime, QString futureName,
                        QObject* parent = 0);

    MonitorRealTimeData(QString dbhost, int monitorTime, 
                        QString selectIndex, QString hedgedIndex,
                        QObject* parent = 0);

    ~MonitorRealTimeData();

    void initCommonData();
    void initIndexHedgeMetaInfo();
    void initDatabase();
    void initTimer();

    void startTimer();
    void stopTimer();
    
    void addMacd(MACD initMacdData);

    bool checkPortfolioData(QMap<QString,QStringList> realTimeData);
    ChartData computeSpreadMACDData();
    QList<double> computeSpreadData();

    void readFutureData();
    void readIndexData();
    void readPortfolioData();

signals:
   
    void sendPreCloseData(double);

    void sendTradeOver();
    void startMonitorTimer_signal();
    void stopMonitorTimer_signal();

    void sendRealtimeSpreadMACDData_signal(ChartData);
    void sendRealtimeSpreadData_signal(QList<double> );
    void sendRealtimeIndexData_signal(QList<double>);
    void sendRealtimeFutureData_signal(QList<double>);

    void sendRealTimeData(ChartData data);
    void sendHistFutureData_signal(QList<double>);
    
    void sendRealTimeIndexData_signal(QStringList, QStringList);
    void sendRealTimeIndexDataError_signal(QString);    

public slots:
    void startReadRealtimeData_slot();

    void getPreCloseSpread_slot();

    void getHistFutureData_slot();

    void getRealtimeData_slot();

private:
    QString                         m_dbhost;
    Database*                       m_database;

    bool                            m_isPortfolio;
    bool                            m_isBuySalePortfolio;
    bool                            m_isFuture;
    bool                            m_isIndex;
    bool                            m_isCSSChart;

    bool                            m_isInitTimer;
    QString                         m_futureName;

    QMap<QString, QStringList>      m_realTimeData;
    int                             m_monitorTime;
    int                             m_singleSecodeReadTime;
    int                             m_minWaitTime;
    QTimer*                         m_pTimer;

    QList<int>                      m_macdTime;
    QMap<QString, QList<double>>    m_vot;
    QMap<QString, QList<QString>>   m_time;

    QMap<QString, double>           m_indexPriceMap;
    QString                         m_hedgeIndexCode;
    QStringList                     m_secodeNameList;

    QMap<QString, int>              m_portfolio;
    QMap<QString, int>              m_buyPortfolio;
    QMap<QString, int>              m_salePortfolio;

    QList<MACD>                     m_macdData;

    QString                         m_selectIndex;
    QString                         m_hedgedIndex;
};
#endif // MONITORREALTIMEDATA_H
