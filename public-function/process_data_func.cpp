#include "process_data_func.h"
#include <QDebug>
#include <algorithm>
#include "secode_func.h"
#include "io_func.h"

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

QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
                                      QList<QPointF> secondList, int secondBuyCount) {

    int firstIter = 0;
    int secondIter = 0;
    QList<QPointF> result;
    while (firstIter < firstList.size() && secondIter < secondList.size ()) {
        if (firstList[firstIter].x () < secondList[secondIter].x ()) {
            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
            ++firstIter;
        } else if (firstList[firstIter].x () >  secondList[secondIter].x ()) {
            result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
            ++secondIter;
        } else {
            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount + secondList[secondIter].y () * secondBuyCount));
            ++firstIter;
            ++secondIter;
        }
    }
    while (firstIter < firstList.size()) {
        result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
        ++firstIter;
    }
    while( secondIter < secondList.size ()) {
        result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
        ++secondIter;
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

QList<double> getMACDRange(QList<MACD> oriData) {
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

        int rangeInterval = 6;
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

