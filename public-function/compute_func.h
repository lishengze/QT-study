#ifndef COMPUTE_FUNC_H
#define COMPUTE_FUNC_H
#include <QList>
#include <QPointF>
#include "macd.h"

double getAveValue(QList<double>);

double getAveDev(QList<double> oriData);

double reboundValue(double oridata, double maxValue, double minValue);

QList<MACD> computeMACD(QList<double> oriData, int t1, int t2, int t3);

MACD computeMACDData(double newData, MACD oldData, int t1, int t2, int t3);

QList<double> computeMACDDoubleData(QList<double> oriData, int t1, int t2, int t3);

QList<QList<double>> getHedgedData(QList<QPointF> pointDataList, QMap<QString, QStringList> indexHedgeData,
                            int indexBuyCount, int indexBaseCount);

QList<QPointF> getHedgedData(QList<QPointF> buyPointDataList, QList<QPointF> salePointDataList);

QList<double> getHedgedData(QMap<QString, QStringList> oneTimeData, QMap<QString, int> seocdebuyCountMap,
                            QString indexCode, int indexBuyCount, int indexBaseCount);

QList<double> getHedgedData(QMap<QString, QStringList> oneTimeData,
                            QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy);

double getHedgedSpread(QMap<QString, QStringList> preCloseData, QMap<QString, int> seocdebuyCountMap,
                                QString indexCode, int indexBuyCount, int indexBaseCount);

double getHedgedSpread(QMap<QString, QStringList> preCloseData,
                         QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy);

QList<QStringList> getRelativeProfitList(QList<QStringList> selectIndexData, QList<QStringList> hedgedIndexData);

QList<QStringList> getCSSList(QList<QStringList> profiltList, int aveNumb, double csst12Rate,
                              double csstRate1, double csstRate2, double maxCSS, double minCSS);

#endif // COMPUTE_FUNC_H
