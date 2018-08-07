#ifndef PROCESS_DATA_FUNC_H
#define PROCESS_DATA_FUNC_H
#include <QList>
#include <QPointF>
#include "macd.h"

QList<QPointF> sortPointFList(QList<QPointF> oridata);

QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
                                      QList<QPointF> secondList, int secondBuyCount);

QList<double> getChartYvalueRange(QList<QPointF> pointList );

QList<double> getChartYvalueRange(QList<double> yValueList );

QList<double> getMACDRange(QList<MACD> oriData);

QList<int> getNumbList(int dataNumb, int desNumb);

QList<QString> getAddedList(QList<QString> oriList, QList<QString> afterAddList);

QMap<QString, int> transStrategyMap(QMap<QString, int> oriStrategyMap, QString codeStyle);

QList<QPointF> getStrategyPointList(QMap<QString, QList<QStringList>> oriData,
                                    QMap<QString, int> seocdebuyCountMap);

QMap<QString, int> EmpytQStringIntMap();

QStringList EmpytStringList();

QMap<QString, QStringList> EmptyQStringQStringListMap();

QList<QStringList> transRealTimeDataToMinuteData(QList<QStringList> oriData, QList<int> standardTimeList);

void resizeMinuteData(QList<QStringList>& selectIndexTransData, QList<QStringList>& hedgedIndexTransData);

template<class T>
QStringList transNumbData(QString title, QList<T> oriData) {
    QStringList tmpData;
    tmpData.append(title);
    for (int i = 0; i < oriData.size(); ++i) {
        tmpData.append(QString("%1").arg(oriData[i]));
    }
    return tmpData;
}

template<class T>
QList<T> getSubList(QList<T> oriData, int startIndex, int endIndex) {
    QList<T> result;
    if (startIndex < 0) return result;

    if (endIndex > oriData.size()) {
        endIndex = oriData.size();
    }

    for (int i = startIndex; i < endIndex; ++i) {
        result.append(oriData[i]);
    }
    return result;
}

#endif // PROCESS_DATA_FUNC_H
