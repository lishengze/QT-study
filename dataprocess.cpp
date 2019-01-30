#include "dataprocess.h"
#include "toolfunc.h"
#include <QPointF>
#include <QDateTime>
#include <QString>
#include "compute_func.h"
#include "energy_compute.h"
#include "spread_compute.h"
#include "process_data_func.h"
#include "io_func.h"

DataProcess::DataProcess(bool isRealTime, bool isBuySalePortfolio,
                         QMap<QString, QList<QStringList>>& oridata,
                         QMap<QString, int> buyCount, QString hedgeIndexCode, QList<int> macdTime,
                         QObject *parent):
    m_isCSSChart(false), m_isRealTime(isRealTime), m_isBuySalePortfolio(isBuySalePortfolio),
    m_oriPortfilioData(oridata), m_portfolio(buyCount),
    m_hedgeIndexCode(hedgeIndexCode), m_macdTime(macdTime),
    QObject(parent)
{
    initIndexHedgeMetaInfo();
    m_secodeNameList = m_portfolio.keys();
    if (!m_isRealTime) {
        filterHedgeIndexData();
    }
}

DataProcess::DataProcess(bool isRealTime, bool isBuySalePortfolio,
                         QMap<QString, QList<QStringList>>& oridata,
                         QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy,
                         QList<int> macdTime,
                         QObject *parent):
    m_isCSSChart(false), m_isRealTime(isRealTime), m_isBuySalePortfolio(isBuySalePortfolio),
    m_oriPortfilioData(oridata), m_macdTime(macdTime),
    m_buyPortfolio(buyStrategy), m_salePortfolio(saleStrategy),
    QObject(parent)
{

}

DataProcess::DataProcess(QMap<QString, QList<QStringList>>& oridata,
                        QMap<QString, int>& portfolio, QString hedgeIndexCode,
                        QList<int> aveNumbList, QList<bool> isEMAList,
                        int mainAveNumb, int subAveNumb, int energyAveNumb,
                        double css12Rate, double mainCssRate1, double mainCssRate2,
                        double energyCssRate1, double energyCssRate2,
                        double maxCSS, double minCSS,
                        QObject *parent):
                        m_isCSSChart(true), m_isBuySalePortfolio(false), m_isRealTime(false),
                        m_oriPortfilioData(oridata), m_portfolio(portfolio), m_hedgeIndexCode(hedgeIndexCode),
                        m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
                        m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
                        m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
                        m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
                        m_maxCSS(maxCSS), m_minCSS(minCSS),
                        QObject(parent)
{
    initIndexHedgeMetaInfo();
    m_secodeNameList = m_portfolio.keys();
    filterHedgeIndexData();
}


void DataProcess::initIndexHedgeMetaInfo() 
{
    m_indexHedgeMetaInfo.insert("SH000300", 300);
    m_indexHedgeMetaInfo.insert("SH000016", 300);  // 上证50
    m_indexHedgeMetaInfo.insert("SH000852", 1000);
    m_indexHedgeMetaInfo.insert("SH000904", 200);
    m_indexHedgeMetaInfo.insert("SH000905", 200);
    m_indexHedgeMetaInfo.insert("SH000906", 800);
    m_indexHedgeMetaInfo.insert("SZ399903", 100);

    m_indexHedgeMetaInfo.insert("000300.SH", 300);
    m_indexHedgeMetaInfo.insert("000016.SH", 300);
    m_indexHedgeMetaInfo.insert("000852.SH", 1000);
    m_indexHedgeMetaInfo.insert("000904.SH", 200);
    m_indexHedgeMetaInfo.insert("000905.SH", 200);
    m_indexHedgeMetaInfo.insert("000906.SH", 800);
    m_indexHedgeMetaInfo.insert("399903.SZ", 100);
}

void DataProcess::filterHedgeIndexData() 
{
    QList<QStringList> tmpIndexHedgeData = m_oriPortfilioData[m_hedgeIndexCode];
    m_oriPortfilioData.remove(m_hedgeIndexCode);
    for (int i = 0; i < tmpIndexHedgeData.size (); ++i) 
    {
        QStringList tmpKeyValue;
        tmpKeyValue << tmpIndexHedgeData[i][1] << tmpIndexHedgeData[i][2];
        QString dateTimeString = QDateTime::fromMSecsSinceEpoch(qint64(tmpIndexHedgeData[i][0].toLongLong())).toString ("yyyyMMddhhmmss");
        m_indexHedgeData.insert(dateTimeString, tmpKeyValue);
    }
}

