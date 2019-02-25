#include <cstdlib>
#include <QString>
#include <QStringList>
#include "historydata.h"
#include "toolfunc.h"
#include "secode_func.h"
#include "process_data_func.h"
#include "time_func.h"
#include "compute_func.h"
#include "energy_compute.h"
#include "spread_compute.h"
#include "io_func.h"
#include "database_func.h"

using std::unique_lock;

#pragma execution_character_set("utf-8")

// 简单组合对冲
HistoryData::HistoryData(QString dbhost, QString databaseName, 
                         bool isBuySalePortfolio, bool isRealTime, 
                         QList<QString> secodeNameList,
                         QList<int> macdTime, QString hedgeIndexCode,
                         QString startDate, QString endDate,
                         QMap<QString, int> seocdebuyCountMap, 
                         QMap<QString, int> buyStrategyMap,
                         QMap<QString, int> saleStrategyMap,
                         QObject *parent):
    m_dbhost(dbhost), m_databaseName(databaseName),
    m_secodeNameList(secodeNameList),
    m_macdTime(macdTime), m_hedgedCodeName(hedgeIndexCode),
    m_startDate(startDate), m_endDate(endDate),
    m_portfolio(seocdebuyCountMap), m_buyPortfolio(buyStrategyMap),
    m_salePortfolio(saleStrategyMap),
    QObject(parent)
{
    m_isPortfolio = true;
    m_isBuySalePortfolio = isBuySalePortfolio;
    m_isFuture = false;
    m_isIndex = false;

    m_isCSSChart = false;
    m_isRealTime = isRealTime;

    if (m_isRealTime)
    {
        m_oneDayTimeList = getOneDayTimeList(m_dbhost, m_databaseName);
    }

    initCommonData();
}

// 组合势能
HistoryData::HistoryData(QString dbhost, QString databaseName,
                         QString startDate, QString endDate,
                        QList<QString> secodeNameList, QString hedgeIndexCode,
                        QMap<QString, int> portfolioMap,
                        
                        QList<int> aveNumbList, QList<bool> isEMAList,
                        int mainAveNumb, int subAveNumb, int energyAveNumb,
                        double css12Rate, double mainCssRate1, double mainCssRate2,
                        double energyCssRate1, double energyCssRate2,
                        double maxCSS, double minCSS, int dataID,
                        QObject *parent):
                        m_dbhost(dbhost), m_databaseName(databaseName),
                        m_startDate(startDate), m_endDate(endDate),
                        m_secodeNameList(secodeNameList), m_hedgedCodeName(hedgeIndexCode),
                        m_portfolio(portfolioMap), m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
                        m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
                        m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
                        m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
                        m_maxCSS(maxCSS), m_minCSS(minCSS), m_databaseID(dataID),
                        QObject(parent)
{    
    m_isPortfolio = true;
    m_isBuySalePortfolio = false;
    m_isFuture = false;
    m_isIndex = false;    

    m_isCSSChart = true;

    if (getDate(m_endDate) == QDate::currentDate() && isTradingDay(getDate(m_endDate)))
    {
        m_isRealTime = true;
    }
    else
    {
        m_isRealTime = false;
    }
   
    initCommonData();
}

// 指数对冲;
HistoryData::HistoryData(QString dbhost, QString databaseName,
                         QString startDate, QString endDate,
                         QString selectCodeName, QString hedgedCodeName,

                         QList<int> aveNumbList, QList<bool> isEMAList,
                         int mainAveNumb, int subAveNumb, int energyAveNumb,
                         double css12Rate, double mainCssRate1, double mainCssRate2,
                         double energyCssRate1, double energyCssRate2,
                         double maxCSS, double minCSS, int dataID,
                         QObject *parent):
    m_isPortfolio(false), m_isCSSChart(true),
    m_dbhost(dbhost), m_databaseName(databaseName),
    m_startDate(startDate), m_endDate(endDate),
    m_selectCodeName(selectCodeName), m_hedgedCodeName(hedgedCodeName),
    m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
    m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
    m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
    m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
    m_maxCSS(maxCSS), m_minCSS(minCSS), m_databaseID(dataID),
    QObject(parent)
{
    m_isPortfolio = false;
    m_isBuySalePortfolio = false;
    m_isFuture = false;
    m_isIndex = true;    

    m_isCSSChart = true;

    if (getDate(m_endDate) == QDate::currentDate() 
    &&  isTradingDay(getDate(m_endDate)))
    {
        m_isRealTime = true;
    }
    else
    {
        m_isRealTime = false;
    }

    initCommonData();
}

// 数据库所有表列表
HistoryData::HistoryData(QString dbhost, QStringList timeTypeList, QObject* parent):
     m_dbhost(dbhost), m_timeTypeList(timeTypeList), QObject(parent)
{
    initDatabase();
}

HistoryData::~HistoryData() 
{
    for (int i = 0; i < m_dataProcessThreadList.size(); ++i) 
    {
        delete m_dataProcessThreadList[i];
        m_dataProcessThreadList[i] = NULL;
    }

    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) 
    {
        delete m_dataReaderThreadList[i];
        m_dataReaderThreadList[i] = NULL;
    }

    if (NULL != m_database) 
    {
        delete m_database;
        m_database = NULL;
    }

    if (m_isRealTime)
    {
        m_realtimeThread.quit();
        m_realtimeThread.wait();
    }
}

