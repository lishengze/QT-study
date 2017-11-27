#ifndef TOOLFUNC_H
#define TOOLFUNC_H
#include <QString>
#include <QList>
#include <QPointF>

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

QString removePathName(QString fullFileName);

QList<QPointF> sortPointFList(QList<QPointF> oridata);

QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
                                      QList<QPointF> secondList, int secondBuyCount);

#endif // TOOLFUNC_H
