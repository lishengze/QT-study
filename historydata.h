#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include <QTableView>
#include <QMutexLocker>
#include <QMutex>

#include "database.h"
#include "dataread.h"
#include "dataprocess.h"
#include "dataread.h"
#include "toolfunc.h"
#include "process_data_func.h"
#include "monitorrealtimedata.h"

class HistoryData : public QObject
{
    Q_OBJECT
public:
    HistoryData(QString dbhost, QString databaseName, 
                bool isBuySalePortfolio, bool isRealTime, 
                QList<QString> secodeNameList,
                QList<int> macdTime, QString hedgeIndexCode,
                QString startDate="", QString endDate="", 
                QMap<QString, int> seocdebuyCountMap = EmpytQStringIntMap(),
                QMap<QString, int> buyStrategyMap = EmpytQStringIntMap(),
                QMap<QString, int> saleStrategyMap = EmpytQStringIntMap(),
                QObject *parent = 0);

    HistoryData(QString dbhost, QString databaseName, 
                QString startDate, QString endDate,
                QList<QString> secodeNameList, QString hedgeIndexCode,
                QMap<QString, int> seocdebuyCountMap,
                QList<int> aveNumbList, QList<bool> isEMAList,
                int mainAveNumb, int subAveNumb, int energyAveNumb,
                double css12Rate, double mainCssRate1, double mainCssRate2,
                double energyCssRate1, double energyCssRate2,
                double maxCSS, double minCSS, int id = 0,
                QObject *parent = 0);

    HistoryData(QString dbhost, QString databaseName,
                QString startDate, QString endDate,
                QString selectCodeName, QString hedgedCodeName,
                QList<int> aveNumbList, QList<bool> isEMAList,
                int mainAveNumb, int subAveNumb, int energyAveNumb,
                double css12Rate, double mainCssRate1, double mainCssRate2,
                double energyCssRate1, double energyCssRate2,
                double maxCSS, double minCSS, int id,
                QObject *parent = 0);

    HistoryData(QString dbhost, QString databaseName,
                QString selectIndex, QString hedgedIndex,
                QString startDate, QString endDate, bool isRealTime,
                int aveNumb, double css12Rate,
                double cssRate1, double cssRate2,
                double m_maxCSS, double m_minCSS,
                QObject *parent = 0);

    HistoryData(QString dbhost, QStringList timeList, QObject* parent = 0);

    ~HistoryData();

    void initCommonData();

    void initDatabase();
    void initRealtimeWorker();

    void initPortfolioSignalType();
    void initPortfolioThreadSecodeList();
    void initPortfolioIndexHedgeMetaInfo();

    void initReadPortfolioDataSignal();

    void initCompPortfolioDataSignal();

    void initTimeFormat();

    void releaseAveList();
    QList<double> getRealtimeAveList();
    QList<double> getRealtimeCSSList();
    bool isAddRealtimeData();
    void resetRealtimeData();

    void releaseReadPortfolioDataThreads ();
    void releaseCompPortfolioDataThreads ();    
    void completeIndexTdyRealtimeData(QList<QStringList>& oriData, QString codeName);

    void computePortfolioData();
    void computeHistPortData();

    void filterHedgeIndexData();

    void computeHistoryCSSData();
    void computeRealtimeCSSData();

    void computeHedgedData();
    void computeVotData();
    void computeMacdData();

signals:
    void getOrinPortfolioData_Signal(QString dataType);
    void startCompPortfolioData_Signal(QString dataType);
    void startReadRealtimeData_signal();

    void sendTableViewInfo_Signal(QString msg);
    void sendHistPortfolioData_Signal(QList<QList<double>> allData);

    void sendHistCSSData_signal(QList<QString>, QList<QList<double>>, QList<QList<double>>, int);
    void sendRealTimeCSSData_signal(QList<double>, QList<double>, int, bool);

    void sendTableList_signal(QString timeType, QStringList tableList);

    void sendLatestHistIndexData_signal(QString, double, double, QList<int>);
    void sendHistIndexData_signal(QList<QString>, QList<double>, QList<double>,
                                  QList<double>, QList<double>, QList<double>);
    void sendHistIndexError_signal(QString);

public slots:
    void getTableList_slot();
    void getRealtimeData_slot();
    void getHistPortfolioData_slot();

    void getIndexCSSData_slot();

