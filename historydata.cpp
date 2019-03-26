#include <cstdlib>
#include <QString>
#include <QStringList>

#include <iostream>
#include <exception>

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


HistoryData::HistoryData(DatabaseParam dbParam, 
                         HedgedParam hedgedParam,                           
                         CSSParam cssParam,  
                         int dataID,
                         QObject *parent):
                         m_hedgedParam(hedgedParam),
                         m_dbParam(dbParam),
                         m_cssParam(cssParam),
                         m_dataID(dataID),
                         m_isInit(false),
                         QObject(parent)
{

}    

HistoryData::HistoryData(QString dbhost, 
                         QStringList timeTypeList, 
                         QObject *parent): 
                         m_dbParam(dbhost), 
                         m_timeTypeList(timeTypeList), 
                         QObject(parent)
{

}

HistoryData::~HistoryData()
{
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

    releaseMonitorWorker();

    if (m_hedgedParam.m_isRealTime)
    {
        // m_realtimeThread.quit();
        m_realtimeThread.terminate();
        // m_realtimeThread.wait();
    }
}

void HistoryData::initAll(int type)
{
    if (m_isInit == true)
    {
        return;
    }

    m_isInit = true;
    if (type == 1)
    {
        initCommonData();
    }

    if (type == 2)
    {
        initDatabase();        
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
    m_realtimeWorker = NULL;

    m_keyValueList.clear();
    m_keyValueList << "TCLOSE" << "VOTRUNOVER" << "PCTCHG";

    if (m_hedgedParam.m_oriCode.size() == 0 && m_hedgedParam.m_hedgedCode.size() != 0)
    {
        m_hedgedParam.m_oriCode = m_hedgedParam.m_hedgedCode;
    }

    if (m_hedgedParam.m_isRealTime)
    {
        m_oneDayTimeList = getOneDayTimeList(m_dbParam.m_dbhost, m_dbParam.m_databaseName);
    }

    initDatabase();
    initTimeFormat();

    if (m_hedgedParam.m_isPortfolio)
    {
        m_threadNumb = m_hedgedParam.m_codeList.size();

        initPortfolioSignalType();
        initPortfolioThreadSecodeList();
        setReadPortfolioDataThreads();
        initPortfolioIndexHedgeMetaInfo();

        if (m_hedgedParam.m_hedgedType == CLVALUE_WEIGHT_EARNING 
         || m_hedgedParam.m_hedgedType == CLVALUE_WEIGHT_HEDGE)
        {
            transPortfolio();
        }
    }

    if (m_hedgedParam.m_isRealTime)
    {
        initRealtimeWorker();
    }
}

void HistoryData::initTimeFormat()
{
    if (!isMinuteType(m_dbParam.m_databaseName))
    {
        m_timeFormat = "yyyyMMdd";
        m_timeType = "day";
    }
    else
    {
        m_timeFormat = "yyyyMMdd hhmm";
        m_timeType = (m_dbParam.m_databaseName.split("_"))[1];
        m_timeType.remove(m_timeType.size() - 1, 1);
    }
}

void HistoryData::initDatabase()
{
    m_database = new Database(m_dbParam.m_dbhost);
}

void HistoryData::initRealtimeWorker()
{
    int monitorTime;
    if (m_hedgedParam.m_isCSSChart)
    {
        monitorTime = 30 * 1000;
    }
    else
    {
        monitorTime = 5 * 1000;
    }

    m_realtimeWorker = new MonitorRealTimeData(m_dbParam, m_hedgedParam);

    if (m_hedgedParam.m_isPortfolio)
    {
        if (m_hedgedParam.m_isCSSChart)
        {
            connect(m_realtimeWorker, SIGNAL(sendRealtimeSpreadData_signal(QList<double>, QMap<QString, double>)),
                    this,             SLOT(sendRealtimeSpreadData_slot(QList<double>, QMap<QString, double>)));
        }
        else
        {
            connect(this,             SIGNAL(getPreCloseData_signal()),
                    m_realtimeWorker, SLOT(getPreCloseData_slot()));

            connect(m_realtimeWorker, SIGNAL(sendRealtimeSpreadMACDData_signal(QList<double>, QMap<QString, double>)),
                    this,             SLOT(sendRealtimeSpreadMACDData_slot(QList<double>, QMap<QString, double>)));

            connect(m_realtimeWorker, SIGNAL(sendPreCloseData_signal(double)),
                    this,             SLOT(sendPreCloseData_slot(double)));
        }
    }

    if (m_hedgedParam.m_isIndex)
    {
        connect(m_realtimeWorker, SIGNAL(sendRealtimeIndexData_signal(QList<double>)),
                this,             SLOT(sendRealtimeIndexData_slot(QList<double>)));
    }

    connect(this,             SIGNAL(getRealtimeData_signal()),
            m_realtimeWorker, SLOT(getRealtimeData_slot()));

    connect(this,             SIGNAL(processRealtimeDataComplete_signal()),
            m_realtimeWorker, SLOT(processRealtimeDataComplete_slot()));

    connect(&m_realtimeThread, SIGNAL(finished()),
            m_realtimeWorker,  SLOT(deleteLater()));

    m_realtimeWorker->moveToThread(&m_realtimeThread);

    m_realtimeThread.start();
}

void HistoryData::releaseMonitorWorker()
{
    if (m_hedgedParam.m_isRealTime && NULL != m_realtimeWorker)
    {
        disconnect(this, SIGNAL(getRealtimeData_signal()),
                   m_realtimeWorker, SLOT(getRealtimeData_slot()));

        disconnect(this, SIGNAL(processRealtimeDataComplete_signal()),
                   m_realtimeWorker, SLOT(processRealtimeDataComplete_slot()));
    }
}

void HistoryData::initPortfolioSignalType()
{
    if (m_hedgedParam.m_isCSSChart)
    {
        m_signalType = "HistoryData";
    }
    else
    {
        if (m_hedgedParam.m_isRealTime)
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
    allocateThreadCodeList(m_threadNumb, m_hedgedParam.m_codeList, m_threadSecodeList);
    // printList(m_threadSecodeList, "m_threadSecodeList");
}

void HistoryData::initPortfolioIndexHedgeMetaInfo()
{
    m_indexPriceMap.insert("SH000300", 300);
    m_indexPriceMap.insert("SH000016", 300); // 上证50
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

    for (int i = 0; i < m_threadSecodeList.size(); ++i)
    {

        DataRead *curDataReader = new DataRead(m_dbParam.m_dbhost, m_dbParam.m_databaseName, m_threadSecodeList[i],
                                               m_hedgedParam.m_startDate, m_hedgedParam.m_endDate, m_keyValueList);

        QThread *curThread = new QThread();
        curDataReader->moveToThread(curThread);

        connect(curThread, SIGNAL(finished()),
                curDataReader, SLOT(deleteLater()));

        connect(this, SIGNAL(getHistPortfolioData_Signal(QString)),
                curDataReader, SLOT(getHistPortfolioData_slot(QString)));

        connect(curDataReader, SIGNAL(sendHistPortfolioData_signal(QMap<QString, QList<QStringList>>)),
                this, SLOT(sendHistPortfolioData_slot(QMap<QString, QList<QStringList>>)),
                Qt::QueuedConnection);

        curThread->start();
        m_dataReaderList.append(curDataReader);
        m_dataReaderThreadList.append(curThread);
    }
}

void HistoryData::transPortfolio()
{
    Database dbObj(m_dbParam.m_dbhost);
    double sumMarketValue = 0;

    // printMap(m_hedgedParam.m_oriPortfolio, "m_hedgedParam.m_oriPortfolio"); 
    // printMap(m_hedgedParam.m_hedgedPortfolio, "m_hedgedParam.m_hedgedPortfolio");  

    for (QMap<QString, double>::iterator it = m_hedgedParam.m_oriPortfolio.begin();
        it != m_hedgedParam.m_oriPortfolio.end(); ++it)
    {
        if (it.key() != m_hedgedParam.m_hedgedCode)
        {
            double closePrice = dbObj.getClosePrice(getCompleteSecode(it.key(), "tinysoft"), m_hedgedParam.m_weightDate);
            sumMarketValue += closePrice * it.value();
            it.value() = closePrice * it.value();
        }
    }

    for (QMap<QString, double>::iterator it = m_hedgedParam.m_oriPortfolio.begin();
        it != m_hedgedParam.m_oriPortfolio.end(); ++it)
    {
        if (it.key() != m_hedgedParam.m_hedgedCode)
        {            
            it.value() = it.value() / sumMarketValue;
        }
    }

    sumMarketValue = 0;
    for (QMap<QString, double>::iterator it = m_hedgedParam.m_hedgedPortfolio.begin();
        it != m_hedgedParam.m_hedgedPortfolio.end(); ++it)
    {
        double closePrice = dbObj.getClosePrice(getCompleteSecode(it.key(), "tinysoft"), m_hedgedParam.m_weightDate);
        sumMarketValue += closePrice * it.value();    
        it.value() = closePrice * it.value();
    }    

    for (QMap<QString, double>::iterator it = m_hedgedParam.m_hedgedPortfolio.begin();
        it != m_hedgedParam.m_hedgedPortfolio.end(); ++it)
    {   
        it.value() = it.value() / sumMarketValue;
    }     

    // printMap(m_hedgedParam.m_oriPortfolio, "m_hedgedParam.m_oriPortfolio"); 
    // printMap(m_hedgedParam.m_hedgedPortfolio, "m_hedgedParam.m_hedgedPortfolio");  
}

void HistoryData::getTableList_slot()
{
    initAll(2);
    for (int i = 0; i < m_timeTypeList.size(); ++i)
    {
        QString databaseName = QString("MarketData_%1").arg(m_timeTypeList[i]);
        QStringList result = m_database->getTableList(databaseName);
        emit sendTableList_signal(m_timeTypeList[i], result);
    }
}

void HistoryData::getPreCloseData_slot()
{
    initAll(1);
    emit getPreCloseData_signal();
}

void HistoryData::getHistPortfolioData_slot()
{
    initAll(1);

    emit getHistPortfolioData_Signal(m_signalType);
}

void HistoryData::getHistIndexData_slot()
{
    initAll(1);
    QStringList keyList;
    keyList << "TDATE"
            << "TIME"
            << "TCLOSE"
            << "TOPEN"
            << "PCTCHG"
            << "VATRUNOVER";

    QList<QStringList> selectDBData = m_database->getDataByDate(m_hedgedParam.m_startDate, m_hedgedParam.m_endDate, keyList,
                                                                m_hedgedParam.m_oriCode, m_dbParam.m_databaseName);
    int secStartPos = getDelayStartDate(selectDBData);
    selectDBData = getSubList(selectDBData, secStartPos, selectDBData.size());

    if (m_hedgedParam.m_isRealTime)
    {
        completeIndexRealtimeData(selectDBData, m_hedgedParam.m_oriCode);
    }

    m_oldSelectData = selectDBData.last()[2].toDouble();
    m_curSelectData = selectDBData.last()[2].toDouble();

    if (m_hedgedParam.m_hedgedCode.size() != 0)
    {
        QList<QStringList> hedgedDBData = m_database->getDataByDate(m_hedgedParam.m_startDate, m_hedgedParam.m_endDate, keyList,
                                                                    m_hedgedParam.m_hedgedCode, m_dbParam.m_databaseName);

        int hedStartPos = getDelayStartDate(hedgedDBData);
        hedgedDBData = getSubList(hedgedDBData, hedStartPos, hedgedDBData.size());

        if (m_hedgedParam.m_isRealTime)
        {
            completeIndexRealtimeData(hedgedDBData, m_hedgedParam.m_hedgedCode);
        }

        if (m_hedgedParam.m_hedgedCode == m_hedgedParam.m_oriCode)
        {
            zeroHistData(selectDBData);
        }

        getTypEarningList(selectDBData, hedgedDBData, m_timeFormat,
                          m_timeList, m_oriTypeList, m_earningList);

        getTransTypList(m_oriTypeList, m_typList);

        m_oldHedgedData = hedgedDBData.last()[2].toDouble();
        m_curHedgedData = hedgedDBData.last()[2].toDouble();
    }
    else
    {
        getTypEarningList(selectDBData, m_timeFormat, m_timeList,
                          m_oriTypeList, m_earningList);
        getTransTypList(m_oriTypeList, m_typList);
    }

    computeHistoryCSSData();
}

void HistoryData::getRealtimeData_slot()
{
    initAll(1);
    emit getRealtimeData_signal();
}

void HistoryData::sendPreCloseData_slot(double preCloseData)
{
    // qDebug() << "HistoryData::sendPreCloseData_slot: " << preCloseData;
    emit sendPreCloseData_signal(preCloseData);
}

void HistoryData::sendHistPortfolioData_slot(QMap<QString, QList<QStringList>> subThreadData)
{
    QMutexLocker locker(&m_mutex);
    m_oriHistPortfolioData.unite(subThreadData);
    ++m_readDataThreadCount;

    emit sendTableViewInfo_Signal(QString("完成股票指数读取数目: %1").arg(m_oriHistPortfolioData.size()));

    if (m_readDataThreadCount == m_threadNumb)
    {
        m_readDataThreadCount = 0;
        if (m_oriHistPortfolioData.find("Error") != m_oriHistPortfolioData.end())
        {
            emit sendTableViewInfo_Signal(QString("Error: 读取数据出错"));
        }
        else if (m_isSupple)
        {
            m_suppledPortfolioData = m_oriHistPortfolioData;
            m_oriHistPortfolioData.clear();
            releaseReadPortfolioDataThreads();

            completeSupplePortfolioData();
            computePortfolioHistCSSData();
        }
        else
        {
            m_oriPortfilioData = m_oriHistPortfolioData;
            m_oriHistPortfolioData.clear();
            releaseReadPortfolioDataThreads();

            computePortfolioData();
        }
    }
}

void HistoryData::sendRealtimeIndexData_slot(QList<double> indexData)
{
    if (m_hedgedParam.m_isCSSChart)
    {
        if (m_hedgedParam.m_hedgedCode.size() == 0)
        {
            m_curSelectData = indexData[0];
            double curChange = (m_curSelectData - m_oldSelectData) / m_oldSelectData;
            m_earningList.append(m_earningList.last() * (1 + curChange));
            m_oriTypeList.append((m_earningList.last() + m_earningList[m_earningList.size() - 2]) / 2);
            
            // qDebug() << QString("Close: %1, Open: %2, Time: %3")
            //             .arg(closePrice).arg(indexData[1])
            //             .arg(QDateTime::fromMSecsSinceEpoch(indexData[2])
            //                  .toString("yyyy.MM.dd hh:mm:ss")); 
        }
        else
        {
            // qDebug() << QString("%1: %2, %3: %4, Time: %5")
            //             .arg(m_hedgedParam.m_oriCode).arg(indexData[0])
            //             .arg(m_hedgedParam.m_hedgedCode).arg(indexData[1])
            //             .arg(QDateTime::fromMSecsSinceEpoch(indexData[2])
            //             .toString("yyyy.MM.dd hh:mm:ss"));

            m_curSelectData = indexData[0];
            m_curHedgedData = indexData[1];
            double curSelectChange = (m_curSelectData - m_oldSelectData) / m_oldSelectData;
            double curHedgedChange = (m_curHedgedData - m_oldHedgedData) / m_oldHedgedData;

            if (m_hedgedParam.m_oriCode == m_hedgedParam.m_hedgedCode)
            {
                curSelectChange = 0;
            }

            double relativeEarning = curSelectChange - curHedgedChange;

            m_earningList.append(m_earningList.last() * (1 + relativeEarning));
            m_oriTypeList.append((m_earningList.last() + m_earningList[m_earningList.size() - 2]) / 2);
        }

        m_curEpochRealtime = indexData[2];

        computeRealtimeCSSData();
    }
    else
    {
    }
}

void HistoryData::sendRealtimeSpreadData_slot(QList<double> oridata, QMap<QString, double> lastCodeClose)
{
    try
    {
        m_curSelectData = oridata[0];
        m_curHedgedData = oridata[2];
        m_curCodeClose  = lastCodeClose;

        computeCurEarning();

        m_oriTypeList.append((m_earningList.last() + m_earningList[m_earningList.size() - 2]) / 2);

        m_curEpochRealtime = oridata[1];

        computeRealtimeCSSData();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void HistoryData::sendRealtimeSpreadMACDData_slot(QList<double> oriData, QMap<QString, double> lastCodeClose)
{
    try
    {
        m_curSelectData = oriData[0];
        m_curHedgedData = oriData.last();
        m_curCodeClose  = lastCodeClose;

        double votData  = oriData[1];
        double timeData = oriData[2];

        computeCurEarning();
        computeCurMACD();

        qDebug() << m_earningList.last() << votData << timeData <<  m_curHedgedData;

        ChartData curChartData(m_earningList.last(), votData, timeData,
                            m_MACDData.last(), m_curHedgedData);

        m_oldSelectData = m_curSelectData;
        m_oldHedgedData = m_curHedgedData;
        emit sendRealtimeSpreadMACDData_signal(curChartData);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void HistoryData::processRealtimeDataComplete_slot()
{
    emit processRealtimeDataComplete_signal();
}

void HistoryData::computeCurEarning()
{
    double curEarning = 1;

    if (m_hedgedParam.m_hedgedType == CLVALUE_WEIGHT_EARNING 
    && m_earningList.size() > 0)
    {
        if (m_oldCodeClose.size() > 0)
        {
            double chg = 0;
            for (QMap<QString ,double>::const_iterator it = m_curCodeClose.begin();
                it != m_curCodeClose.end(); ++it)
            {
                double curChg = (it.value() - m_oldCodeClose[it.key()]) / m_oldCodeClose[it.key()];
                chg += curChg * m_hedgedParam.m_oriPortfolio[it.key()];
            }
            curEarning = m_earningList.last() * (1 + chg);                 
        }
    }

    if (m_hedgedParam.m_hedgedType == MKVALUE_WEIGHT_EARNING
    && m_earningList.size() > 0)
    {
        double chg = (m_curSelectData - m_oldSelectData) / m_oldSelectData;
        curEarning = m_earningList.last()*(1+chg);
    }

    if (m_hedgedParam.m_hedgedType == CLVALUE_WEIGHT_HEDGE
    &&  m_earningList.size() > 0)
    {
        if (m_oldCodeClose.size() > 0)
        {
            // printMap(m_oldCodeClose, "m_oldCodeClose");
            // printMap(m_curCodeClose, "m_curCodeClose");

            double selectChg = 0;
            double hedgedChg = 0;
            for (QMap<QString ,double>::const_iterator it = m_curCodeClose.begin();
                it != m_curCodeClose.end(); ++it)
            {
                if (it.key() == m_hedgedParam.m_hedgedCode) continue;

                double curChg = (it.value() - m_oldCodeClose[it.key()]) / m_oldCodeClose[it.key()];
                
                if (m_hedgedParam.m_oriPortfolio.find(it.key()) != m_hedgedParam.m_oriPortfolio.end())
                {
                    selectChg += curChg * m_hedgedParam.m_oriPortfolio[it.key()];

                    // qDebug() << it.key() << m_oldCodeClose[it.key()] << it.value() <<  curChg << m_hedgedParam.m_oriPortfolio[it.key()]
                    //          << selectChg;
                }

                if (m_hedgedParam.m_isPortfolioHedge)
                {
                    if (m_hedgedParam.m_hedgedPortfolio.find(it.key()) != m_hedgedParam.m_hedgedPortfolio.end())
                    {
                        hedgedChg += curChg * m_hedgedParam.m_hedgedPortfolio[it.key()];
                    }   
                }         
            }

            if (m_hedgedParam.m_isPortfolioHedge == false 
             && m_curCodeClose.find(m_hedgedParam.m_hedgedCode) != m_curCodeClose.end())
            {
                hedgedChg = (m_curCodeClose[m_hedgedParam.m_hedgedCode] - m_oldCodeClose[m_hedgedParam.m_hedgedCode])
                          / m_oldCodeClose[m_hedgedParam.m_hedgedCode];
            }

            double relativeProfit = selectChg - hedgedChg;
            curEarning = m_earningList.last() * (1 + relativeProfit);

            // qDebug() << "m_earningList.last(): " << m_earningList.last() << " curEarning: " << curEarning
            //          << "selectChg: " << selectChg << " hedgedChg:" << hedgedChg;            
        }
    }

    if (m_hedgedParam.m_hedgedType == MKVALUE_WEIGHT_HEDGE
     && m_earningList.size() > 0)
    {
        double selectChg = (m_curSelectData - m_oldSelectData) / m_oldSelectData;
        double hedgedChg = (m_curHedgedData - m_oldHedgedData) / m_oldHedgedData;
        double relativeProfit = selectChg - hedgedChg;
        curEarning = m_earningList.last()*(1+relativeProfit);
    }

    if (m_hedgedParam.m_hedgedType >= SPREAD_HEDGE)
    {
        if (m_hedgedParam.m_isPortfolioHedge)
        {
            curEarning = m_curSelectData - m_curHedgedData;
        }
        else if (m_hedgedParam.m_isCSSChart && m_earningList.size() > 0)
        {
            double oldSpread = m_oldSelectData / (m_hedgedParam.m_oriPortfolio[m_hedgedParam.m_hedgedCode] * m_indexPriceMap[m_hedgedParam.m_hedgedCode]) - m_oldHedgedData;

            double curSpread = m_curSelectData / (m_hedgedParam.m_oriPortfolio[m_hedgedParam.m_hedgedCode] * m_indexPriceMap[m_hedgedParam.m_hedgedCode]) - m_curHedgedData;

            double chg = (curSpread - oldSpread) / m_oldHedgedData;
            curEarning = m_earningList.last() * (1 + chg);
        }
        else
        {
            curEarning = m_curSelectData / (m_hedgedParam.m_oriPortfolio[m_hedgedParam.m_hedgedCode] * m_indexPriceMap[m_hedgedParam.m_hedgedCode]) 
                       - m_curHedgedData;
        }
    }

    if (!m_hedgedParam.m_isCSSChart)
    {
        m_oldCodeClose  = m_curCodeClose;
    }
    
    m_earningList.append(curEarning);
}

void HistoryData::computeCurMACD()
{
    if (m_MACDData.size() > 0)
    {
        m_MACDData.append(computeMACDData(m_earningList.last(), m_MACDData.last(),
                                          m_hedgedParam.m_macdTime[0], 
                                          m_hedgedParam.m_macdTime[1], 
                                          m_hedgedParam.m_macdTime[2]));
    }
    else
    {
        m_MACDData.append(MACD(m_earningList.last(), m_earningList.last(), 0, 0, 0));
    }
}

void HistoryData::computePortfolioData()
{
    if (m_hedgedParam.m_isCSSChart)
    {
        checkSuppleData();
        if (!m_isSupple)
        {
            computePortfolioHistCSSData();
        }
    }
    else
    {
        if (m_hedgedParam.m_isRealTime)
        {
            if (m_hedgedParam.m_isPortfolioHedge)
            {
                computeSnapshootData(m_oriPortfilioData, m_hedgedParam.m_oriPortfolio,
                                     m_hedgedParam.m_hedgedPortfolio, m_hedgedParam.m_macdTime,
                                     m_timeData, m_earningList, m_votData,
                                     m_macdData, m_indexCodeData,
                                     m_curSelectData, m_curHedgedData,
                                     m_curCodeClose,
                                     m_hedgedParam.m_hedgedType);
            }
            else
            {
                computeSnapshootData(m_oriPortfilioData, m_hedgedParam.m_oriPortfolio, m_hedgedParam.m_hedgedCode,
                                     m_hedgedParam.m_oriPortfolio[m_hedgedParam.m_hedgedCode],
                                     m_indexPriceMap[m_hedgedParam.m_hedgedCode], m_hedgedParam.m_macdTime,
                                     m_timeData, m_earningList, m_votData,
                                     m_macdData, m_indexCodeData,
                                     m_curSelectData, m_curHedgedData,
                                     m_curCodeClose);
            }
        }
        else
        {
            if (m_hedgedParam.m_isPortfolioHedge)
            {
                computeHistSpreadTimeList(m_oriPortfilioData, m_hedgedParam.m_oriPortfolio,
                                          m_hedgedParam.m_hedgedPortfolio, m_timeData, m_earningList,
                                          m_curSelectData, m_curHedgedData, m_curCodeClose,
                                          m_hedgedParam.m_hedgedType);
            }
            else
            {
                computeHistSpreadTimeList(m_oriPortfilioData, m_hedgedParam.m_oriPortfolio,
                                          m_hedgedParam.m_hedgedCode, m_hedgedParam.m_hedgedType,
                                          m_indexPriceMap[m_hedgedParam.m_hedgedCode],
                                          m_timeData, m_earningList, m_indexCodeData,
                                          m_curSelectData, m_curHedgedData,
                                          m_curCodeClose);

                m_oriPortfilioData.remove(m_hedgedParam.m_hedgedCode);                                           
            }
                       
            computeVotList(m_oriPortfilioData, m_votData);
            computeMACDList(m_earningList, m_hedgedParam.m_macdTime, m_macdData);

            // qDebug() << m_timeData.size() << m_earningList.size() << m_votData.size();
        }

        m_oldCodeClose  = m_curCodeClose;
        m_oldSelectData = m_curSelectData;
        m_oldHedgedData = m_curHedgedData;

        // qDebug() << QString("oldSelectData:%1, curSelectData:%2, oldHedgedData:%3, curHedgedData:%4, lastEarning: %5")
        //                 .arg(m_oldSelectData)
        //                 .arg(m_curSelectData)
        //                 .arg(m_oldHedgedData)
        //                 .arg(m_curHedgedData)
        //                 .arg(m_earningList.last());

        QList<QList<double>> allData;
        allData.append(m_timeData);
        allData.append(m_earningList);
        allData.append(m_votData);
        allData.append(m_macdData);
        allData.append(m_indexCodeData);

        emit sendHistPortfolioData_Signal(allData);
    }
}

void HistoryData::checkSuppleData()
{
    if (m_hedgedParam.m_isRealTime && !isStockTradingNotStart())
    {
        QList<QString> suppleCodeList;
        checkHistData(m_oriPortfilioData, m_hedgedParam.m_endDate, suppleCodeList);

        if (suppleCodeList.size() > 0 )
        {
            m_isSupple = true;
            m_threadNumb = suppleCodeList.size();
            allocateThreadCodeList(m_threadNumb, suppleCodeList, m_threadSecodeList);
            setReadPortfolioDataThreads();
            emit getHistPortfolioData_Signal("RealTimeData");
        }
    }
}

void HistoryData::computePortfolioHistCSSData()
{
    if (m_hedgedParam.m_isPortfolioHedge)
    {
        computeHistSpreadTimeList(m_oriPortfilioData, m_hedgedParam.m_oriPortfolio,
                                  m_hedgedParam.m_hedgedPortfolio, m_timeData, m_earningList,
                                  m_curSelectData, m_curHedgedData,m_curCodeClose,
                                  m_hedgedParam.m_hedgedType);
    }
    else
    {
        computeHistSpreadTimeList(m_oriPortfilioData, m_hedgedParam.m_oriPortfolio,
                                  m_hedgedParam.m_hedgedCode, m_hedgedParam.m_hedgedType,
                                  m_indexPriceMap[m_hedgedParam.m_hedgedCode],
                                  m_timeData, m_hedgedData, m_indexCodeData,
                                  m_curSelectData, m_curHedgedData, m_curCodeClose);

        getEarningList(m_hedgedData, m_indexCodeData, m_earningList, m_hedgedParam.m_hedgedType);
    }

    m_oldCodeClose  = m_curCodeClose;
    m_oldSelectData = m_curSelectData;
    m_oldHedgedData = m_curHedgedData;

    getOriTypList(m_earningList, m_oriTypeList);

    getTransTypList(m_oriTypeList, m_typList);

    transEpochTimeList(m_timeList, m_timeData, m_timeFormat);

    computeHistoryCSSData();
}

void HistoryData::completeSupplePortfolioData()
{
    checkRestoreData(m_oriPortfilioData, m_suppledPortfolioData);

    QMap<QString, double> longHistPortfolio = transPorfolio(m_hedgedParam.m_oriPortfolio, "tinysoft");
    QMap<QString, double> todyHistPortfolio = transPorfolio(m_hedgedParam.m_oriPortfolio, "wind");

    filterRealtimeData(m_oneDayTimeList, m_suppledPortfolioData);
    completeHistData(m_oriPortfilioData, m_suppledPortfolioData);
}

void HistoryData::completeIndexRealtimeData(QList<QStringList> &oriData, QString codeName)
{
    if (isStockTradingNotStart() || QDate::currentDate() != getRealtimeDate(m_dbParam.m_dbhost))
    {
        return;
    }

    Database dbObj(m_dbParam.m_dbhost);
    QList<QStringList> tdyRealtimeData = dbObj.getSnapShootHistoryData(getCompleteIndexCode(codeName, "wind"));
    if (tdyRealtimeData.size() > 1)
    {
        QList<int> onedayTimeList = getOneDayTimeList(m_dbParam.m_dbhost, m_dbParam.m_databaseName);
        QList<QStringList> fileteredList;
        int onedayTimeListStartIndex = 0;
        for (int i = 1; i < tdyRealtimeData.size(); ++i)
        {
            int oldTime = tdyRealtimeData[i - 1].last().toLongLong() % 1000000;
            int curTime = tdyRealtimeData[i].last().toLongLong() % 1000000;

            for (int j = onedayTimeListStartIndex; j < onedayTimeList.size() - 1; ++j)
            {
                if (oldTime == onedayTimeList[j])
                {
                    QStringList addedData = tdyRealtimeData[i - 1];
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
            QStringList addedData = tdyRealtimeData.last();
            addedData[1] = QString("%1").arg(onedayTimeList.last());
            fileteredList.append(addedData);
        }

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
    computeAVEList(m_earningList, m_aveList, m_cssParam.m_aveNumbList, m_cssParam.m_isEMAList);

    QList<double> mainList = computeCSSList(m_typList, m_curMainCSST, m_curMainCSSA,
                                            m_cssParam.m_mainAveNumb, m_cssParam.m_css12Rate,
                                            m_cssParam.m_mainCssRate1, m_cssParam.m_mainCssRate2,
                                            m_cssParam.m_maxCSS, m_cssParam.m_minCSS, false);

    QList<double> subValueList = computeCSSList(m_typList, m_curSubCSST, m_curSubCSSA,
                                                m_cssParam.m_subAveNumb, m_cssParam.m_css12Rate,
                                                m_cssParam.m_mainCssRate1, m_cssParam.m_mainCssRate2,
                                                m_cssParam.m_maxCSS, m_cssParam.m_minCSS, false);

    QList<double> energyValueList = computeCSSList(m_typList, m_curEnergyCSST, m_curEnergyCSSA,
                                                   m_cssParam.m_energyAveNumb, m_cssParam.m_css12Rate,
                                                   m_cssParam.m_energyCssRate1, m_cssParam.m_energyCssRate2,
                                                   m_cssParam.m_maxCSS, m_cssParam.m_minCSS, true);

    QList<QList<double>> cssList;
    cssList.append(mainList);
    cssList.append(subValueList);
    cssList.append(energyValueList);

    QList<QList<double>> aveList = m_aveList;
    aveList.insert(0, m_earningList);

    if (m_hedgedParam.m_isPortfolio)
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

    // qDebug() << QString("oldSelectData:%1,  oldHedgedData:%3, curEarning: %5,")
    //                 .arg(m_oldSelectData)
    //                 .arg(m_oldHedgedData)
    //                 .arg(m_earningList.last());

    emit sendHistCSSData_signal(m_timeList, aveList, cssList, m_dataID);
}

void HistoryData::computeRealtimeCSSData()
{
    m_typList.append((m_oriTypeList.last() + m_oriTypeList[m_oriTypeList.size() - 2]) / 2);

    computeAVEAtom(m_earningList, m_earningList.size() - 1, m_aveList, m_cssParam.m_aveNumbList, m_cssParam.m_isEMAList);

    m_mainValue = computeCSSAtom(m_typList, m_curMainCSST,
                                 m_curMainCSSA, m_typList.size() - 1,
                                 m_cssParam.m_mainAveNumb, m_cssParam.m_css12Rate,
                                 m_cssParam.m_mainCssRate1, m_cssParam.m_mainCssRate2,
                                 m_cssParam.m_maxCSS, m_cssParam.m_minCSS, false);

    m_subValue = computeCSSAtom(m_typList, m_curSubCSST,
                                m_curSubCSSA, m_typList.size() - 1,
                                m_cssParam.m_subAveNumb, m_cssParam.m_css12Rate,
                                m_cssParam.m_mainCssRate1, m_cssParam.m_mainCssRate2,
                                m_cssParam.m_maxCSS, m_cssParam.m_minCSS, false);

    m_energyValue = computeCSSAtom(m_typList, m_curEnergyCSST,
                                   m_curEnergyCSSA, m_typList.size() - 1,
                                   m_cssParam.m_energyAveNumb, m_cssParam.m_css12Rate,
                                   m_cssParam.m_energyCssRate1, m_cssParam.m_energyCssRate2,
                                   m_cssParam.m_maxCSS, m_cssParam.m_minCSS, true);

    QList<double> aveList = getRealtimeAveList();
    QList<double> cssList = getRealtimeCSSList();
    m_isAddedRealtimeData = isAddRealtimeData();
    emit sendRealTimeCSSData_signal(aveList, cssList, m_dataID, m_isAddedRealtimeData);

    resetRealtimeData();

    // qDebug() << QString("oldSelectData:%1, curSelectData:%2, oldHedgedData:%3, curHedgedData:%4, isAdded: %5 ")
    //                     .arg(m_oldSelectData).arg(m_curSelectData)
    //                     .arg(m_oldHedgedData).arg(m_curHedgedData)
    //                     .arg(m_isAddedRealtimeData);
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
        if (m_oldTime <= m_oneDayTimeList[i] && m_curTime >= m_oneDayTimeList[i])
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

        m_curMainCSSA   = m_oldMainCSSA;
        m_curMainCSST   = m_oldMainCSST;
        m_curSubCSSA    = m_oldSubCSSA;
        m_curSubCSST    = m_oldSubCSST;
        m_curEnergyCSSA = m_oldEnergyCSSA;
        m_curEnergyCSST = m_oldEnergyCSST;
    }
    else
    {
        m_oldMainCSST   = m_curMainCSST;
        m_oldMainCSSA   = m_curMainCSSA;
        m_oldSubCSST    = m_curSubCSST;
        m_oldSubCSSA    = m_curSubCSSA;
        m_oldEnergyCSST = m_curEnergyCSST;
        m_oldEnergyCSSA = m_curEnergyCSSA;

        m_oldSelectData = m_curSelectData;
        m_oldHedgedData = m_curHedgedData;

        m_oldCodeClose  = m_curCodeClose;
        m_oldTime       = m_curTime;
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

void HistoryData::releaseReadPortfolioDataThreads()
{
    for (int i = 0; i < m_dataReaderList.size(); ++i)
    {
        if (NULL != m_dataReaderList[i])
        {
            disconnect(this, SIGNAL(getHistPortfolioData_Signal(QString)),
                       m_dataReaderList[i], SLOT(getHistPortfolioData_slot(QString)));

            disconnect(m_dataReaderList[i], SIGNAL(sendHistPortfolioData_signal(QMap<QString, QList<QStringList>>)),
                       this, SLOT(sendHistPortfolioData_slot(QMap<QString, QList<QStringList>>)));
        }
    }

    for (int i = 0; i < m_dataReaderThreadList.size(); ++i)
    {
        m_dataReaderThreadList[i]->quit();
        m_dataReaderThreadList[i]->wait();
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

void HistoryData::sendRealtimeFutureData_slot(QList<double>)
{
}

