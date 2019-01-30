#include "monitorrealtimedata.h"
#include <QDateTime>
#include <QDebug>
#include <QThread>
#include <algorithm>
#include "io_func.h"
#include "secode_func.h"
#include "time_func.h"
#include "compute_func.h"
#include "spread_compute.h"

#pragma execution_character_set("utf-8")

using namespace  std;

MonitorRealTimeData::MonitorRealTimeData(QString dbhost, int monitorTime, 
                                        bool isBuySalePortfolio, bool isCSSChart,
                                        QList<int> macdTime, QString hedgeIndexCode,
                                        QMap<QString, int> seocdebuyCountMap, 
                                        QStringList secodeNameList,
                                        QMap<QString, int> buyStrategyMap, 
                                        QMap<QString, int> saleStrategyMap,
                                        QObject* parent):
    m_dbhost(dbhost), m_monitorTime(monitorTime), m_macdTime(macdTime),
    m_hedgeIndexCode(hedgeIndexCode), 
    m_portfolio(seocdebuyCountMap), m_secodeNameList(secodeNameList),
    m_buyPortfolio(buyStrategyMap), m_salePortfolio(saleStrategyMap),
    QObject(parent)
{
    m_isFuture             = false;
    m_isBuySalePortfolio   = isBuySalePortfolio; 
    m_isIndex              = false;
    m_isPortfolio          = true;
    m_isCSSChart           = isCSSChart;

    m_isInitTimer          = false;
    m_pTimer               = nullptr;

    m_hedgeIndexCode = getCompleteIndexCode(m_hedgeIndexCode, "wind");
    m_portfolio = transPorfolio(m_portfolio, "wind");
    m_buyPortfolio = transPorfolio(m_buyPortfolio, "wind"); 
    m_salePortfolio = transPorfolio(m_salePortfolio, "wind"); 

    for (int i = 0; i < m_secodeNameList.size(); ++i)
    {
        m_secodeNameList[i] = getCompleteSecode(m_secodeNameList[i], "wind");
    }

    // printList(m_secodeNameList, "m_secodeNameList");    

    initCommonData();
    initDatabase();
}

MonitorRealTimeData::MonitorRealTimeData(QString dbhost, int monitorTime,
                                         QString selectIndex, QString hedgedIndex,
                                         QObject* parent):
  m_dbhost(dbhost), m_monitorTime(monitorTime),
  m_selectIndex(selectIndex), m_hedgedIndex(hedgedIndex),
  m_isFuture(false), m_isPortfolio(false), 
  m_isBuySalePortfolio(false), m_isIndex(true),  
  QObject(parent)
{
    m_isFuture              = false;
    m_isBuySalePortfolio    = false; 
    m_isIndex               = true;
    m_isPortfolio           = false;
    m_isCSSChart            = false;  

    m_isInitTimer           = false;
    m_pTimer                = nullptr;

    m_selectIndex = getCompleteIndexCode(m_selectIndex, "wind");
    m_hedgedIndex = getCompleteIndexCode(m_hedgedIndex, "wind");

    initDatabase();
}

MonitorRealTimeData::MonitorRealTimeData(QString dbhost, int monitorTime, QString futureName,
                                         QObject* parent):
    m_dbhost(dbhost), m_futureName(futureName), m_monitorTime(monitorTime),   
    QObject(parent)
{
    m_isFuture              = true;
    m_isBuySalePortfolio    = false; 
    m_isIndex               = false;
    m_isPortfolio           = false;
    m_isCSSChart            = false;  

    m_isInitTimer           = false;
    m_pTimer                = nullptr;
    // initTimer();
    initDatabase();
}

