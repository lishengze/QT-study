#include "chartdata.h"

ChartData::ChartData()
{

}

ChartData::ChartData(double earningData, double votData,
                     double timeData, MACD macdData, 
                     double hedgeData) :
    m_earningData(earningData), m_votData(votData),
    m_timeData(timeData), m_MACDData(macdData), m_hedgeData(hedgeData)
{

}

//explict ChartData::ChartData(ChartData &srcData) {
//    strategyData = srcData.strategyData;
//    votData = srcData.votData;
//    timeData = srcData.timeData;
//    macdData = srcData.macdData;
//}
