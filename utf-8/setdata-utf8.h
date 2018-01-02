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
#include <QSqlDatabase>
#include <QSqlQuery>
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

/**连接sql server数据库
  *数据库名：abc
  *表名：SQL_2000
  *用户名：sa
  *密码：123
  *端口号：(默认)1433
*/
void connectSql(QString sIp, QString iPort,  QString sDbNm, QString sUserNm, QString sPwd);

#endif // SETDATA_H