MonitorRealTimeData::~MonitorRealTimeData() 
{
    if (NULL != m_database) 
    {
        delete m_database;
        m_database = NULL;
    }

    if (nullptr != m_pTimer)
    {
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

void MonitorRealTimeData::initCommonData() 
{
    initIndexHedgeMetaInfo();
    m_singleSecodeReadTime = 30;
    for (int i = 0; i < m_secodeNameList.size(); ++i) 
    {
        QList<double> tmpVot;
        QList<QString> tmpTime;
        m_vot.insert(m_secodeNameList[i], tmpVot);
        m_time.insert(m_secodeNameList[i], tmpTime);
    }
}

void MonitorRealTimeData::initDatabase() 
{
    m_database = new Database(m_dbhost);
}

void MonitorRealTimeData::initTimer()
{
    if (!isTradingDay(QDate::currentDate()) && isTradingOver())
    {
        qDebug() << QDate::currentDate();
        return;
    }

    if (m_isInitTimer)
    {
        return;
    }

    if (m_isFuture) 
    {
        // connect(&m_timer, SIGNAL(timeout()), this, SLOT(setFutureData()));
    } 

    if (m_isPortfolio) 
    {
        // connect(&m_timer, SIGNAL(timeout()), this, SLOT(setRealTimeData()));
    }

    if (m_isIndex)
    {
        // connect(&m_timer, SIGNAL(timeout()), this, SLOT(getRealtimeData_slot()));
    }

    qDebug() << "MonitorRealTimeData::initTimer";

    m_pTimer = new QTimer();
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(getRealtimeData_slot()));

    m_isInitTimer = true;
}

void MonitorRealTimeData::initIndexHedgeMetaInfo() 
{
    m_indexHedgeMetaInfo.insert("000300.SH", 300);
    m_indexHedgeMetaInfo.insert("000016.SH", 300);
    m_indexHedgeMetaInfo.insert("000852.SH", 1000);
    m_indexHedgeMetaInfo.insert("000904.SH", 200);
    m_indexHedgeMetaInfo.insert("000905.SH", 200);
    m_indexHedgeMetaInfo.insert("000906.SH", 800);
    m_indexHedgeMetaInfo.insert("399903.SZ", 100);
}

void MonitorRealTimeData::addMacd(MACD initMacdData) 
{
    m_macdData.append(initMacdData);
}

void MonitorRealTimeData::startTimer() 
{
    if (isTradingOver())
    {
        return;
    }

    initTimer();

    if (m_isInitTimer && m_pTimer != nullptr && !m_pTimer->isActive()) 
    {
        qDebug() << "MonitorRealTimeData::startTimer";
        m_pTimer->start(m_monitorTime);
    }   
}

void MonitorRealTimeData::stopTimer() 
{
    if (m_isInitTimer && m_pTimer != nullptr && m_pTimer->isActive()) 
    {
        qDebug() << "stopTimer";
        m_pTimer->stop();
    }
}

void MonitorRealTimeData::startReadRealtimeData_slot()
{
    startTimer();    
}

void MonitorRealTimeData::getPreCloseSpread_slot() 
{
    QMap<QString, QStringList> allPreCLoseData = m_database->getPreCloseData("PreCloseData");
    double preCloseSpread;
    if (m_isBuySalePortfolio) {
        preCloseSpread = getHedgedSpread(allPreCLoseData, m_buyPortfolio, m_salePortfolio);
    } else {
        preCloseSpread = getHedgedSpread(allPreCLoseData, m_portfolio,
                                         m_hedgeIndexCode, m_portfolio[m_hedgeIndexCode],
                                         m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }
    emit sendPreCloseData(preCloseSpread);
}

void MonitorRealTimeData::getRealtimeData_slot()
{
    /**/
    // qDebug() << "Monitor Thread: " << QThread::currentThreadId();

    if (isStockTradingOver()) 
    {
       stopTimer();
    }

    if (!isStockTrading())
    {
        return;
    }
    
    if (m_isFuture)
    {
        qDebug() << "Is Future";
        QList<double> result = m_database->getFutureSpread(m_futureName);
        double datetime = result[1];
        if (datetime > 0) 
        {
            emit sendRealtimeFutureData_signal(result);
        } else {
            qDebug() << QString("获取实时期货基差失败");
        }
    }

    if (m_isIndex)
    {
        QStringList todayKeyValueList;
        todayKeyValueList << "日期" << "时间" << "最新成交价" << "开盘" << "前收";
        QList<double> sendData;

        if (m_hedgedIndex.size() > 0)
        {
            m_selectIndex = getCompleteIndexCode(m_selectIndex, "wind");
            m_hedgedIndex = getCompleteIndexCode(m_hedgedIndex, "wind");
            QStringList selectData = m_database->getSnapShootData(m_selectIndex, todayKeyValueList);
            QStringList hedgedData = m_database->getSnapShootData(m_hedgedIndex, todayKeyValueList);


            double selectTime = QDateTime(getDate(selectData[0]), getTime(selectData[1])).toMSecsSinceEpoch();;
            double hedgedTime = QDateTime(getDate(hedgedData[0]), getTime(hedgedData[1])).toMSecsSinceEpoch();;
            double timeData = selectTime > hedgedTime ? selectTime : hedgedTime;

            sendData.append(selectData[2].toDouble());
            sendData.append(hedgedData[2].toDouble());

            // double selectChg = (selectData[2].toDouble() - selectData[4].toDouble()) / selectData[4].toDouble();
            // double hedgeChg = (hedgedData[2].toDouble() - hedgedData[4].toDouble()) / hedgedData[4].toDouble();
            // sendData.append(selectChg);
            // sendData.append(hedgeChg);

            sendData.append(timeData);

            emit sendRealTimeIndexData_signal(selectData, hedgedData);
        }
        else
        {
            m_selectIndex = getCompleteIndexCode(m_selectIndex, "wind");
            QStringList selectData = m_database->getSnapShootData(m_selectIndex, todayKeyValueList);
            sendData.append(selectData[2].toDouble());
            sendData.append(selectData[4].toDouble());
            double currTime = QDateTime(getDate(selectData[0]), getTime(selectData[1])).toMSecsSinceEpoch();            
            sendData.append(currTime);            
        }
        
        emit sendRealtimeIndexData_signal(sendData);              
    }

    if (m_isPortfolio)
    {
        QMap<QString, QStringList> oriRealTimeData = m_database->getSnapShootData(m_secodeNameList);

        bool isDataUseful = checkPortfolioData(oriRealTimeData);
        
        if (isDataUseful) 
        {
            if (m_isCSSChart)
            {
                emit sendRealtimeSpreadData_signal(computeSpreadData());
            }
            else
            {
                emit sendRealtimeSpreadMACDData_signal(computeSpreadMACDData());
            }
        }
        else
        {
            qDebug() << "Data is UnUseful";
        }
    }
}

// 根据更新的数据比例判断当前的更新是否有效;
bool MonitorRealTimeData::checkPortfolioData(QMap<QString, QStringList> realTimeData) 
{
    int sameTimeCount = 0;
    double unUpdatedDataPercent = 0.8;

    if (realTimeData.find("Error") != realTimeData.end()) 
    {
        return false;
    }

    for (QMap<QString, QStringList>::iterator it = realTimeData.begin();
         it != realTimeData.end(); ++it) 
    {
        QString secode = it.key();

        m_vot[secode].append(realTimeData[secode][4].toDouble());
        m_time[secode].append(realTimeData[secode][1]);

        if (m_time[secode].size() > 1 
         && m_time[secode].last() == m_time[secode][m_time[secode].size()-2]) 
        {
            ++sameTimeCount ;
        }

        m_realTimeData[secode] = realTimeData[secode];

        // 计算成交量;
        // double currUpdateTime = 100;
        // if(m_time[secode].size()>1) 
        // {
        //    currUpdateTime = m_time[secode].last().toDouble() - m_time[secode][m_time[secode].size()-2].toDouble();
        // }

        if ( m_vot[secode].size() > 1) 
        {
            m_realTimeData[secode][4] = QString("%1").arg(m_vot[secode].last() - m_vot[secode][m_vot[secode].size()-2]);
        } 
        else 
        {
            m_realTimeData[secode][4] = QString("%1").arg(0);
        }
    }

    sameTimeCount < m_secodeNameList.size() * unUpdatedDataPercent;
    if (realTimeData.size() == m_secodeNameList.size()) 
    {
        return true;
    } 
    else 
    {
        if (sameTimeCount >= m_secodeNameList.size() * unUpdatedDataPercent) 
        {
            for (QMap<QString, QStringList>::iterator it = realTimeData.begin();
                 it != realTimeData.end(); ++it) 
            {
                QString secode = it.key();
                if (!m_vot[secode].isEmpty()) 
                {
                    m_vot[secode].pop_back();
                }
                if (!m_time[secode].isEmpty()) 
                {
                    m_time[secode].pop_back();
                }
            }
        }
        return false;
    }
}

ChartData MonitorRealTimeData::computeSpreadMACDData() 
{
    QList<double> result;
    if (m_isBuySalePortfolio) 
    {
        result = getHedgedData(m_realTimeData, m_buyPortfolio, m_salePortfolio);
    } 
    else 
    {
        result = getHedgedData(m_realTimeData, m_portfolio,
                               m_hedgeIndexCode, m_portfolio[m_hedgeIndexCode],
                               m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }

    double strategyData = result[0];
    double votData = result[1];
    double timeData = result[2];

    MACD macdData;

    if (m_macdData.size() > 0) 
    {
        MACD latestData = m_macdData[m_macdData.size()-1];
        macdData = computeMACDData(strategyData, latestData, 
                                    m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    } 
    else 
    {
        macdData = MACD(strategyData, strategyData, 0, 0, 0);
    }
    m_macdData.append(macdData);

    ChartData curChartData(strategyData, votData, timeData, macdData);
    return curChartData;
}

QList<double> MonitorRealTimeData::computeSpreadData()
{
    QList<double> result;
    if (m_isBuySalePortfolio) 
    {
        result = getHedgedData(m_realTimeData, m_buyPortfolio, m_salePortfolio);
    } 
    else 
    {
        result = getHedgedData(m_realTimeData, m_portfolio,
                               m_hedgeIndexCode, m_portfolio[m_hedgeIndexCode],
                               m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }
    result.removeAt(1);
    return result;
}

void MonitorRealTimeData::getHistFutureData_slot() 
{
    QList<double> result = m_database->getHistFutureSpread(m_futureName);
    double datetime = result[1];
    if (datetime > 0) {
        emit sendHistFutureData_signal(result);
    } else {
        qDebug() << QString("获取历史期货基差失败");
    }
}