void HistoryData::initCommonData()
{
    m_isSupple = false;
    m_database = NULL;
    m_readDataThreadCount = 0;
    m_oldTime = 0;
    m_curTime = 0;
    m_lastAddedTime = 0;

    m_keyValueList.clear();
    m_keyValueList << "TCLOSE" << "VOTRUNOVER" << "PCTCHG";  

    if (m_selectCodeName.size() == 0 && m_hedgedCodeName.size() != 0)
    {
        m_selectCodeName = m_hedgedCodeName;
    }

    if (m_isRealTime)
    {
        m_oneDayTimeList = getOneDayTimeList(m_dbhost, m_databaseName);
        // printList(m_oneDayTimeList, "m_oneDayTimeList");
    }

    initDatabase();

    initTimeFormat();

    if (m_isPortfolio)
    {
        m_threadNumb = min(m_secodeNameList.size(), 8);
        initPortfolioSignalType();
        initPortfolioThreadSecodeList();
        setReadPortfolioDataThreads();
        initPortfolioIndexHedgeMetaInfo();                  
    }

    if (m_isRealTime && m_isPortfolio)
    {
        Database realtimeDB = Database(m_dbhost);
        QList<QString> rlCodeList = realtimeDB.getTableList("MarketData_RealTime");

        QList<QString> portfolioCodeList = m_portfolio.keys();

        for (QString code:portfolioCodeList)
        {
            if (rlCodeList.indexOf(getCompleteSecode(code, "wind")) < 0)
            {
                qDebug() << code << " is lost!";
                m_isRealTime = false;
            }
        }
    }

    if (m_isRealTime && m_isCSSChart)
    {
        initRealtimeWorker();        
    }
}

void HistoryData::initTimeFormat()
{
    if (!isMinuteType(m_databaseName))
    {
        m_timeFormat = "yyyyMMdd";
        m_timeType = "day";
    }
    else
    {
        m_timeFormat = "yyyyMMdd hhmm";
        m_timeType = (m_databaseName.split("_"))[1];
        m_timeType.remove(m_timeType.size()-1, 1);
    }
}

void HistoryData::initDatabase()
{
    m_database = new Database(m_dbhost);
}

void HistoryData::initRealtimeWorker()
{
    int monitorTime;
    if (m_isCSSChart)
    {
        monitorTime = 10 * 1000;
    }
    else
    {
        monitorTime = 5 * 1000;
    }

    if (m_isPortfolio)
    {
        m_realtimeWorker = new MonitorRealTimeData(m_dbhost, monitorTime, m_isBuySalePortfolio,
                                                   m_isCSSChart, m_macdTime, 
                                                   m_hedgedCodeName,  m_portfolio, m_portfolio.keys());

        if (m_isCSSChart)
        {
            connect(m_realtimeWorker, SIGNAL(sendRealtimeSpreadData_signal(QList<double>)), 
                    this, SLOT(sendRealtimeSpreadData_slot(QList<double>)));
        }
        else
        {
            connect(m_realtimeWorker, SIGNAL(sendRealtimeSpreadMACDData_signal(ChartData)), 
                    this, SLOT(sendRealtimeSpreadMACDData_slot(ChartData)));
        }
    }

    if (m_isIndex)
    {
        m_realtimeWorker = new MonitorRealTimeData(m_dbhost, monitorTime, 
                                                   m_selectCodeName, m_hedgedCodeName);   
        connect(m_realtimeWorker, SIGNAL(sendRealtimeIndexData_signal(QList<double>)), 
                this, SLOT(sendRealtimeIndexData_slot(QList<double>)));                                                        
    }

    connect(this, SIGNAL(startReadRealtimeData_signal()),
            m_realtimeWorker, SLOT(startReadRealtimeData_slot()));

    m_realtimeWorker->moveToThread(&m_realtimeThread);

    connect(&m_realtimeThread, SIGNAL(finished()),
            m_realtimeWorker,  SLOT(deleteLater()));

    m_realtimeThread.start(); 
}

void HistoryData::initPortfolioSignalType() 
{
    if (m_isCSSChart)
    {        
        m_signalType = "HistoryData";
    }
    else
    {
        if (m_isRealTime)
        {
            m_signalType = "RealTimeData";
        }
        else
        {
            m_signalType = "HistoryData";
        }        
    }
}

void HistoryData::initPortfolioThreadSecodeList() 
{
    allocateThreadCodeList(m_threadNumb, m_secodeNameList, m_threadSecodeList);
    // printList(m_threadSecodeList, "m_threadSecodeList");
}

void HistoryData::initPortfolioIndexHedgeMetaInfo() 
{
    m_indexPriceMap.insert("SH000300", 300);
    m_indexPriceMap.insert("SH000016", 300);  // 上证50
    m_indexPriceMap.insert("SH000852", 1000);
    m_indexPriceMap.insert("SH000904", 200);
    m_indexPriceMap.insert("SH000905", 200);
    m_indexPriceMap.insert("SH000906", 800);
    m_indexPriceMap.insert("SZ399903", 100);

    m_indexPriceMap.insert("000300.SH", 300);
    m_indexPriceMap.insert("000016.SH", 300);
    m_indexPriceMap.insert("000852.SH", 1000);
    m_indexPriceMap.insert("000904.SH", 200);
    m_indexPriceMap.insert("000905.SH", 200);
    m_indexPriceMap.insert("000906.SH", 800);
    m_indexPriceMap.insert("399903.SZ", 100);
}

void HistoryData::setReadPortfolioDataThreads() 
{
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");

    for (int i = 0; i < m_threadSecodeList.size (); ++i) 
    {

        DataRead* curDataReader = new DataRead(m_dbhost, m_databaseName, m_threadSecodeList[i],
                                               m_startDate, m_endDate, m_keyValueList);

        QThread* curThread = new QThread();
        curDataReader->moveToThread(curThread);

        connect (curThread,     SIGNAL(finished()),
                 curDataReader, SLOT(deleteLater()));

        connect (this,          SIGNAL(getOrinPortfolioData_Signal(QString)),
                 curDataReader, SLOT(getOrinPortfolioData_slot(QString)));

        connect (curDataReader, SIGNAL(sendHistoryData(QMap<QString, QList<QStringList>>)),
                this,           SLOT(sendHistOrinPortfolioData_slot(QMap<QString, QList<QStringList>>)),
                Qt::QueuedConnection);
        
        curThread->start();
        m_dataReaderList.append(curDataReader);
        m_dataReaderThreadList.append(curThread);
    }
}

