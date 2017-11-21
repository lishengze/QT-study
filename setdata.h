#ifndef SETDATA_H
#define SETDATA_H

#include <QList>
#include <QPointF>
#include <QLineSeries>
QList<QPointF> getOpenPrice();

QList<QPointF> getTimeData();

QT_CHARTS_USE_NAMESPACE

void setSeriresData(QLineSeries* series);

void testSetTimeData(QLineSeries* series);

#endif // SETDATA_H
