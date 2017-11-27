#ifndef SETDATA_H
#define SETDATA_H

#include <QList>
#include <QPointF>
#include <QLineSeries>
#include <QAxObject>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include "strategy.h"

QList<QPointF> getOpenPrice();

QList<QPointF> getTimeData();

QList<QPointF> getTimeData(QString startDate, QString endDate, QString keyValue,
                           QString tableName, QString databaseName ="MarketData");

QT_CHARTS_USE_NAMESPACE

void setSeriresData(QLineSeries* series);

void testSetTimeData(QLineSeries* series);

void readBasicDataFromExcel(QString excelFilePath);

QList<strategy_ceil> readStrategyDataFromExcel(QString excelFilePath);

QList<QString> getExcelFileName(QString dirName);

#endif // SETDATA_H
