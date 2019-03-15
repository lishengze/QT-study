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

using namespace std;
#pragma execution_character_set("utf-8")

MonitorRealTimeData::MonitorRealTimeData(DatabaseParam dbParam, HedgedParam hedgedParam, QObject *parent):
    m_dbParam(dbParam), m_hedgedParam(hedgedParam), QObject(parent)
{
    m_isFuture                      = false;
    m_hedgedParam.m_oriCode         = getCompleteIndexCode(m_hedgedParam.m_oriCode, "wind");
    m_hedgedParam.m_hedgedCode      = getCompleteIndexCode(m_hedgedParam.m_hedgedCode, "wind");
    m_hedgedParam.m_oriPortfolio    = transStrategyMap(m_hedgedParam.m_oriPortfolio, "wind");
    m_hedgedParam.m_hedgedPortfolio = transStrategyMap(m_hedgedParam.m_hedgedPortfolio, "wind");
    for (int i = 0; i < m_hedgedParam.m_codeList.size(); ++i)
    {
        m_hedgedParam.m_codeList[i] = getCompleteSecode(m_hedgedParam.m_codeList[i], "wind");
    }

    initCommonData();
}                                               

MonitorRealTimeData::MonitorRealTimeData(QString dbhost, QString futureName, QObject *parent)
    : m_futureName(futureName), QObject(parent)
{
    m_isFuture = true;
    m_dbParam.m_dbhost = dbhost;
    initCommonData();
}

MonitorRealTimeData::~MonitorRealTimeData()
{
    stopTimer();

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
    initDatabase();
    m_isInitTimer   = false;
    m_pTimer        = nullptr;  
    m_isTest        = false;
    m_monitorTime   = 3 * 1000;
    m_lastTime      = QDateTime::currentDateTime();

    if (m_hedgedParam.m_isCSSChart)
    {
        m_updateMSecs = m_hedgedParam.m_energyUpdateMSecs;
    }
    else
    {
        m_updateMSecs = m_hedgedParam.m_spreadUpdateMSecs;
    }
    
    if (m_hedgedParam.m_isPortfolio)
    {
        initIndexHedgeMetaInfo();
        for (int i = 0; i < m_hedgedParam.m_codeList.size(); ++i)
        {
            QList<double> tmpVot;
            QList<QString> tmpTime;
            m_vot.insert(m_hedgedParam.m_codeList[i], tmpVot);
            m_time.insert(m_hedgedParam.m_codeList[i], tmpTime);
        }
    }

    m_isTest = false;
}

void MonitorRealTimeData::initDatabase()
{
    m_database = new Database(m_dbParam.m_dbhost);
}

void MonitorRealTimeData::initTimer()
{
    if (m_isTest == false)
    {
        if (!isTradingDay(QDate::currentDate()) && isTradingOver())
        {
            qDebug() << QDate::currentDate();
            return;
        }
    }

    if (m_isInitTimer)
    {
        return;
    }

    qDebug() << "MonitorRealTimeData::initTimer";

    m_pTimer = new QTimer();

    connect(m_pTimer, SIGNAL(timeout()),
            this, SLOT(readRealtimeData_timer_slot()));

    m_isInitTimer = true;
}

void MonitorRealTimeData::initIndexHedgeMetaInfo()
{
    m_indexPriceMap.insert("000300.SH", 300);
    m_indexPriceMap.insert("000016.SH", 300);
    m_indexPriceMap.insert("000852.SH", 1000);
    m_indexPriceMap.insert("000904.SH", 200);
    m_indexPriceMap.insert("000905.SH", 200);
    m_indexPriceMap.insert("000906.SH", 800);
    m_indexPriceMap.insert("399903.SZ", 100);
}

void MonitorRealTimeData::addMacd(MACD initMacdData)
{
    m_macdData.append(initMacdData);
}

void MonitorRealTimeData::startTimer()
{
    if (m_isTest == false)
    {
        if (isTradingOver())
        {
            return;
        }
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
        qDebug() << "MonitorRealTimeData stopTimer";
        m_pTimer->stop();
    }
    qDebug() << "";
}