void DataProcess::receiveOrigianlHistoryData (QString dataType) {
    if (dataType == "all") 
    {
        if (m_isRealTime) 
        {
            emit getProcessedData_signal(computeSnapshootData());
        } 
        else 
        {
            emit getProcessedData_signal(computeAllData());
        }
    } 
    else 
    {
        if (dataType == "strategy") 
        {
            emit sendStrategyData (computeHedgedData());
        }
        if (dataType == "vot") 
        {
            emit sendVotData (computeVotData());
        }
        if (dataType == "macd") 
        {
            emit sendMACDData (computeMacdData());
        }
    }
}

QList<QList<double>> DataProcess::computeAllData() {
    QList<QList<double>> allData;
    computeHedgedData();
    allData.append (m_timeData);

    if (!m_isCSSChart)
    {
        allData.append (m_hedgedData);
        computeVotData();
        computeMacdData();
        allData.append(m_votData);
        allData.append(m_macdData);
    }
    else
    {
        allData.append(m_mainList);
        allData.append(m_subValueList);
        allData.append(m_energyValueList);
        allData += m_aveList;
        allData.append(m_hedgedData);
    }

    allData.append(m_indexCodeData);
    return allData;
}

QList<QList<double>> DataProcess::computeHedgedData () {
    QList<QList<double>> result;
    QList<QPointF> pointDataList;
    QList<QList<double>> hedgedResult;

    if (m_isBuySalePortfolio) 
    {
        QList<QPointF> buyPointDataList = getStrategyPointList(m_oriPortfilioData, m_buyPortfolio);
        QList<QPointF> salePointDataList = getStrategyPointList(m_oriPortfilioData, m_salePortfolio);
        hedgedResult = getHedgedData(buyPointDataList, salePointDataList);
    } 
    else 
    {
        QMap<QString, int> seocdebuyCountMap = m_portfolio;
        seocdebuyCountMap.remove(m_hedgeIndexCode);
        pointDataList = getStrategyPointList(m_oriPortfilioData, seocdebuyCountMap);
        hedgedResult = getHedgedData(pointDataList, m_indexHedgeData,
                                     m_portfolio[m_hedgeIndexCode],
                                     m_indexHedgeMetaInfo[m_hedgeIndexCode]);
    }

    m_timeData = hedgedResult[0];
    m_hedgedData = hedgedResult[1];
    m_indexCodeData = hedgedResult[2];

    /*
    qDebug() << "m_timeData.size: " << m_timeData.size()
              << "m_hedgedData.size: " << m_hedgedData.size()
              << "m_indexCodeData.size: " << m_indexCodeData.size();

    printList(m_timeData, "m_timeData");
    for (int i = 0; i < m_timeData.size() && i < 50; ++i)
    {
        qDebug() << QDateTime::fromMSecsSinceEpoch(m_timeData[i]).toString("yyyyMMdd hh:mm:ss");
    }

    printList(m_hedgedData, "m_hedgedData");
    printList(m_indexCodeData, "m_indexCodeData");
    */

    result.append(m_timeData);
    result.append(m_hedgedData);

    if (m_isCSSChart)
    {
        QList<double> oriTypeList;
        QList<double> typList;
        QList<double> earningList;
        getTypeEarningList(m_hedgedData, m_indexCodeData, oriTypeList, earningList);
        getTransedTYP(oriTypeList, typList);

        computeAVEList(earningList, m_aveList, m_aveNumbList, m_isEMAList);

        m_aveList.insert(0, earningList);

        m_mainList = getCSSList(typList,  m_mainAveNumb, m_css12Rate,
                                m_mainCssRate1, m_mainCssRate2, m_maxCSS, m_minCSS,false);
        m_subValueList = getCSSList(typList,  m_subAveNumb, m_css12Rate,
                                    m_mainCssRate1, m_mainCssRate2, m_maxCSS, m_minCSS,false);
        m_energyValueList = getCSSList(typList,  m_energyAveNumb, m_css12Rate,
                                       m_energyCssRate1, m_energyCssRate2, m_maxCSS, m_minCSS, true);
        result.append(m_mainList);
        result.append(m_subValueList);
        result.append(m_energyValueList);
    }

    return result;
}