void HistoryData::setCompPortfolioDataThreads() 
{
    int oridataCount = 0;
    for (int i = 0; i < m_secodeNameList.size (); ++i) {
        oridataCount += m_oriPortfilioData[m_secodeNameList[i]].size ();
    }
    emit sendTableViewInfo_Signal(QString(QString("原始数据获取完成, 原始数据总数: %1"))
                             .arg(oridataCount));

    DataProcess* curDataProcess;
    if (m_isCSSChart)
    {
        curDataProcess = new DataProcess(m_oriPortfilioData,
                                         m_portfolio, m_hedgedCodeName,
                                         m_aveNumbList, m_isEMAList,
                                         m_mainAveNumb, m_subAveNumb, m_energyAveNumb,
                                         m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                         m_energyCssRate1, m_energyCssRate2,
                                         m_maxCSS, m_minCSS);
    }
    else
    {
        if (m_isBuySalePortfolio)
        {
            curDataProcess = new DataProcess(m_isRealTime, m_isBuySalePortfolio, m_oriPortfilioData,
                                             m_buyPortfolio, m_salePortfolio, m_macdTime);
        }
        else
        {
            curDataProcess = new DataProcess(m_isRealTime, m_isBuySalePortfolio, m_oriPortfilioData,
                                             m_portfolio, m_hedgedCodeName, m_macdTime);
        }
    }

    QThread* curDataProcessThread = new QThread();
    curDataProcess->moveToThread (curDataProcessThread);

    connect (curDataProcessThread, SIGNAL(finished()),
             curDataProcess, SLOT(deleteLater()));

    connect (this, SIGNAL(startCompPortfolioData_Signal(QString)),
             curDataProcess, SLOT(receiveOrigianlHistoryData(QString)));

    connect(curDataProcess, SIGNAL(getProcessedData_signal(QList<QList<double>>)),
            this, SLOT(sendHistCompPortfolioData_slot (QList<QList<double>>)));

    curDataProcessThread->start ();
    m_dataProcessList.append (curDataProcess);
    m_dataProcessThreadList.append (curDataProcessThread);
}

void HistoryData::getTableList_slot()
{
    for (int i = 0 ; i<m_timeTypeList.size(); ++i)
    {
        QString databaseName = QString("MarketData_%1").arg(m_timeTypeList[i]);
        QStringList result = m_database->getTableList(databaseName);
        emit sendTableList_signal(m_timeTypeList[i], result);
    }
}

void HistoryData::getHistPortfolioData_slot() 
{
    emit getOrinPortfolioData_Signal(m_signalType);
}

void HistoryData::getRealtimeData_slot()
{
    emit startReadRealtimeData_signal();
}

void HistoryData::getIndexCSSData_slot() 
{
    if (m_isPortfolio)
    {
        emit getOrinPortfolioData_Signal(m_signalType);
        return;
    }

    QStringList keyList;
    keyList << "TDATE" << "TIME" << "TCLOSE" << "TOPEN" << "PCTCHG" << "VATRUNOVER" ;

    QList<QStringList> selectDBData = m_database->getDataByDate(m_startDate, m_endDate, keyList,
                                                                m_selectCodeName, m_databaseName);
    int secStartPos = getDelayStartDate(selectDBData);
    selectDBData = getSubList(selectDBData, secStartPos, selectDBData.size());

    if (m_isRealTime)
    {
        completeIndexRealtimeData(selectDBData, m_selectCodeName);
    }

    m_oldSelectData = selectDBData.last()[2].toDouble();
    m_curSelectData = selectDBData.last()[2].toDouble();

    if (m_hedgedCodeName.size() != 0)
    {
        QList<QStringList> hedgedDBData = m_database->getDataByDate(m_startDate, m_endDate, keyList,
                                                                    m_hedgedCodeName, m_databaseName);

        int hedStartPos = getDelayStartDate(hedgedDBData);
        hedgedDBData = getSubList(hedgedDBData, hedStartPos, hedgedDBData.size());

        if (m_isRealTime)
        {
            completeIndexRealtimeData(hedgedDBData, m_hedgedCodeName);
        }

        if (m_hedgedCodeName == m_selectCodeName)
        {
            zeroHistData(selectDBData);
        }

        getTypEarningList(selectDBData, hedgedDBData, m_timeFormat,
                           m_timeList, m_oriTypeList, m_earningList);

        getTransedTYP(m_oriTypeList, m_typList);

        m_oldHedgedData = hedgedDBData.last()[2].toDouble();
        m_curHedgedData = hedgedDBData.last()[2].toDouble();
    }
    else
    {
        getTypEarningList(selectDBData, m_timeFormat, m_timeList, m_oriTypeList, m_earningList);
        getTransedTYP(m_oriTypeList, m_typList);
    }

    computeHistoryCSSData();
}

void HistoryData::sendHistOrinPortfolioData_slot (QMap<QString, QList<QStringList>> subThreadData) 
{
    QMutexLocker locker(&m_mutex);
    // qDebug() << "receive " << m_signalType << QThread::currentThreadId();
    m_oriHistPortfolioData.unite (subThreadData);
    ++m_readDataThreadCount;

    emit sendTableViewInfo_Signal(QString("完成股票指数读取数目: %1").arg(m_oriHistPortfolioData.size ()));

    if (m_readDataThreadCount == m_threadNumb) 
    {
        m_readDataThreadCount = 0;
        if (m_oriHistPortfolioData.find("Error")!= m_oriHistPortfolioData.end())
        {
            emit sendTableViewInfo_Signal(QString("Error: 读取数据出错"));
        }
        else if (m_isSupple)
        {
            m_suppledPortfolioData = m_oriHistPortfolioData;
            m_oriHistPortfolioData.clear ();
            releaseReadPortfolioDataThreads();

            completeSupplePortfolioData();
            computeHistPortData();
        }
        else
        {
           m_oriPortfilioData = m_oriHistPortfolioData;
           m_oriHistPortfolioData.clear ();
           releaseReadPortfolioDataThreads();

           computePortfolioData();
        }
    }
}