void MonitorRealTimeData::checkUpdateTime()
{
    m_currTime = QDateTime::currentDateTime();
    int currCostTime = m_currTime.toMSecsSinceEpoch() - m_lastTime.toMSecsSinceEpoch();

    if (currCostTime < m_updateMSecs)
    {
        QThread::msleep(m_updateMSecs - currCostTime);
    }

    m_lastTime = QDateTime::currentDateTime();
}

void MonitorRealTimeData::getRealtimeData_slot()
{
    if (m_isTest)
    {
        readRealtimeData_timer_slot();
    }
    else
    {
        if (isStockTrading())
        {
            readRealtimeData_timer_slot();
        }
        else if (!isStockTradingOver())
        {
            startTimer();
        }   
    }
}

void MonitorRealTimeData::getPreCloseData_slot()
{
    double preCloseSpread = 0;
    if (m_hedgedParam.m_hedgedType > 0)
    {
        QMap<QString, QStringList> allPreCLoseData = m_database->getPreCloseData("PreCloseData");
        
        if (m_hedgedParam.m_isPortfolioHedge)
        {
            preCloseSpread = getHedgedSpread(allPreCLoseData, m_hedgedParam.m_oriPortfolio, m_hedgedParam.m_hedgedPortfolio);
        }
        else
        {
            preCloseSpread = getHedgedSpread(allPreCLoseData, 
                                            m_hedgedParam.m_oriPortfolio,
                                            m_hedgedParam.m_hedgedCode, 
                                            m_hedgedParam.m_oriPortfolio[m_hedgedParam.m_hedgedCode],
                                            m_indexPriceMap[m_hedgedParam.m_hedgedCode]);
        }
    }
    emit sendPreCloseData_signal(preCloseSpread);
}

void MonitorRealTimeData::processRealtimeDataComplete_slot()
{
    readRealtimeData_timer_slot();
}