QList<QList<double>> DataProcess::computeVotData () 
{
    QList<QList<double>> result;
    QList<QString> keyList = m_oriPortfilioData.keys ();
    QList<QPointF> pointDataList;
    for (int i = 0; i < keyList.size (); ++i) {
        QString key = keyList[i];
        QList<QStringList> tmpStringList = m_oriPortfilioData[key];
        QList<QPointF> tmpPointData;
        for (int i = 0; i < tmpStringList.size (); ++i) {
            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(), tmpStringList[i][2].toDouble()));
        }
        tmpPointData = sortPointFList(tmpPointData);
        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
    }

    if (m_timeData.size() == 0) {
        for (int i = 0; i < pointDataList.size(); ++i) {
            m_timeData.append(pointDataList[i].x());
        }
    }

    for (int i = 0; i < pointDataList.size(); ++i) {
        m_votData.append(pointDataList[i].y());
    }

    result.append(m_timeData);
    result.append(m_votData);
    return result;
}

QList<QList<double>> DataProcess::computeMacdData () 
{
    QList<QList<double>> result;
    m_macdData = computeMACDDoubleData(m_hedgedData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    result.append(m_timeData);
    result.append(m_macdData);
    return result;
}

// 逆序计算的原因: 实时数据的同时获取的数据各个股票的时间不一致，也无法排序进行计算，根据获取的时间顺序来进行计算;
QList<QList<double>> DataProcess::computeSnapshootData() 
{
    QList<QString> secodeList = m_oriPortfilioData.keys();

    QList<int> timeNumbList;
    int timeNumb = 1000000;
    for (int i = 0; i < secodeList.size(); ++i) {
        QString secode = secodeList[i];
        if (timeNumbList.indexOf(m_oriPortfilioData[secode].size()) < 0) {
            timeNumbList.append(m_oriPortfilioData[secode].size());
            if (timeNumb > m_oriPortfilioData[secode].size()) {
                timeNumb = m_oriPortfilioData[secode].size();
            }
        }
    }

    for (int i = timeNumb-1; i > -1; --i) {
        QMap<QString, QStringList> oneTimeData;
        for (int j = 0; j < secodeList.size(); ++j) {
            QString secode = secodeList[j];
            QStringList tmpData = m_oriPortfilioData[secode][i];
            double currUpdateTime = 100;
            if (i > 0) {
                currUpdateTime = m_oriPortfilioData[secode][i][1].toDouble() -  m_oriPortfilioData[secode][i-1][1].toDouble();
            }
            if (currUpdateTime < 8 || currUpdateTime >= 2*60*60) {
                tmpData[4] = QString("%1").arg(m_oriPortfilioData[secode][i][4].toDouble()
                                               - m_oriPortfilioData[secode][i-1][4].toDouble());
            } else {
                tmpData[4] = QString("%1").arg(0);
            }
            oneTimeData.insert(secode, tmpData);
        }

        QList<double> tmpResult;
        if (m_isBuySalePortfolio) {
            tmpResult = getHedgedData(oneTimeData, m_buyPortfolio, m_salePortfolio);
        } else {
            tmpResult = getHedgedData(oneTimeData, m_portfolio,
                                      m_hedgeIndexCode, m_portfolio[m_hedgeIndexCode],
                                      m_indexHedgeMetaInfo[m_hedgeIndexCode]);
        }
        if (tmpResult.size() != 0 && tmpResult[1] != 0) {
            m_hedgedData.prepend(tmpResult[0]);
            m_votData.prepend(tmpResult[1]);
            m_timeData.prepend(tmpResult[2]);
            m_indexCodeData.append(tmpResult[3]);
        }
    }

    QList<QList<double>> allData;
    allData.append (m_timeData);
    allData.append (m_hedgedData);
    allData.append (m_votData);

    m_macdData = computeMACDDoubleData(m_hedgedData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    allData.append(m_macdData);
    allData.append(m_indexCodeData);

    qDebug() <<"m_timeData.size: " << m_timeData.size()
             <<"m_hedgedData.size: " << m_hedgedData.size()
             <<"m_votData.size: " << m_votData.size()
             <<"m_macdData.size: " << m_macdData.size()
             <<"m_indexCodeData.size: " << m_indexCodeData.size();

    qDebug()<< "DataProcess::computeSnapshootDataReverse end!";
    return allData;
}

DataProcess::~DataProcess () {

}