void HistoryData::sendRealtimeIndexData_slot(QList<double> indexData)
{
    if (m_isCSSChart)
    {
        if (m_hedgedCodeName.size() == 0)
        {       
            m_curSelectData = indexData[0];

            double curChange = (m_curSelectData - m_oldSelectData) / m_oldSelectData;
            m_earningList.append(m_earningList.last() * (1 + curChange));
            m_oriTypeList.append((m_earningList.last() + m_earningList[m_earningList.size()-2]) / 2);

            /*
            qDebug() << QString("Close: %1, Open: %2, Time: %3")
                        .arg(closePrice).arg(indexData[1])
                        .arg(QDateTime::fromMSecsSinceEpoch(indexData[2])
                             .toString("yyyy.MM.dd hh:mm:ss")); 
                             */
        }
        else
        {
            // qDebug() << QString("%1: %2, %3: %4, Time: %5")
            //             .arg(m_selectCodeName).arg(indexData[0])
            //             .arg(m_hedgedCodeName).arg(indexData[1])
            //             .arg(QDateTime::fromMSecsSinceEpoch(indexData[2])
            //                 .toString("yyyy.MM.dd hh:mm:ss"));   
                                     
            m_curSelectData = indexData[0];
            m_curHedgedData = indexData[1];
            double curSelectChange = (m_curSelectData - m_oldSelectData) / m_oldSelectData;
            double curHedgedChange = (m_curHedgedData - m_oldHedgedData) / m_oldHedgedData;

            if (m_selectCodeName == m_hedgedCodeName)
            {
                curSelectChange = 0;
            }

            double relativeEarning = curSelectChange - curHedgedChange;

            m_earningList.append(m_earningList.last()*(1 + relativeEarning));
            m_oriTypeList.append((m_earningList.last() + m_earningList[m_earningList.size()-2]) / 2);
        }

        m_curEpochRealtime = indexData[2];

        computeRealtimeCSSData();
    }
    else
    {

    }
}

void HistoryData::sendRealtimeSpreadData_slot(QList<double> oridata)
{
    m_curSelectData = oridata[0];
    m_curHedgedData = oridata[2];

    double relativeEarning = (m_curSelectData - m_oldSelectData) / m_curHedgedData;

    m_earningList.append(m_earningList.last()*(1 + relativeEarning));
    m_oriTypeList.append((m_earningList.last() + m_earningList[m_earningList.size()-2]) / 2);    

    m_curEpochRealtime = oridata[1];

    // qDebug() << QString("oldSelectData:%1, curSelectData:%2, oldHedgedData:%3, curHedgedData:%4, %5")
    //                     .arg(m_oldSelectData).arg(m_curSelectData)
    //                     .arg(m_oldHedgedData).arg(m_curHedgedData)
    //                     .arg(QDateTime::fromMSecsSinceEpoch(oridata[1])
    //                     .toString("yyyy.MM.dd hh:mm:ss"));

    computeRealtimeCSSData();
}

void HistoryData::computePortfolioData () 
{
    if (m_isCSSChart)
    {
        checkSuppleData();

        if (!m_isSupple)
        {
            computeHistPortData();
        }        
    }
    else
    {
        QList<QList<double>> allData;
        if (m_isRealTime)
        {
            if (m_isBuySalePortfolio)
            {
                allData = computeSnapshootData(m_oriPortfilioData, m_buyPortfolio, m_salePortfolio, m_macdTime);                    
            }
            else
            {
                allData = computeSnapshootData(m_oriPortfilioData, m_portfolio, m_hedgedCodeName, 
                                                m_indexPriceMap[m_hedgedCodeName], m_macdTime);
            }                
        }
        else
        {
            if (m_isBuySalePortfolio)
            {
                computeHistSpreadTimeList(m_oriPortfilioData, m_buyPortfolio,
                                            m_salePortfolio, m_timeData, m_hedgedData);
            }
            else
            {
                computeHistSpreadTimeList(m_oriPortfilioData, m_portfolio, 
                                            m_hedgedCodeName,m_portfolio[m_hedgedCodeName], 
                                            m_indexPriceMap[m_hedgedCodeName], 
                                            m_timeData, m_hedgedData, m_indexCodeData);              
            }

            computeVotList(m_oriPortfilioData, m_votData);
            computeMACDList(m_hedgedData, m_macdTime, m_macdData);
            
            allData.append(m_timeData);
            allData.append(m_hedgedData);
            allData.append(m_votData);
            allData.append(m_macdData);
            allData.append(m_indexCodeData);                
        }
        emit sendHistPortfolioData_Signal(allData);
    }
}

void HistoryData::checkSuppleData()
{
    if (m_isRealTime)
    {
        QList<QString> suppleCodeList;
        checkHistData(m_oriPortfilioData, m_endDate, suppleCodeList);

        if (suppleCodeList.size() > 0)
        {
            m_isSupple = true;
            m_threadNumb = min(8, suppleCodeList.size());
            allocateThreadCodeList(m_threadNumb, suppleCodeList, m_threadSecodeList);
            setReadPortfolioDataThreads();
            emit getOrinPortfolioData_Signal("RealTimeData");
        }                    
    }    
}

void HistoryData::computeHistPortData()
{
    computeHistSpreadTimeList(m_oriPortfilioData, m_portfolio, 
                              m_hedgedCodeName,m_portfolio[m_hedgedCodeName], 
                              m_indexPriceMap[m_hedgedCodeName], 
                              m_timeData, m_hedgedData, m_indexCodeData);      

    m_curSelectData = m_hedgedData.last();
    m_oldSelectData = m_hedgedData.last();            
    m_curHedgedData = m_indexCodeData.last();
    m_oldHedgedData = m_indexCodeData.last();

    transEpochTimeList(m_timeList, m_timeData, m_timeFormat);

    getTypEarningList(m_hedgedData, m_indexCodeData, m_oriTypeList, m_earningList);

    getTransedTYP(m_oriTypeList, m_typList);

    computeHistoryCSSData();                              
}

void HistoryData::completeSupplePortfolioData()
{        
    checkRestoreData(m_oriPortfilioData, m_suppledPortfolioData);

    QMap<QString, int> longHistPortfolio = transPorfolio(m_portfolio, "tinysoft");
    QMap<QString, int> todyHistPortfolio = transPorfolio(m_portfolio, "wind");

    filterRealtimeData(m_oneDayTimeList, m_suppledPortfolioData);
    completeHistData(m_oriPortfilioData, m_suppledPortfolioData); 
}