void MonitorRealTimeData::readRealtimeData_timer_slot()
{
    if (m_isTest == false)
    {
        if (isStockTradingNotStart())
        {
            return;
        }
        else if (isStockTrading())
        {
            stopTimer();
        }
        else if (isStockTradingOver())
        {
            stopTimer();
            return;
        }
        else if (isStockNoonBreak())
        {
            startTimer();
            return;
        }
    }

    if (m_isFuture)
    {
        QList<double> result = m_database->getFutureSpread(m_futureName);
        double datetime = result[1];
        if (datetime > 0)
        {
            checkUpdateTime();

            emit sendRealtimeFutureData_signal(result);
        }
        else
        {
            qDebug() << QString("获取实时期货基差失败");
        }
    }

    if (m_hedgedParam.m_isIndex)
    {
        QStringList todayKeyValueList;
        todayKeyValueList << "日期"
                          << "时间"
                          << "最新成交价"
                          << "开盘"
                          << "前收";
        QList<double> sendData;

        if (m_hedgedParam.m_hedgedCode.size() > 0)
        {
            m_hedgedParam.m_oriCode = getCompleteIndexCode(m_hedgedParam.m_oriCode, "wind");
            m_hedgedParam.m_hedgedCode = getCompleteIndexCode(m_hedgedParam.m_hedgedCode, "wind");
            QStringList selectData = m_database->getSnapShootData(m_hedgedParam.m_oriCode, todayKeyValueList);
            QStringList hedgedData = m_database->getSnapShootData(m_hedgedParam.m_hedgedCode, todayKeyValueList);

            double selectTime = QDateTime(getDate(selectData[0]), getTime(selectData[1])).toMSecsSinceEpoch();
            double hedgedTime = QDateTime(getDate(hedgedData[0]), getTime(hedgedData[1])).toMSecsSinceEpoch();
            double timeData = selectTime > hedgedTime ? selectTime : hedgedTime;

            sendData.append(selectData[2].toDouble());
            sendData.append(hedgedData[2].toDouble());
            sendData.append(timeData);
        }
        else
        {
            m_hedgedParam.m_oriCode = getCompleteIndexCode(m_hedgedParam.m_oriCode, "wind");
            QStringList selectData = m_database->getSnapShootData(m_hedgedParam.m_oriCode, todayKeyValueList);
            sendData.append(selectData[2].toDouble());
            sendData.append(selectData[4].toDouble());
            double currTime = QDateTime(getDate(selectData[0]), getTime(selectData[1])).toMSecsSinceEpoch();
            sendData.append(currTime);
        }

        checkUpdateTime();
        emit sendRealtimeIndexData_signal(sendData);
    }

    if (m_hedgedParam.m_isPortfolio)
    {
        QMap<QString, QStringList> oriRealTimeData = m_database->getSnapShootData(m_hedgedParam.m_codeList);

        QMap<QString, double> lastCodeClose;
        for (QMap<QString, QStringList>::const_iterator it = oriRealTimeData.begin();
            it != oriRealTimeData.end(); ++it)
        {
            QString code = it.key();
            if (m_hedgedParam.m_isCSSChart)
            {
                code = getCompleteSecode(code, "tinysoft");
            }
            lastCodeClose.insert(code, it.value()[2].toDouble());
        }

        bool isDataUseful = checkPortfolioData(oriRealTimeData);

        if (isDataUseful)
        {
            if (m_hedgedParam.m_isCSSChart)
            {
                QList<double> result = computeSpreadData();
                checkUpdateTime();
                emit sendRealtimeSpreadData_signal(result, lastCodeClose);
            }
            else
            {
                QList<double> result = computeSpreadMACDData();
                checkUpdateTime();
                emit sendRealtimeSpreadMACDData_signal(result, lastCodeClose);
            }
        }
        else
        {
            qDebug() << "Data is UnUseful";
        }
    }
}

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

        if (m_time[secode].size() > 1 && m_time[secode].last() == m_time[secode][m_time[secode].size() - 2])
        {
            ++sameTimeCount;
        }

        m_realTimeData[secode] = realTimeData[secode];

        // 计算成交量;
        // double currUpdateTime = 100;
        // if(m_time[secode].size()>1)
        // {
        //    currUpdateTime = m_time[secode].last().toDouble() - m_time[secode][m_time[secode].size()-2].toDouble();
        // }

        if (m_vot[secode].size() > 1)
        {
            m_realTimeData[secode][4] = QString("%1").arg(m_vot[secode].last() - m_vot[secode][m_vot[secode].size() - 2]);
        }
        else
        {
            m_realTimeData[secode][4] = QString("%1").arg(0);
        }
    }

    // sameTimeCount < m_hedgedParam.m_codeList.size() * unUpdatedDataPercent;
    if (realTimeData.size() == m_hedgedParam.m_codeList.size())
    {
        return true;
    }
    else
    {
        if (sameTimeCount >= m_hedgedParam.m_codeList.size() * unUpdatedDataPercent)
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

QList<double> MonitorRealTimeData::computeSpreadMACDData()
{
    QList<double> result;
    if (m_hedgedParam.m_isPortfolioHedge)
    {
        result = getHedgedData(m_realTimeData, m_hedgedParam.m_oriPortfolio, m_hedgedParam.m_hedgedPortfolio);
    }
    else
    {
        result = getHedgedData(m_realTimeData, m_hedgedParam.m_oriPortfolio, m_hedgedParam.m_hedgedCode);
    }
    return result;
}

QList<double> MonitorRealTimeData::computeSpreadData()
{
    QList<double> result;
    if (m_hedgedParam.m_isPortfolioHedge)
    {
        result = getHedgedData(m_realTimeData, m_hedgedParam.m_oriPortfolio, m_hedgedParam.m_hedgedPortfolio);
    }
    else
    {
        result = getHedgedData(m_realTimeData, m_hedgedParam.m_oriPortfolio, m_hedgedParam.m_hedgedCode);
    }
    result.removeAt(1);
    return result;
}

void MonitorRealTimeData::getHistFutureData_slot()
{
    QList<double> result = m_database->getHistFutureSpread(m_futureName);
    double datetime = result[1];
    if (datetime > 0)
    {
        emit sendHistFutureData_signal(result);
    }
    else
    {
        qDebug() << QString("获取历史期货基差失败");
    }
}
