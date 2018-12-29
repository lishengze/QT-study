#include "process_data_func.h"
#include <QDebug>
#include <algorithm>
#include "secode_func.h"
#include "io_func.h"
#include "time_func.h"

using std::abs;
using std::sort;
using std::max;
using std::min;

bool compQPointF(QPointF pointA, QPointF pointB) {
    return pointA.x() < pointB.x();
}

QList<QPointF> sortPointFList(QList<QPointF> oridata) {
    sort(oridata.begin (), oridata.end (), compQPointF);
    return oridata;
}

bool compQString(QStringList strListA, QStringList strListB) {
    return strListA[0].toLongLong() < strListB[0].toLongLong();
}

QList<QStringList> sortQStringList(QList<QStringList> oridata) {
    sort(oridata.begin (), oridata.end (), compQString);
    return oridata;
}

QList<QPointF> mergeSortedPointedList(QList<QPointF>& firstList, int firstBuyCount,
                                      QList<QPointF>& secondList, int secondBuyCount, bool isUnion) {

    int firstIter = 0;
    int secondIter = 0;
    QList<QPointF> result;
    while (firstIter < firstList.size() && secondIter < secondList.size ()) {
        if (firstList[firstIter].x () < secondList[secondIter].x ())
        {
            if (isUnion)
            {
                result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
            }
            ++firstIter;
        }
        else if (firstList[firstIter].x () >  secondList[secondIter].x ())
        {
            if (isUnion)
            {
                result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
            }
            ++secondIter;
        }
        else
        {
            result.append (QPointF(firstList[firstIter].x(),
                                   firstList[firstIter].y()*firstBuyCount + secondList[secondIter].y()*secondBuyCount));
            ++firstIter;
            ++secondIter;
        }
    }
    if (isUnion)
    {
        while (firstIter < firstList.size()) {
            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
            ++firstIter;
        }
        while( secondIter < secondList.size ()) {
            result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
            ++secondIter;
        }
    }

    return result;
}

QList<double> getChartYvalueRange(QList<QPointF> pointList ) {
    double maxValue = -1000000000000000000.0;
    double minValue = 10000000000000000000.0;
    for (int i = 0; i < pointList.size(); ++i) {
        maxValue = max(maxValue, pointList.at(i).y());
        minValue = min(minValue, pointList.at (i).y ());
    }

    int rangeInterval = 6;
    maxValue += (maxValue - minValue) / rangeInterval;
    minValue -= (maxValue - minValue) / rangeInterval;
    QList<double> result;
    result.append (minValue);
    result.append (maxValue);
    return result;
}

QList<double> getChartYvalueRange(QList<QList<double>> yValueList) {
    QList<double> result;
    if (yValueList.size() == 0) {
        result.append(0);
        result.append(1);
    } else {
        double maxValue = -1000000000000000000.0;
        double minValue = 10000000000000000000.0;
        for (int i = 0; i < yValueList.size(); ++i) {
            for (int j = 0; j < yValueList[i].size(); ++j) {
                maxValue = max(maxValue, yValueList[i][j]);
                minValue = min(minValue, yValueList[i][j]);
            }
        }

        int rangeInterval = 6;
        maxValue += (maxValue - minValue) / rangeInterval;
        minValue -= (maxValue - minValue) / rangeInterval;

        if (maxValue == minValue) {
            double addedRange = abs(maxValue) / rangeInterval;
            maxValue = maxValue + addedRange;
            minValue = minValue - addedRange;
        }
        result.append (minValue);
        result.append (maxValue);
    }

    return result;
}

QList<double> getChartYvalueRange(QList<double> yValueList ) {
    QList<double> result;
    if (yValueList.size() == 0) {
        result.append(0);
        result.append(1);
    } else {
        double maxValue = -1000000000000000000.0;
        double minValue = 10000000000000000000.0;
        for (int i = 0; i < yValueList.size(); ++i) {
            maxValue = max(maxValue, yValueList[i]);
            minValue = min(minValue, yValueList[i]);
        }

        int rangeInterval = 6;
        maxValue += (maxValue - minValue) / rangeInterval;
        minValue -= (maxValue - minValue) / rangeInterval;

        if (maxValue == minValue) {
            double addedRange = abs(maxValue) / rangeInterval;
            maxValue = maxValue + addedRange;
            minValue = minValue - addedRange;
        }
        result.append (minValue);
        result.append (maxValue);
    }

    return result;
}

