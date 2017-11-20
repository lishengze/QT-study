#ifndef SETDATA_H
#define SETDATA_H

#include <QList>
#include <QPointF>
#include <QLineSeries>
QList<QPointF> getDatabaseData();

QT_CHARTS_USE_NAMESPACE

void setSeriresData(QLineSeries* series);

#endif // SETDATA_H
