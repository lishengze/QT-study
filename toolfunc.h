#ifndef TOOLFUNC_H
#define TOOLFUNC_H
#include <QString>
#include <QList>
#include <QPointF>
#include "macd.h"

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

QString removePathName(QString fullFileName);

QList<QPointF> sortPointFList(QList<QPointF> oridata);

QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
                                      QList<QPointF> secondList, int secondBuyCount);

QList<MACD> computeMACD(QList<double> oriData, int t1, int t2, int t3);

void ErrorMessage(QString msg);

QList<double> getMACDRange(QList<MACD> oriData);

QString transOffsetSecondsToTime(qint64 offSecs);

QList<int> getNumbList(int dataNumb, int desNumb);

#endif // TOOLFUNC_H
