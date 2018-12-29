﻿#ifndef COMPUTE_FUNC_H
#define COMPUTE_FUNC_H
#include <QList>
#include <QPointF>
#include "macd.h"

extern int g_errorNumbValue;

double getAveValue(QList<double>);

double getAveValue(QList<double> oriData, int startPos, int endPos);

double getAveDev(QList<double> oriData);

double reboundValue(double oridata, double maxValue, double minValue);

QList<MACD> computeMACD(QList<double> oriData, int t1, int t2, int t3);

MACD computeMACDData(double newData, MACD oldData, int t1, int t2, int t3);

QList<double> computeMACDDoubleData(QList<double> oriData, int t1, int t2, int t3);

QList<QList<double>> getHedgedData(QList<QPointF>& pointDataList, QMap<QString, QStringList>& indexHedgeData,
                            int indexBuyCount, int indexBaseCount);

QList<QList<double>> getHedgedData(QList<QPointF>& buyPointDataList, QList<QPointF>& salePointDataList);

QList<double> getHedgedData(QMap<QString, QStringList>& oneTimeData, QMap<QString, int> seocdebuyCountMap,
                            QString indexCode, int indexBuyCount, int indexBaseCount);

QList<double> getHedgedData(QMap<QString, QStringList>& oneTimeData,
                            QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy);

double getHedgedSpread(QMap<QString, QStringList>& preCloseData, QMap<QString, int> seocdebuyCountMap,
                                QString indexCode, int indexBuyCount, int indexBaseCount);

double getHedgedSpread(QMap<QString, QStringList> preCloseData,
                         QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy);

QList<QStringList> getRelativeProfitList(QList<QStringList> selectIndexData, QList<QStringList> hedgedIndexData);

QList<QStringList> getCSSList(QList<QStringList> profiltList, int aveNumb, double csst12Rate,
                              double csstRate1, double csstRate2, double maxCSS, double minCSS);

QList<double> getCSSList(QList<double> typList, int aveNumb, double csst12Rate,
                         double csstRate1, double csstRate2, double maxCSS, double minCSS,
                         bool isPotentialEnergy);

double getCSSAtom(QList<double>& typList, QList<double>& csstList, QList<double>& cssaList, 
                    int dataIndex, int aveNumb, double csst12Rate,
                    double csstRate1, double csstRate2, double maxCSS, double minCSS,
                    bool isPotentialEnergy);

QList<QList<double>> getAVEList(QList<double> oridata, QList<int> aveNumb, QList<bool> isEMAList);




QList<double> computeCSSList(QList<double> typList, double& curCSST, double& curCSSA, 
                            int aveNumb, double csst12Rate,
                            double csstRate1, double csstRate2, double maxCSS, double minCSS,
                            bool isPotentialEnergy);

double computeCSSAtom(QList<double>& typList, double& curCSST, double& curCSSA, 
                    int dataIndex, int aveNumb, double csst12Rate,
                    double csstRate1, double csstRate2, double maxCSS, double minCSS,
                    bool isPotentialEnergy);

void computeAVEAtom(QList<double> oridataList, int oridataIndex, 
                    QList<QList<double>>& aveList, 
                    QList<int> aveNumb, QList<bool> isEMAList);

void computeAVEList(QList<double>& oridataList, QList<QList<double>>& aveList, 
                    QList<int> aveNumb, QList<bool> isEMAList);

QList<double> getAVEList(QList<double> oridata, int aveNumb, bool isEMAList);

template<class T>
T mean(QList<T> oridata, int startPos, int endPos) {
    if (startPos < 0 || startPos > oridata.size()-1
      || endPos < 0 || endPos > oridata.size()
      || oridata.size() == 0 || startPos >= endPos) {
        return g_errorNumbValue;
    }
    double sum = 0;
    for (int i = startPos; i < endPos; ++i) {
        sum += oridata[i];
    }
    double result = sum / (endPos - startPos);
    return result;

}

template<class T>
T Max(QList<T> oridata, int startPos, int endPos) {
    if (startPos < 0 || startPos > oridata.size()-1
      || endPos < 0 || endPos > oridata.size()
      || oridata.size() == 0 || startPos >= endPos) {
        return g_errorNumbValue;
    }
    T result = oridata[startPos];
    for (int i = startPos; i < endPos; ++i) {
        if (oridata[i] > result) result = oridata[i];
    }
    return result;

}

#endif // COMPUTE_FUNC_H
