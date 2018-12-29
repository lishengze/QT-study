#include "realtimedataprocess.h"
#include <QDebug>
#include <iostream>
#include "toolfunc.h"
using namespace std;

#include "WAPIWrapperCpp.h"

RealTimeDataProcess::RealTimeDataProcess(QMap<QString, int> seocdebuyCountMap, QObject* parent):
    m_seocdebuyCountMap(seocdebuyCountMap),
    QObject(parent)
{

}

void RealTimeDataProcess::loginWind() {
}

void RealTimeDataProcess::startWsq(QStringList secodeList) {

}