QList<double> getMACDRange(QList<MACD> oriData, int rangeInterval) {
    QList<double> result;
    if (oriData.size() == 0) {
        result.append(0);
        result.append(1);
    } else {
        double maxValue = -1000000000000000000.0;
        double minValue = 10000000000000000000.0;
        for (int i = 0; i < oriData.size(); ++i) {
            maxValue = max(max(maxValue, oriData[i].m_diff), max(oriData[i].m_dea, oriData[i].m_macd));
            minValue = min(min(minValue, oriData[i].m_diff), min(oriData[i].m_dea, oriData[i].m_macd));
        }

        maxValue += (maxValue - minValue) / rangeInterval;
        minValue -= (maxValue - minValue) / rangeInterval;

        if (maxValue == minValue) {
            if (maxValue != 0) {
                double addedRange = abs(maxValue) / rangeInterval;
                maxValue = maxValue + addedRange;
                minValue = minValue - addedRange;
            } else {
                maxValue = 0.5;
                minValue = -0.5;
            }
        }

        result.append (minValue);
        result.append (maxValue);
    }

    return result;
}

QList<int> getNumbList(int dataNumb, int intervalNumb) {
    QList<int> result;
    if (dataNumb <= intervalNumb) {
        for (int i = 0; i < dataNumb; ++i) {
            result.append(i);
        }
    } else {
        int intervalLength = dataNumb / intervalNumb;
        int leftInterval = dataNumb % intervalNumb - 1;
        for (int i = 0, tmpIntervalNumb = 0; tmpIntervalNumb < intervalNumb; ++tmpIntervalNumb) {
            result.append(i);
            if (tmpIntervalNumb < leftInterval) {
                i += intervalLength + 1;
            } else {
                i += intervalLength;
            }
//            qDebug() << "i: " <<i;
        }
        result.append(dataNumb - 1);
    }
    return result;
}

QList<QString> getAddedList(QList<QString> listA, QList<QString> listB) {
    QList<QString> result;
    for(int i = 0; i < listB.size (); ++i) {
        bool curBStringStrInListA = false;
        for (int j = 0; j < listA.size(); ++j) {
            if (listB[i] == listA[j]) {
                curBStringStrInListA = true;
                break;
            }
        }
        if (!curBStringStrInListA) {
            result.append (listB[i]);
        }
    }
    return result;
}

QList<QPointF> getStrategyPointList(QMap<QString, QList<QStringList>> oriData,
                                    QMap<QString, int> seocdebuyCountMap) {
    QList<QString> keyList = seocdebuyCountMap.keys ();
    QList<QPointF> pointDataList;
    for (int i = 0; i < keyList.size (); ++i) {
        QString key = keyList[i];
        QList<QStringList> tmpStringList = oriData[key];
        QList<QPointF> tmpPointData;
        for (int i = 0; i < tmpStringList.size (); ++i) {
            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(),
                                 tmpStringList[i][1].toDouble() * seocdebuyCountMap[key]));
        }
        tmpPointData = sortPointFList(tmpPointData);
        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
    }
    return pointDataList;
}

QMap<QString, int> transStrategyMap(QMap<QString, int> oriStrategyMap, QString codeStyle) {
    QMap<QString, int> result;
    for (QMap<QString, int>::const_iterator i = oriStrategyMap.begin(); i != oriStrategyMap.end(); ++i) {
        result.insert(getCompleteSecode(i.key(), codeStyle), i.value());
    }
    return result;
}

QMap<QString, int> EmpytQStringIntMap() {
    QMap<QString, int> data;
    return data;
}

QStringList EmpytStringList() {
    QStringList data;
    return data;
}

QMap<QString, QStringList> EmptyQStringQStringListMap() {
    QMap<QString, QStringList> result;
    return result;
}

