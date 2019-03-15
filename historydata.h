#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include <QTableView>
#include <QMutexLocker>
#include <QMutex>
#include <mutex>
#include <condition_variable>
#include "datastruct.h"

#include "database.h"
#include "dataread.h"
#include "dataprocess.h"
#include "dataread.h"
#include "toolfunc.h"
#include "process_data_func.h"
#include "monitorrealtimedata.h"

using std::condition_variable;
using std::mutex;

class HistoryData : public QObject
{
  Q_OBJECT
public:
    HistoryData(DatabaseParam dbParam, HedgedParam hedgedParam, CSSParam cssParam, 
                int dataID = 0, QObject *parent = 0);

    HistoryData(QString dbhost, QStringList timeList, QObject *parent = 0);

    ~HistoryData();

    void initAll(int type);
    void initCommonData();

    void initDatabase();
    void initRealtimeWorker();

    void initPortfolioSignalType();
    void initPortfolioThreadSecodeList();
    void initPortfolioIndexHedgeMetaInfo();

    void setReadPortfolioDataThreads();
    void setCompPortfolioDataThreads();

    void initTimeFormat();

    void transPortfolio();

    void releaseMonitorWorker();
    void releaseAveList();
    QList<double> getRealtimeAveList();
    QList<double> getRealtimeCSSList();
    bool isAddRealtimeData();
    void resetRealtimeData();

    void releaseReadPortfolioDataThreads();
    void releaseCompPortfolioDataThreads();
    void completeIndexRealtimeData(QList<QStringList> &oriData, QString codeName);

    void checkSuppleData();
    void computePortfolioData();
    void completeSupplePortfolioData();
    void computePortfolioHistCSSData();

    void computeHistoryCSSData();
    void computeRealtimeCSSData();

    void computeCurEarning();
    void computeCurMACD();

signals:
    void getPreCloseData_signal();
    void getHistPortfolioData_Signal(QString dataType);
    void getRealtimeData_signal();

    void sendPreCloseData_signal(double);

    void sendHistPortfolioData_Signal(QList<QList<double>> allData);
    void sendHistCSSData_signal(QList<QString>, QList<QList<double>>, QList<QList<double>>, int);

    void sendRealTimeCSSData_signal(QList<double>, QList<double>, int, bool);
    void sendRealtimeSpreadMACDData_signal(ChartData);

    void sendTableList_signal(QString timeType, QStringList tableList);
    void sendTableViewInfo_Signal(QString msg);

    void processRealtimeDataComplete_signal();

public slots:
    void getTableList_slot();

    void getPreCloseData_slot();

    void getHistPortfolioData_slot();
    void getHistIndexData_slot();

    void getRealtimeData_slot();

    void sendPreCloseData_slot(double);
    void sendHistPortfolioData_slot(QMap<QString, QList<QStringList>> subThreadData);

    void sendRealtimeSpreadMACDData_slot(QList<double>, QMap<QString, double>);
    void sendRealtimeSpreadData_slot(QList<double>, QMap<QString, double>);

    void sendRealtimeIndexData_slot(QList<double>);
    void sendRealtimeFutureData_slot(QList<double>);

    void processRealtimeDataComplete_slot();

  private:

    bool                                  m_isInit;
    HedgedParam                           m_hedgedParam;
    DatabaseParam                         m_dbParam;
    CSSParam                              m_cssParam;

    bool                                  m_isFuture;
    bool                                  m_isSupple;

    int                                   m_threadNumb;
    QString                               m_signalType;
    mutable QMutex                        m_mutex;

    QStringList                           m_timeTypeList;

    QStringList                           m_keyValueList;

    Database *                            m_database;
    QList<QString>                        m_codeList;
    QString                               m_timeFormat;
    QString                               m_timeType;

    QList<QStringList>                    m_threadSecodeList;
    QList<DataRead *>                     m_dataReaderList;
    QList<QThread *>                      m_dataReaderThreadList;
    int                                   m_readDataThreadCount;

    QMap<QString, QList<QStringList>>     m_oriPortfilioData;
    QMap<QString, QList<QStringList>>     m_oriHistPortfolioData;
    QMap<QString, QList<QStringList>>     m_suppledPortfolioData;

    MonitorRealTimeData *                 m_realtimeWorker;
    QThread                               m_realtimeThread;

    // 势能指标计算;
    int                               m_dataID;
    QList<QString>                    m_timeList;
    QList<double>                     m_oriTypeList;
    QList<double>                     m_typList;
    QList<double>                     m_mainList;
    QList<double>                     m_subValueList;
    QList<double>                     m_earningList;
    QList<QList<double>>              m_aveList;

    double                            m_curEpochRealtime;
    double                            m_mainValue;
    double                            m_subValue;
    double                            m_energyValue;

    double                            m_curMainCSST;
    double                            m_oldMainCSST;
    double                            m_curMainCSSA;
    double                            m_oldMainCSSA;
    double                            m_curSubCSST;
    double                            m_oldSubCSST;
    double                            m_curSubCSSA;
    double                            m_oldSubCSSA;
    double                            m_curEnergyCSST;
    double                            m_oldEnergyCSST;
    double                            m_curEnergyCSSA;
    double                            m_oldEnergyCSSA;
    int                               m_oldTime;
    int                               m_curTime;

    QList<int>                        m_oneDayTimeList;
    bool                              m_isAddedRealtimeData;
    int                               m_lastAddedTime;

    // MACD
    QMap<QString, double>             m_indexPriceMap;
    QMap<QString, QStringList>        m_indexHedgeData;

    QList<double>                     m_hedgedData;
    QList<double>                     m_votData;
    QList<double>                     m_macdData;
    QList<MACD>                       m_MACDData;
    QList<double>                     m_timeData;
    QList<double>                     m_indexCodeData;

    QMap<QString, double>             m_oldCodeClose;
    QMap<QString, double>             m_curCodeClose;
    double                            m_oldSelectData;
    double                            m_curSelectData;
    double                            m_oldHedgedData;
    double                            m_curHedgedData;
};

  // int                               m_mainAveNumb;
  // int                               m_subAveNumb;
  // int                               m_energyAveNumb;
  // double                            m_mainCssRate1;
  // double                            m_mainCssRate2;
  // double                            m_energyCssRate1;
  // double                            m_energyCssRate2;
  // double                            m_css12Rate;
  // double                            m_cssRate1;
  // double                            m_cssRate2;
  // double                            m_maxCSS;
  // double                            m_minCSS;
  // QList<int>                        m_aveNumbList;
  // QList<bool>                       m_isEMAList;

  // bool                                  m_isRealTime;
  // bool                                  m_isPortfolio;
  // bool                                  m_isPortfolioHedge;

  // bool                                  m_isCSSChart;
  // bool                                  m_isIndex;

  // QString                               m_startDate;
  // QString                               m_endDate;
  // QString                               m_weightDate;

  // QString                               m_oriCode;
  // QString                               m_hedgedCode;

  // QMap<QString, double>                    m_oriPortfolio;
  // QMap<QString, double>                    m_hedgedPortfolio;

  // int                                   m_hedgedType;

  // QList<int>                            m_macdTime;

#endif // HISTORYDATA_H