void HistoryData::completeIndexRealtimeData(QList<QStringList>& oriData, QString codeName)
{
    if (isStockTradingNotStart() || QDate::currentDate() != getRealtimeDate(m_dbhost))
    {
        return;
    }

    Database dbObj(m_dbhost);
    QList<QStringList> tdyRealtimeData = dbObj.getSnapShootHistoryData(getCompleteIndexCode(codeName, "wind"));
    if (tdyRealtimeData.size()>1)
    {
        QList<int> onedayTimeList = getOneDayTimeList(m_dbhost, m_databaseName);
        QList<QStringList> fileteredList;
        int onedayTimeListStartIndex = 0;
        for (int i = 1; i < tdyRealtimeData.size(); ++i)
        {
            int oldTime = tdyRealtimeData[i-1].last().toLongLong() % 1000000;
            int curTime = tdyRealtimeData[i].last().toLongLong() % 1000000;

            for (int j = onedayTimeListStartIndex; j < onedayTimeList.size() - 1; ++j)
            {
                    if (oldTime == onedayTimeList[j])
                    {
                        QStringList addedData = tdyRealtimeData[i-1];
                        addedData[1] = QString("%1").arg(onedayTimeList[j]);
                        fileteredList.append(addedData);
                        ++onedayTimeListStartIndex;
                        break;                        
                    }
                    else if (oldTime < onedayTimeList[j] && curTime >= onedayTimeList[j])
                    {
                        QStringList addedData = tdyRealtimeData[i];
                        addedData[1] = QString("%1").arg(onedayTimeList[j]);
                        fileteredList.append(addedData);
                        ++onedayTimeListStartIndex;
                        break;
                    }
            }
        }

        int lastTime = tdyRealtimeData.last().last().toLongLong() % 1000000;
        if (lastTime >= 150000)
        {
            // qDebug() << tdyRealtimeData.last();
            QStringList addedData = tdyRealtimeData.last();
            addedData[1] = QString("%1").arg(onedayTimeList.last());
            fileteredList.append(addedData);            
        }


        printList(fileteredList, "fileteredList");

        // keyList << "TDATE" << "TIME" << "TCLOSE" << "TOPEN" << "PCTCHG" << "VATRUNOVER" ;
        
        for (int i = 0; i < fileteredList.size(); ++i)
        {
            if (isTimeEarly(oriData.last(), fileteredList[i]))
            {
                QStringList curAddedData;
                QString curDate = fileteredList[i][0];
                QString curTime = fileteredList[i][1];
                QString curClose = fileteredList[i][2];
                QString curOpen = fileteredList[i][5];
                QString oldClose = oriData.last()[2];
                QString pctchg = QString("%1").arg((curClose.toDouble() - oldClose.toDouble()) / oldClose.toDouble());
                QString vatrunover = fileteredList[i][4];
                curAddedData.append(curDate);
                curAddedData.append(curTime);
                curAddedData.append(curClose);
                curAddedData.append(curOpen);
                curAddedData.append(pctchg);
                curAddedData.append(vatrunover);
                oriData.append(curAddedData);
            }
        }
    }
}

void HistoryData::computeHistoryCSSData()
{
        computeAVEList(m_earningList, m_aveList, m_aveNumbList, m_isEMAList);

        QList<double> mainList = computeCSSList(m_typList,  m_curMainCSST, m_curMainCSSA,
                                                m_mainAveNumb, m_css12Rate,
                                                m_mainCssRate1, m_mainCssRate2, 
                                                m_maxCSS, m_minCSS, false);

        QList<double> subValueList = computeCSSList(m_typList, m_curSubCSST, m_curSubCSSA,
                                                    m_subAveNumb, m_css12Rate,
                                                    m_mainCssRate1, m_mainCssRate2, 
                                                    m_maxCSS, m_minCSS, false);

        QList<double> energyValueList = computeCSSList(m_typList, m_curEnergyCSST, m_curEnergyCSSA,
                                                    m_energyAveNumb, m_css12Rate,
                                                    m_energyCssRate1, m_energyCssRate2, 
                                                    m_maxCSS, m_minCSS, true);

        QList<QList<double>> cssList;
        cssList.append(mainList);
        cssList.append(subValueList);
        cssList.append(energyValueList);

        QList<QList<double>> aveList = m_aveList;
        aveList.insert(0, m_earningList);

        if (m_isPortfolio)
        {
            aveList.append(m_hedgedData);
            aveList.append(m_indexCodeData);
        }

        m_oldMainCSST = m_curMainCSST;
        m_oldMainCSSA = m_curMainCSSA;    
        m_oldSubCSST = m_curSubCSST;
        m_oldSubCSSA = m_curSubCSSA;
        m_oldEnergyCSST = m_curEnergyCSST;
        m_oldEnergyCSSA = m_curEnergyCSSA;

        emit sendHistCSSData_signal(m_timeList, aveList, cssList, m_databaseID);      
}

void HistoryData::computeRealtimeCSSData()
{
    m_typList.append((m_oriTypeList.last() + m_oriTypeList[m_oriTypeList.size()-2])/2);

    computeAVEAtom(m_earningList, m_earningList.size()-1, m_aveList, m_aveNumbList, m_isEMAList);

    m_mainValue = computeCSSAtom(m_typList, m_curMainCSST, m_curMainCSSA, m_typList.size()-1,
                                m_mainAveNumb, m_css12Rate,
                                m_mainCssRate1, m_mainCssRate2, 
                                m_maxCSS, m_minCSS, false);

    m_subValue = computeCSSAtom(m_typList, m_curSubCSST, m_curSubCSSA, m_typList.size()-1,
                                m_subAveNumb, m_css12Rate,
                                m_mainCssRate1, m_mainCssRate2, 
                                m_maxCSS, m_minCSS, false);

    m_energyValue = computeCSSAtom(m_typList, m_curEnergyCSST, m_curEnergyCSSA, m_typList.size()-1,
                                m_energyAveNumb, m_css12Rate,
                                m_energyCssRate1, m_energyCssRate2, 
                                m_maxCSS, m_minCSS, true);                                                                              

    QList<double> aveList = getRealtimeAveList();
    QList<double> cssList = getRealtimeCSSList();
    m_isAddedRealtimeData = isAddRealtimeData();
    emit sendRealTimeCSSData_signal(aveList, cssList, m_databaseID, m_isAddedRealtimeData);

    resetRealtimeData();

    qDebug() << QString("oldSelectData:%1, curSelectData:%2, oldHedgedData:%3, curHedgedData:%4, isAdded: %5 ")
                        .arg(m_oldSelectData).arg(m_curSelectData)
                        .arg(m_oldHedgedData).arg(m_curHedgedData)
                        .arg(m_isAddedRealtimeData);
}