    void sendHistOrinPortfolioData_slot(QMap<QString, QList<QStringList>> subThreadData);
    void sendHistCompPortfolioData_slot(QList<QList<double>> allData);

    void sendRealtimeSpreadMACDData_slot(ChartData);
    void sendRealtimeSpreadData_slot(QList<double> );
    void sendRealtimeIndexData_slot(QList<double>);
    void sendRealtimeFutureData_slot(QList<double>);


private:
    bool                               m_isPortfolio;
    bool                               m_isBuySalePortfolio;
    bool                               m_isFuture;
    bool                               m_isIndex;

    bool                               m_isCSSChart;
    bool                               m_isRealTime;    
    bool                               m_isShort;

    int                                m_threadNumb;
    QString                            m_signalType;
    mutable QMutex                     m_mutex;

    int                                m_chartViewID;
    QStringList                        m_timeTypeList;
    QString                            m_dbhost;
    QString                            m_databaseName;
    QString                            m_startDate;
    QString                            m_endDate;

    QString                            m_hedgeIndexCode;
    QList<int>                         m_macdTime;
    QStringList                        m_keyValueList;

    QMap<QString, int>                 m_portfolio;
    QMap<QString, int>                 m_buyPortfolio;
    QMap<QString, int>                 m_salePortfolio;

    QList<QString>                     m_secodeNameList;
    QString                            m_timeFormat;
    QString                            m_timeType;

    QList<DataProcess*>                m_dataProcessList;
    QList<QThread*>                    m_dataProcessThreadList;

    QList<QStringList>                 m_threadSecodeList;
    QList<DataRead*>                   m_dataReaderList;
    QList<QThread*>                    m_dataReaderThreadList;
    int                                m_readDataThreadCount;
    QMap<QString, QList<QStringList>>  m_completeTableData;

    MonitorRealTimeData*               m_realtimeWorker;
    QThread                            m_realtimeThread;
    
    // 指数对冲信息;
    Database*                          m_database;
    QString                            m_selectIndex;
    QString                            m_hedgedIndex;
    int                                m_aveNumb;
    double                             m_css12Rate;
    double                             m_cssRate1;
    double                             m_cssRate2;
    double                             m_maxCSS;
    double                             m_minCSS;

    QString                            m_selectCodeName;
    QString                            m_hedgedCodeName;

    int                                m_mainAveNumb;
    int                                m_subAveNumb;
    int                                m_energyAveNumb;
    double                             m_mainCssRate1;
    double                             m_mainCssRate2;
    double                             m_energyCssRate1;
    double                             m_energyCssRate2;
    int                                m_databaseID;

    QList<int>                         m_aveNumbList;
    QList<bool>                        m_isEMAList;

    // MACD
    QMap<QString, int>                   m_indexHedgeMetaInfo;
    QMap<QString, QStringList>           m_indexHedgeData;

    QList<double>                        m_hedgedData;
    QList<double>                        m_votData;
    QList<double>                        m_macdData;
    QList<MACD>                          m_MACDData;
    QList<double>                        m_timeData;
    QList<double>                        m_indexCodeData;    
    QMap<QString, QList<QStringList>>    m_oriPortfilioData;

    // 势能指标计算;
    QList<QString>                     m_timeList;
    QList<double>                      m_oriTypeList;
    QList<double>                      m_typList;
    QList<double>                      m_mainList;
    QList<double>                      m_subValueList;
    QList<double>                      m_earningList;        
    QList<QList<double>>               m_aveList;

    double                             m_curEpochRealtime;

    double                             m_mainValue;
    double                             m_subValue;
    double                             m_energyValue;

    double                             m_curMainCSST;
    double                             m_oldMainCSST;
    double                             m_curMainCSSA;
    double                             m_oldMainCSSA;
    double                             m_curSubCSST;
    double                             m_oldSubCSST;
    double                             m_curSubCSSA;
    double                             m_oldSubCSSA;
    double                             m_curEnergyCSST;
    double                             m_oldEnergyCSST;
    double                             m_curEnergyCSSA;
    double                             m_oldEnergyCSSA;  

    double                             m_oldSelectData;
    double                             m_curSelectData;
    double                             m_oldHedgedData;
    double                             m_curHedgedData;

    int                                m_oldTime;
    int                                m_curTime;

    QList<int>                         m_oneDayTimeList;
    bool                               m_isAddedRealtimeData;

    int                                m_lastAddedTime;
};

#endif // HISTORYDATA_H