QList<QStringList> transRealTimeDataToMinuteData(QList<QStringList> oriData, QList<int> standardTimeList) {
    QList<QStringList> result;
    int timeIndex = 0;
    for (int i = 0; i < oriData.size(); ++i) {
        int currTime = oriData[i][1].toInt();
        int standardTime = standardTimeList[timeIndex];
    //        qDebug() << "currTime: " << currTime << ", standardTime: " << standardTime;
        if (currTime == standardTime) {
            result.append(oriData[i]);
            timeIndex += 1;
        } else if (currTime > standardTime) {
            int lastTime = oriData[i-1][1].toInt();
            QStringList currData;
            if (currTime - standardTime > standardTime - lastTime - 40) {
                currData = oriData[i-1];
    //                qDebug() << "A lastTime: " << lastTime << ", currTime: " << currTime << ", standardTime: " << standardTime;
    //                qDebug() << abs(currTime - standardTime) << ", " << abs(lastTime - standardTime);
            } else {
                currData = oriData[i];
    //                qDebug() << "B lastTime: " << lastTime << ", currTime: " << currTime << ", standardTime: " << standardTime;
    //                qDebug() << abs(currTime - standardTime) << ", " << abs(lastTime - standardTime);
            }
    //            result.append(currData);
            currData[1] = QString("%1").arg(standardTime);
            result.append(currData);
            timeIndex += 1;
        }
        if (timeIndex == standardTimeList.size()) break;
    }
    return result;
}

void resizeMinuteData(QList<QStringList>& selectIndexTransData, QList<QStringList>& hedgedIndexTransData) {
    int dataSize = min(selectIndexTransData.size(), hedgedIndexTransData.size());
    while (selectIndexTransData.size() > dataSize) {
        selectIndexTransData.removeAt(selectIndexTransData.size()-1);
    }
    while (hedgedIndexTransData.size() > dataSize) {
        hedgedIndexTransData.removeAt(hedgedIndexTransData.size()-1);
    }
}

void deleteDelistData(QList<QStringList>& oriData, int flagPos) {
    int delistCount = 0;
    for (int i = 0; i < oriData.size(); ++i) {
        if (oriData[i][flagPos].toDouble() == 0) {
            oriData.removeAt(i);
            --i;
            ++delistCount;
        }
    }
    qDebug() << "delistCount: " << delistCount;
}

void getHedegdTYP(QList<QStringList>& selectData, QList<QStringList>& hedgedData,
                  QList<QStringList>& result)
{

    while (isTimeEarly(selectData[0], hedgedData[0]))
    {
        selectData.removeFirst();
    }

    while(isTimeEarly( hedgedData[0], selectData[0]))
    {
        hedgedData.removeFirst();
    }

    QList<double> earningList;
    earningList.append(1.0);
    for (int selectDataIndex = 0, hedgedDataIndex = 0; selectDataIndex < selectData.size() && hedgedDataIndex < hedgedData.size();)
    {
        QStringList curData;
        if (isTimeEarly(selectData[selectDataIndex], hedgedData[hedgedDataIndex]))
        {
            ++selectDataIndex;
        }
        else if (isTimeEarly( hedgedData[hedgedDataIndex], selectData[selectDataIndex]))
        {
            ++hedgedDataIndex;
        }
        else
        {
            curData.append(selectData[selectDataIndex][0]);
            curData.append(selectData[selectDataIndex][1]);
            double relativeProfit = selectData[selectDataIndex][4].toDouble()-
                                    hedgedData[hedgedDataIndex][4].toDouble();

            earningList.append(earningList.last()*(1 + relativeProfit));

            double TYP = (earningList.last() + earningList[earningList.size()-2]) / 2;
            curData[2] = QString("%1").arg(TYP);

            result.append(curData);
            ++selectDataIndex;
            ++hedgedDataIndex;
        }
    }
}

