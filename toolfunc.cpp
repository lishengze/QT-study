#include "toolfunc.h"
#include <QStringList>
#include <algorithm>
using std::sort;

// 20170911 -> [2017, 9, 11]
QList<int> getDateList(int intDate) {
    QList<int> dateList;
    int year = intDate / 10000;
    int month = (intDate - year * 10000) / 100;
    int day = intDate - year * 10000 - month * 100;
    dateList.append (year);
    dateList.append (month);
    dateList.append (day);
    return dateList;
}

// 112002 -> [11, 20 ,2]
QList<int> getTimeList(int intDate) {
    QList<int> timeList;
    int hour = intDate / 10000;
    int minute = (intDate - hour * 10000) / 100;
    int second = intDate - hour * 10000 - minute * 100;
    timeList.append (hour);
    timeList.append (minute);
    timeList.append (second);
    return timeList;
}

QString removePathName(QString fullFileName) {
    QStringList tmpList = fullFileName.split ("/");
    QString fileName = tmpList.at(tmpList.size ()-1);
    fileName.remove(".xlsx");
    return fileName;
}

bool compQPointF(QPointF pointA, QPointF pointB) {
    return pointA.x() < pointB.x();
}

QList<QPointF> sortPointFList(QList<QPointF> oridata) {
    sort(oridata.begin (), oridata.end (), compQPointF);
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
