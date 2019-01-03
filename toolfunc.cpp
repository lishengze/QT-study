#include "toolfunc.h"
#include <QStringList>
#include <algorithm>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QWidget>
#include <QStringList>
#include <QProcess>
#include <QStandardItemModel>
#include <QThread>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QScrollBar>
#include <QFileInfo>
#include "xlsxdocument.h"

using std::sort;
using std::max;
using std::min;

/*
//QString removePathName(QString fullFileName) {
//    QStringList tmpList = fullFileName.split ("/");
//    QString fileName = tmpList.at(tmpList.size ()-1);
//    fileName.remove(".xlsx");
//    return fileName;
//}

//bool compQPointF(QPointF pointA, QPointF pointB) {
//    return pointA.x() < pointB.x();
//}

//QList<QPointF> sortPointFList(QList<QPointF> oridata) {
//    sort(oridata.begin (), oridata.end (), compQPointF);
//    return oridata;
//}

//bool compQString(QStringList strListA, QStringList strListB) {
//    return strListA[0].toLongLong() < strListB[0].toLongLong();
//}

//QList<QStringList> sortQStringList(QList<QStringList> oridata) {
//    sort(oridata.begin (), oridata.end (), compQString);
//    return oridata;
//}

//QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
//                                      QList<QPointF> secondList, int secondBuyCount) {

//    int firstIter = 0;
//    int secondIter = 0;
//    QList<QPointF> result;
//    while (firstIter < firstList.size() && secondIter < secondList.size ()) {
//        if (firstList[firstIter].x () < secondList[secondIter].x ()) {
//            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
//            ++firstIter;
//        } else if (firstList[firstIter].x () >  secondList[secondIter].x ()) {
//            result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
//            ++secondIter;
//        } else {
//            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount + secondList[secondIter].y () * secondBuyCount));
//            ++firstIter;
//            ++secondIter;
//        }
//    }
//    while (firstIter < firstList.size()) {
//        result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
//        ++firstIter;
//    }
//    while( secondIter < secondList.size ()) {
//        result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
//        ++secondIter;
//    }
//    return result;
//}

//QList<double> getChartYvalueRange(QList<QPointF> pointList ) {
//    double maxValue = -1000000000000000000.0;
//    double minValue = 10000000000000000000.0;
//    for (int i = 0; i < pointList.size(); ++i) {
//        maxValue = max(maxValue, pointList.at(i).y());
//        minValue = min(minValue, pointList.at (i).y ());
//    }

//    int rangeInterval = 6;
//    maxValue += (maxValue - minValue) / rangeInterval;
//    minValue -= (maxValue - minValue) / rangeInterval;
//    QList<double> result;
//    result.append (minValue);
//    result.append (maxValue);
//    return result;
//}

//QList<double> getChartYvalueRange(QList<double> yValueList ) {
//    QList<double> result;
//    if (yValueList.size() == 0) {
//        result.append(0);
//        result.append(1);
//    } else {
//        double maxValue = -1000000000000000000.0;
//        double minValue = 10000000000000000000.0;
//        for (int i = 0; i < yValueList.size(); ++i) {
//            maxValue = max(maxValue, yValueList[i]);
//            minValue = min(minValue, yValueList[i]);
//        }

//        int rangeInterval = 6;
//        maxValue += (maxValue - minValue) / rangeInterval;
//        minValue -= (maxValue - minValue) / rangeInterval;

//        if (maxValue == minValue) {
//            double addedRange = abs(maxValue) / rangeInterval;
//            maxValue = maxValue + addedRange;
//            minValue = minValue - addedRange;
//        }

////        if (abs(maxValue - minValue) / abs(minValue) < 0.2) {
////            maxValue += max(abs(maxValue),  abs(minValue)) / 4;
////            minValue -= max(abs(maxValue),  abs(minValue)) / 4;
////        }
//        result.append (minValue);
//        result.append (maxValue);
//    }

//    return result;
//}

//QList<double> getMACDRange(QList<MACD> oriData) {
//    QList<double> result;
//    if (oriData.size() == 0) {
//        result.append(0);
//        result.append(1);
//    } else {
//        double maxValue = -1000000000000000000.0;
//        double minValue = 10000000000000000000.0;
//        for (int i = 0; i < oriData.size(); ++i) {
//            maxValue = max(max(maxValue, oriData[i].m_diff), max(oriData[i].m_dea, oriData[i].m_macd));
//            minValue = min(min(minValue, oriData[i].m_diff), min(oriData[i].m_dea, oriData[i].m_macd));
//        }

//        int rangeInterval = 6;
//        maxValue += (maxValue - minValue) / rangeInterval;
//        minValue -= (maxValue - minValue) / rangeInterval;

//        if (maxValue == minValue) {
//            if (maxValue != 0) {
//                double addedRange = abs(maxValue) / rangeInterval;
//                maxValue = maxValue + addedRange;
//                minValue = minValue - addedRange;
//            } else {
//                maxValue = 0.5;
//                minValue = -0.5;
//            }
//        }

//        result.append (minValue);
//        result.append (maxValue);
//    }

//    return result;
//}

//QList<int> getNumbList(int dataNumb, int intervalNumb) {
//    QList<int> result;
//    if (dataNumb <= intervalNumb) {
//        for (int i = 0; i < dataNumb; ++i) {
//            result.append(i);
//        }
//    } else {
//        int intervalLength = dataNumb / intervalNumb;
//        int leftInterval = dataNumb % intervalNumb - 1;
//        for (int i = 0, tmpIntervalNumb = 0; tmpIntervalNumb < intervalNumb; ++tmpIntervalNumb) {
//            result.append(i);
//            if (tmpIntervalNumb < leftInterval) {
//                i += intervalLength + 1;
//            } else {
//                i += intervalLength;
//            }
////            qDebug() << "i: " <<i;
//        }
//        result.append(dataNumb - 1);
//    }
//    return result;
//}

//QList<QString> getAddedList(QList<QString> listA, QList<QString> listB) {
//    QList<QString> result;

////    for(int i = 0; i < listA.size (); ++i) {
////        bool curAStringStrInListB = false;
////        for (int j = 0; j < listB.size(); ++j) {
////            if (listA[i] == listB[j]) {
////                curAStringStrInListB = true;
////                break;
////            }
////        }
////        if (!curAStringStrInListB) {
////            result.append (listA[i]);
////        }
////    }

//    for(int i = 0; i < listB.size (); ++i) {
//        bool curBStringStrInListA = false;
//        for (int j = 0; j < listA.size(); ++j) {
//            if (listB[i] == listA[j]) {
//                curBStringStrInListA = true;
//                break;
//            }
//        }
//        if (!curBStringStrInListA) {
//            result.append (listB[i]);
//        }
//    }
//    return result;
//}
*/








