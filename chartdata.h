#ifndef CHARTDATA_H
#define CHARTDATA_H
#include "macd.h"

class ChartData
{
public:
    ChartData();
    ChartData(double strategyValue, double votValue,
              double timeValue, MACD macdValue);

public:
    double strategyData;
    double votData;
    double timeData;
    MACD macdData;
};

#endif // CHARTDATA_H
