#ifndef CHARTDATA_H
#define CHARTDATA_H
#include "macd.h"

class ChartData
{
public:
    ChartData();
    ChartData(double earningData, double votData,
              double timeData, MACD macdData, 
              double hedgeData=0);

public:
    double m_earningData;
    double m_votData;
    double m_timeData;
    double m_hedgeData;
    MACD   m_MACDData;
};

#endif // CHARTDATA_H
