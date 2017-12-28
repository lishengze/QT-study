#include "chartdata.h"

ChartData::ChartData()
{

}

ChartData::ChartData(double strategyValue, double votValue,
          double timeValue, MACD macdValue) :
    strategyData(strategyValue), votData(votValue),
    timeData(timeValue), macdData(macdValue)
{

}

//explict ChartData::ChartData(ChartData &srcData) {
//    strategyData = srcData.strategyData;
//    votData = srcData.votData;
//    timeData = srcData.timeData;
//    macdData = srcData.macdData;
//}