QList<double> HistoryData::getRealtimeAveList()
{
    QList<double> result;
    result.append(m_earningList.last());
    for (int i = 0; i < m_aveList.size(); ++i)
    {
        result.append(m_aveList[i].last());
    }
    return result;
}

QList<double> HistoryData::getRealtimeCSSList()
{
    QList<double> result;
    result.append(m_mainValue);
    result.append(m_subValue);
    result.append(m_energyValue);

    return result;
}

bool HistoryData::isAddRealtimeData()
{
    bool result = false;
    if (m_oldTime == 0)
    {
        m_oldTime = QDateTime::fromMSecsSinceEpoch(m_curEpochRealtime).toString("hhmmss").toInt();
        m_curTime = m_oldTime;
    }
    else
    {
        m_oldTime = m_curTime;
        m_curTime = QDateTime::fromMSecsSinceEpoch(m_curEpochRealtime).toString("hhmmss").toInt();
    }

    for (int i = 0; i < m_oneDayTimeList.size() - 1; ++i)
    {
        if (m_oldTime <= m_oneDayTimeList[i] 
         && m_curTime >= m_oneDayTimeList[i])
        {
            if (m_lastAddedTime != m_oneDayTimeList[i])
            {
                result = true;
                m_lastAddedTime = m_oneDayTimeList[i];
            }
            break;
        }
    }

    return result;
}    

void HistoryData::resetRealtimeData()
{
    if (!m_isAddedRealtimeData)
    {
        for (int i = 0; i < m_aveList.size(); ++i)
        {
            m_aveList[i].pop_back();
        }
        m_earningList.pop_back();
        m_oriTypeList.pop_back();
        m_typList.pop_back();

        m_curMainCSSA = m_oldMainCSSA;
        m_curMainCSST = m_oldMainCSST;
        m_curSubCSSA = m_oldSubCSSA;
        m_curSubCSST = m_oldSubCSST;
        m_curEnergyCSSA = m_oldEnergyCSSA;
        m_curEnergyCSST = m_oldEnergyCSST;
    }
    else
    {
        m_oldMainCSST = m_curMainCSST;
        m_oldMainCSSA = m_curMainCSSA;    
        m_oldSubCSST = m_curSubCSST;
        m_oldSubCSSA = m_curSubCSSA;
        m_oldEnergyCSST = m_curEnergyCSST;
        m_oldEnergyCSSA = m_curEnergyCSSA;

        m_oldSelectData = m_curSelectData;
        m_oldHedgedData = m_curHedgedData;

        m_oldTime = m_curTime;
    }

    releaseAveList();
}

void HistoryData::releaseAveList()
{
    QList<QList<double>> result;
    for (int i = 0; i < m_aveList.size(); ++i)
    {
        QList<double> curList;
        curList.append(m_aveList[i].last());
        result.append(curList);
    }

    m_aveList = result;
}

void HistoryData::releaseReadPortfolioDataThreads () 
{
    for (int i = 0; i < m_dataReaderList.size(); ++i) 
    {
        if (NULL != m_dataReaderList[i])
        {
            disconnect (this,                SIGNAL(getOrinPortfolioData_Signal(QString)),
                        m_dataReaderList[i], SLOT(getOrinPortfolioData_slot(QString)));

            disconnect (m_dataReaderList[i], SIGNAL(sendHistoryData(QMap<QString, QList<QStringList>>)),
                        this,                SLOT(sendHistOrinPortfolioData_slot(QMap<QString, QList<QStringList>>)));
        }
    }    

    for (int i = 0; i < m_dataReaderThreadList.size(); ++i)
    {
        m_dataReaderThreadList[i]->quit ();
        m_dataReaderThreadList[i]->wait ();
    }

    for (int i = 0; i < m_dataReaderThreadList.size(); ++i)
    {
        if (NULL != m_dataReaderThreadList[i])
        {
            delete m_dataReaderThreadList[i];
            m_dataReaderThreadList[i] = NULL;
        }
    }

    m_dataReaderThreadList.clear();
    m_dataReaderList.clear();
}

void HistoryData::releaseCompPortfolioDataThreads () 
{
    for (int i = 0; i < m_dataProcessThreadList.size (); ++i) 
    {
        m_dataProcessThreadList[i]->quit ();
    }
}

void HistoryData::sendHistCompPortfolioData_slot (QList<QList<double>> allData) 
{
    if (m_isCSSChart)
    {
        QList<QString> timeList;
        
        QList<QList<double>> cssList;
        QList<QList<double>> aveList;
        for (int i = 0;i < allData[0].size(); ++i)
        {
            timeList.append(QDateTime::fromMSecsSinceEpoch(allData[0][i]).toString(m_timeFormat));
        }

        int index = 1;
        while(index < 4)
        {
            cssList.append(allData[index++]);
        }
        while (index < allData.size())
        {
            aveList.append(allData[index++]);
        }

        emit sendHistCSSData_signal(timeList, aveList, cssList, m_databaseID);
    }
    else
    {

        emit sendHistPortfolioData_Signal(allData);
    }

    releaseCompPortfolioDataThreads();
}

void HistoryData::sendRealtimeSpreadMACDData_slot(ChartData)
{

}

void HistoryData::sendRealtimeFutureData_slot(QList<double>)
{

}

// void HistoryData::computeHistPortData()
// {
//     if (m_isRealTime)
//     {
//         QMap<QString, QList<QStringList>> longHistOriData;
//         QMap<QString, QList<QStringList>> todyHistOriData;
//         seperateHistData(m_oriPortfilioData, longHistOriData, todyHistOriData);
//         checkRestoreData(longHistOriData, todyHistOriData);