void setHedgedTYPClose(QList<QStringList>& selectData, QList<QStringList>& hedgedData, bool isMinute,
                       QList<QString>& timeList, QList<double>& typList, QList<double>& earningList)
{
    // qDebug() << QString("OriDataSize: selectData.size: %1, hedgedData.size: %2")
    //            .arg(selectData.size()).arg(hedgedData.size());

    int secDelistCount = 0;
    int secEarlyCount = 0;
    int hedDelistCount = 0;
    int hedEarlyCount = 0;

    while (isTimeEarly(selectData[0], hedgedData[0]))
    {
        selectData.removeFirst();
        ++secEarlyCount;
    }

    while(isTimeEarly(hedgedData[0], selectData[0]))
    {
        hedgedData.removeFirst();
        ++hedEarlyCount;
    }

    // qDebug() << QString("TransedDataSize: selectData.size: %1, hedgedData.size: %2")
    //            .arg(selectData.size()).arg(hedgedData.size());


    for (int selectDataIndex = 0, hedgedDataIndex = 0; selectDataIndex < selectData.size() && hedgedDataIndex < hedgedData.size();)
    {
        if (selectData[selectDataIndex][5].toDouble() == 0)
        {
            // qDebug() <<"Sec Delist " << selectData[selectDataIndex] << hedgedData[hedgedDataIndex];
            ++selectDataIndex;
            ++hedgedDataIndex;
            ++secDelistCount;
            continue;
        }
        if (hedgedData[hedgedDataIndex][5].toDouble() == 0)
        {

            // qDebug() <<"Hed Delist "<< selectData[selectDataIndex] << hedgedData[hedgedDataIndex];
            ++selectDataIndex;
            ++hedgedDataIndex;
            ++hedDelistCount;
            continue;
        }

        if (isTimeEarly(selectData[selectDataIndex], hedgedData[hedgedDataIndex]))
        {
            // qDebug() <<"Sec Early " <<selectData[selectDataIndex] << hedgedData[hedgedDataIndex];
            ++selectDataIndex;
            ++secEarlyCount;
        }
        else if (isTimeEarly( hedgedData[hedgedDataIndex], selectData[selectDataIndex]))
        {
            // qDebug() <<"Hed Early "<< selectData[selectDataIndex] << hedgedData[hedgedDataIndex];
            ++hedgedDataIndex;
            ++hedEarlyCount;
        }
        else
        {
            if (isMinute)
            {
                timeList.append(QString("%1 %2").arg(selectData[selectDataIndex][0]).arg(selectData[selectDataIndex][1]));
            }
            else
            {
                timeList.append(QString("%1").arg(selectData[selectDataIndex][0]));
            }
            double relativeProfit = selectData[selectDataIndex][4].toDouble() -
                                    hedgedData[hedgedDataIndex][4].toDouble();

            if (earningList.size() == 0)
            {
                 earningList.append(1 + relativeProfit);
                 typList.append((earningList.last() + 1) / 2);
            }
            else
            {
                 earningList.append(earningList.last()*(1 + relativeProfit));
                 typList.append((earningList.last() + earningList[earningList.size()-2]) / 2);
            }
            ++selectDataIndex;
            ++hedgedDataIndex;
        }
    }
/*
    for (int i = typList.size()-1; i > 0; --i) 
    {
        typList[i] = (typList[i] + typList[i-1]) / 2;
    }
    typList.pop_front();
    */
}



void setHedgedTYPClose(QList<QStringList>& selectData, bool isMinute,
                       QList<QString>& timeList, QList<double>& typList,
                       QList<double>& closeList)
{
    for (int selectDataIndex = 0; selectDataIndex < selectData.size();)
    {
        if (selectData[selectDataIndex][5].toDouble() == 0)
        {
            ++selectDataIndex;
            continue;
        }
        if (isMinute)
        {
            timeList.append(QString("%1 %2").arg(selectData[selectDataIndex][0]).arg(selectData[selectDataIndex][1]));
        }
        else
        {
            timeList.append(QString("%1").arg(selectData[selectDataIndex][0]));
        }

        closeList.append(selectData[selectDataIndex][2].toDouble());
        typList.append((selectData[selectDataIndex][2].toDouble() + selectData[selectDataIndex][3].toDouble()) / 2);
        ++selectDataIndex;
    }
}

void getHedegdTYP(QList<double>& hedgedValueList, QList<double>& indexValueList,
                  QList<double>& typList, QList<double>& earningList)
{
    typList.append(1);
    earningList.append(1.0);
    for (int i = 1; i < hedgedValueList.size() && i < indexValueList.size(); ++i)
    {
        double relativeProfit = (hedgedValueList[i] - hedgedValueList[i-1]) / indexValueList[i-1];
        earningList.append(earningList.last() * (1 + relativeProfit));
        double TYP = (earningList.last() + earningList[earningList.size()-2]) / 2;
        typList.append(TYP);
    }   
}

void setTransedTYP(QList<double>& oriTypeList, QList<double>& desTypList)
{
    for (int i = 1; i < oriTypeList.size(); ++i)
    {
        desTypList.append((oriTypeList[i] + oriTypeList[i-1])/2);
    }
}