//         QMap<QString, int> longHistPortfolio = transPorfolio(m_portfolio, "tinysoft");
//         QMap<QString, int> todyHistPortfolio = transPorfolio(m_portfolio, "wind");

//         filterRealtimeData(m_oneDayTimeList, todyHistOriData);
//         completeHistData(longHistOriData, todyHistOriData);

//         computeHistSpreadTimeList(longHistOriData, longHistPortfolio, 
//                                   m_hedgedCodeName,longHistPortfolio[m_hedgedCodeName], 
//                                   m_indexPriceMap[m_hedgedCodeName], 
//                                   m_timeData, m_hedgedData, m_indexCodeData);                        
//     }
//     else
//     {
//         QList<QString> suppleCodeList;
//         checkHistData(m_oriPortfilioData, m_endDate, suppleCodeList);

//         if (suppleCodeList.size() > 0)
//         {
//             m_isSupple = true;
//             emit getOrinPortfolioData_Signal("RealTimeData");
//         }
//         else
//         {
//             computeHistSpreadTimeList(m_oriPortfilioData, m_portfolio, 
//                                     m_hedgedCodeName,m_portfolio[m_hedgedCodeName], 
//                                     m_indexPriceMap[m_hedgedCodeName], 
//                                     m_timeData, m_hedgedData, m_indexCodeData);                 
//         }                     
//     }
// }

// void HistoryData::computeHistPortData()
// {
//     if (m_signalType == "All")
//     {
//         QMap<QString, QList<QStringList>> longHistOriData;
//         QMap<QString, QList<QStringList>> todyHistOriData;
//         seperateHistData(m_oriPortfilioData, longHistOriData, todyHistOriData);
//         checkRestoreData(longHistOriData, todyHistOriData);

//         //qDebug() << QString("longHistOriData.size: %1, todyHistOriData.size: %2")
//         //            .arg(longHistOriData.size()).arg(todyHistOriData.size());

//         QMap<QString, int> longHistPortfolio = transPorfolio(m_portfolio, "tinysoft");
//         QMap<QString, int> todyHistPortfolio = transPorfolio(m_portfolio, "wind");

//         filterRealtimeData(m_oneDayTimeList, todyHistOriData);
//         completeHistData(longHistOriData, todyHistOriData);

//         // printMap(longHistOriData, "longHistOriData");

//         // for (QMap<QString, QList<QStringList>>::iterator it = longHistOriData.begin(); 
//         //     it != longHistOriData.end(); ++it)
//         // {
//         //     qDebug() << it.key() << it.value().last();
//         // }

//         // printMap(todyHistOriData, "todyHistOriData");

//         computeHistSpreadTimeList(longHistOriData, longHistPortfolio, 
//                                   m_hedgedCodeName,longHistPortfolio[m_hedgedCodeName], 
//                                   m_indexPriceMap[m_hedgedCodeName], 
//                                   m_timeData, m_hedgedData, m_indexCodeData);           

//         // QList<QList<double>> realHedgedResult = computeSnapshootData(todyHistOriData, todyHistPortfolio, 
//         //                                                              getCompleteIndexCode(m_hedgedCodeName, "wind"), 
//         //                                                              m_indexPriceMap[m_hedgedCodeName], m_macdTime); 

//         // filterDataByTime(m_oneDayTimeList, realHedgedResult);  

//         // QList<double> tdyTimeData = realHedgedResult[0];
//         // QList<double> tdyHedgedData = realHedgedResult[1];
//         // QList<double> tdyIndexData = realHedgedResult[3];

//         // for (int i = 0; i < tdyTimeData.size(); ++i)
//         // {
//         //     qDebug() << QString("%1, %2, %3").arg(QDateTime::fromMSecsSinceEpoch(tdyTimeData[i]).toString(m_timeFormat))
//         //                                      .arg(tdyHedgedData[i])
//         //                                      .arg(tdyIndexData[i]);
//         // }

//         // if (tdyTimeData.size() > 0 && tdyTimeData.first() > m_timeData.last())
//         // {
//         //     m_timeData +=tdyTimeData;
//         //     m_hedgedData += tdyHedgedData;
//         //     m_indexCodeData += tdyIndexData; 
//         // }                           
//     }
//     else
//     {
//         computeHistSpreadTimeList(m_oriPortfilioData, m_portfolio, 
//                                   m_hedgedCodeName,m_portfolio[m_hedgedCodeName], 
//                                   m_indexPriceMap[m_hedgedCodeName], 
//                                   m_timeData, m_hedgedData, m_indexCodeData);                          
//     }
// }

// QMap<QString, QStringList> indexTimeValueMap = getHistIndexTimeValueMap(longHistOriData, getCompleteIndexCode(m_hedgedCodeName, "tinysoft"));
// QList<QList<double>> histHedgedResult = computeHistHedgedData(longHistOriData, indexTimeValueMap, 
//                                                               longHistPortfolio, getCompleteIndexCode(m_hedgedCodeName, "tinysoft"),
//                                                               m_indexPriceMap[m_hedgedCodeName]);                                                                       
// m_timeData = histHedgedResult[0];
// m_hedgedData = histHedgedResult[1];
// m_indexCodeData = histHedgedResult[2];  

// m_indexHedgeData = getHistIndexTimeValueMap(m_oriPortfilioData, getCompleteIndexCode(m_hedgedCodeName, "tinysoft"));
// QList<QList<double>> hedgedResult = computeHistHedgedData(m_oriPortfilioData, m_indexHedgeData, 
//                                                           m_portfolio, m_hedgedCodeName, 
//                                                           m_indexPriceMap[m_hedgedCodeName]);

// m_timeData = hedgedResult[0];
// m_hedgedData = hedgedResult[1];
// m_indexCodeData = hedgedResult[2];  

// void HistoryData::filterHedgeIndexData() 
// {
//     QList<QStringList> tmpIndexHedgeData = m_oriPortfilioData[m_hedgedCodeName];
//     m_oriPortfilioData.remove(m_hedgedCodeName);
//     for (int i = 0; i < tmpIndexHedgeData.size (); ++i) 
//     {
//         QStringList tmpKeyValue;
//         tmpKeyValue << tmpIndexHedgeData[i][1] << tmpIndexHedgeData[i][2];
//         QString dateTimeString = QDateTime::fromMSecsSinceEpoch(qint64(tmpIndexHedgeData[i][0].toLongLong())).toString ("yyyyMMddhhmmss");
//         m_indexHedgeData.insert(dateTimeString, tmpKeyValue);
//     }
// }

// void computeHedgedData();
// void computeVotData();
// void computeMacdData();

// void HistoryData::computeHedgedData () 
// {
//     QList<QPointF> pointDataList;
//     QList<QList<double>> hedgedResult;

//     if (m_isBuySalePortfolio) 
//     {
//         QList<QPointF> buyPointDataList = getStrategyPointList(m_oriPortfilioData, m_buyPortfolio);
//         QList<QPointF> salePointDataList = getStrategyPointList(m_oriPortfilioData, m_salePortfolio);
//         hedgedResult = getHedgedData(buyPointDataList, salePointDataList);
//     } 
//     else 
//     {
//         filterHedgeIndexData();
//         QMap<QString, int> seocdebuyCountMap = m_portfolio;
//         seocdebuyCountMap.remove(m_hedgedCodeName);
//         pointDataList = getStrategyPointList(m_oriPortfilioData, seocdebuyCountMap);
//         hedgedResult = getHedgedData(pointDataList, m_indexHedgeData,
//                                      m_portfolio[m_hedgedCodeName],
//                                      m_indexPriceMap[m_hedgedCodeName]);
//     }

//     m_timeData = hedgedResult[0];
//     m_hedgedData = hedgedResult[1];
//     m_indexCodeData = hedgedResult[2];
// }

// void  HistoryData::computeVotData () 
// {
//     QList<QString> keyList = m_oriPortfilioData.keys ();
//     QList<QPointF> pointDataList;
//     for (int i = 0; i < keyList.size (); ++i) {
//         QString key = keyList[i];
//         QList<QStringList> tmpStringList = m_oriPortfilioData[key];
//         QList<QPointF> tmpPointData;
//         for (int i = 0; i < tmpStringList.size (); ++i) {
//             tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(), tmpStringList[i][2].toDouble()));
//         }
//         tmpPointData = sortPointFList(tmpPointData);
//         pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
//     }

//     if (m_timeData.size() == 0) {
//         for (int i = 0; i < pointDataList.size(); ++i) {
//             m_timeData.append(pointDataList[i].x());
//         }
//     }

//     for (int i = 0; i < pointDataList.size(); ++i) {
//         m_votData.append(pointDataList[i].y());
//     }
// }

// void  HistoryData::computeMacdData () 
// {
//     m_macdData = computeMACDDoubleData(m_hedgedData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
// }


// if (m_isCSSChart)
// {
//     if (m_isRealTime)
//     {
//         m_curSelectData = m_hedgedData.last();
//         m_oldSelectData = m_hedgedData.last();
//         m_curHedgedData = m_indexCodeData.last();
//         m_oldHedgedData = m_indexCodeData.last();            
//     }

//     for (int i = 0; i < m_timeData.size(); ++i)
//     {
//         m_timeList.append(QDateTime::fromMSecsSinceEpoch(m_timeData[i]).toString(m_timeFormat));
//     }

//     getTypEarningList(m_hedgedData, m_indexCodeData, m_oriTypeList, m_earningList);

//     getTransedTYP(m_oriTypeList, m_typList);

//     computeHistoryCSSData();                        
// }

// void HistoryData::computePortfolioData () 
// {
//     bool isNewType = true;
//     if (isNewType)
//     {
//         if (m_isCSSChart)
//         {
//             computeHistPortData();
            
//             m_curSelectData = m_hedgedData.last();
//             m_oldSelectData = m_hedgedData.last();            
//             m_curHedgedData = m_indexCodeData.last();
//             m_oldHedgedData = m_indexCodeData.last();

//             transEpochTimeList(m_timeList, m_timeData, m_timeFormat);

//             getTypEarningList(m_hedgedData, m_indexCodeData, m_oriTypeList, m_earningList);

//             getTransedTYP(m_oriTypeList, m_typList);

//             computeHistoryCSSData();
//         }
//         else
//         {
//             QList<QList<double>> allData;
//             if (m_isRealTime)
//             {
//                 if (m_isBuySalePortfolio)
//                 {
//                     allData = computeSnapshootData(m_oriPortfilioData, m_buyPortfolio, m_salePortfolio, m_macdTime);                    
//                 }
//                 else
//                 {
//                     allData = computeSnapshootData(m_oriPortfilioData, m_portfolio, m_hedgedCodeName, 
//                                                    m_indexPriceMap[m_hedgedCodeName], m_macdTime);
//                 }                
//             }
//             else
//             {
//                 if (m_isBuySalePortfolio)
//                 {
//                     computeHistSpreadTimeList(m_oriPortfilioData, m_buyPortfolio,
//                                               m_salePortfolio, m_timeData, m_hedgedData);
//                 }
//                 else
//                 {
//                     computeHistSpreadTimeList(m_oriPortfilioData, m_portfolio, 
//                                               m_hedgedCodeName,m_portfolio[m_hedgedCodeName], 
//                                               m_indexPriceMap[m_hedgedCodeName], 
//                                               m_timeData, m_hedgedData, m_indexCodeData);              
//                 }

//                 computeVotList(m_oriPortfilioData, m_votData);
//                 computeMACDList(m_hedgedData, m_macdTime, m_macdData);
                
//                 allData.append(m_timeData);
//                 allData.append(m_hedgedData);
//                 allData.append(m_votData);
//                 allData.append(m_macdData);
//                 allData.append(m_indexCodeData);                
//             }
//             emit sendHistPortfolioData_Signal(allData);
//         }
//     }
//     else
//     {
//         setCompPortfolioDataThreads();
//         emit startCompPortfolioData_Signal("all");
//         emit sendTableViewInfo_Signal("开始计算历史数据");
//     }
// }
