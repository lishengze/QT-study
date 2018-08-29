#include "compute_func.h"
#include <QMap>
#include <QDateTime>
#include <QDebug>
#include "process_data_func.h"
#include "time_func.h"
#include "process_data_func.h"
#include <algorithm>
using std::abs;
using std::max;

int g_errorNumbValue = -10000000;

double getAveValue(QList<double> oriData) {
    double result = 0;
    for (int i = 0; i < oriData.size(); ++i) {
        result += oriData[i];
    }
    return result/oriData.size();
}

double getAveValue(QList<double> oriData, int startPos, int endPos) {
//    qDebug() << "startPos: " << startPos << ", endPos: " << endPos;
    if (startPos < 0 || startPos > oriData.size()-1
      || endPos < 0 || endPos > oriData.size()
      || oriData.size() == 0 || startPos >= endPos) {
        return g_errorNumbValue;
    }
    double result = 0;
    for (int i = startPos; i < endPos; ++i) {
        result += oriData[i];
    }
    return result / (endPos - startPos);
}

double getAveDev(QList<double> oriData) {
    double result = 0;
    double ave = getAveValue(oriData);
    for (int i = 0; i < oriData.size(); ++i) {
        result += abs(oriData[i] - ave);
    }
    result /= oriData.size();
    return result;
}

double reboundValue(double oridata, double maxValue, double minValue) {
    double result = oridata;
    if (oridata > maxValue) {
        result = maxValue;
    }

    if (oridata < minValue) {
        result = minValue;
    }
    return result;
}


QList<MACD> computeMACD(QList<double> oriData, int t1, int t2, int t3) {
    double EMA1[2] = {oriData[0], 0.0};
    double EMA2[2] = {oriData[0], 0.0};
    double DIFF;
    double DEA[2];
    double Macd;
    QList<MACD> result;
    for (int i = 0; i < oriData.size (); ++i) {
        EMA1[1] = EMA1[0] * (t1-1) / (t1 + 1) + oriData[i] * 2 / (t1 + 1);
        EMA2[1] = EMA2[0] * (t2-1) / (t2 + 1) + oriData[i] * 2 / (t2 + 1);
        DIFF = EMA1[1] - EMA2[1];
        if (i == 0) {
            DEA[0] = DIFF;
        }
        DEA[1] = DEA[0] * (t3 -1) / (t3 + 1) + DIFF * 2 / (t3 + 1);
        Macd = 2 * (DIFF - DEA[1]);
        result.append (MACD(EMA1[1], EMA2[1], DIFF, DEA[1], Macd));

        EMA1[0] = EMA1[1];
        EMA2[0] = EMA2[1];
        DEA[0] = DEA[1];
    }
    return result;
}

MACD computeMACDData(double newData,  MACD oldData, int t1, int t2, int t3)  {
    double EMA1;
    double EMA2;
    double DIFF;
    double DEA;
    double Macd;
    MACD result;
    EMA1 = oldData.m_ema1 * (t1-1) / (t1 + 1) + newData * 2 / (t1 + 1);
    EMA2 = oldData.m_ema2 * (t2-1) / (t2 + 1) + newData * 2 / (t2 + 1);
    DIFF = EMA1 - EMA2;
    DEA = oldData.m_dea * (t3 -1) / (t3 + 1) + DIFF * 2 / (t3 + 1);
    Macd = 2 * (DIFF - DEA);
    return MACD(EMA1, EMA2, DIFF, DEA, Macd);
}

QList<double> computeMACDDoubleData(QList<double> oriData, int t1, int t2, int t3) {
    double EMA1[2] = {oriData[0], 0.0};
    double EMA2[2] = {oriData[0], 0.0};
    double DIFF;
    double DEA[2];
    double Macd;
    QList<double> result;
    for (int i = 0; i < oriData.size (); ++i) {
        EMA1[1] = EMA1[0] * (t1-1) / (t1 + 1) + oriData[i] * 2 / (t1 + 1);
        EMA2[1] = EMA2[0] * (t2-1) / (t2 + 1) + oriData[i] * 2 / (t2 + 1);
        DIFF = EMA1[1] - EMA2[1];
        if (i == 0) {
            DEA[0] = DIFF;
        }
        DEA[1] = DEA[0] * (t3 -1) / (t3 + 1) + DIFF * 2 / (t3 + 1);
        Macd = 2 * (DIFF - DEA[1]);
        result << EMA1[1] << EMA2[1] << DIFF << DEA[1] << Macd;

        EMA1[0] = EMA1[1];
        EMA2[0] = EMA2[1];
        DEA[0] = DEA[1];
    }
    return result;
}

QList<QList<double>> getHedgedData(QList<QPointF> pointDataList, QMap<QString, QStringList> indexHedgeData,
                            int indexBuyCount, int indexBaseCount) {
    QList<QList<double>> result;
    QList<double> strategyData;
    QList<double> hedgedData;
    QList<double> timeData;
    QList<QString> indexHedgedataTimeKey = indexHedgeData.keys ();
    for (int i = 0; i < pointDataList.size(); ++i) {
        QString timeKeyStr = QDateTime::fromMSecsSinceEpoch(qint64(pointDataList[i].x())).toString ("yyyyMMddhhmmss");
        if (indexHedgedataTimeKey.indexOf (timeKeyStr) >= 0) {
            strategyData.append(pointDataList[i].y() / (indexBuyCount * indexBaseCount)
                                - indexHedgeData[timeKeyStr][0].toDouble());
            hedgedData.append(indexHedgeData[timeKeyStr][0].toDouble());
            timeData.append(pointDataList[i].x());
        }
//        else {
//            strategyData.append(pointDataList[i].y() / (indexBuyCount* indexBaseCount));
//        }
    }
    result.append(timeData);
    result.append(strategyData);
    result.append(hedgedData);
    return result;
}

QList<QPointF> getHedgedData(QList<QPointF> buyPointDataList, QList<QPointF> salePointDataList) {
    QList<QPointF> pointDataList = mergeSortedPointedList(buyPointDataList, 1, salePointDataList, -1);
    return pointDataList;
}

QList<double> getHedgedData(QMap<QString, QStringList> oneTimeData, QMap<QString, int> seocdebuyCountMap,
                            QString indexCode, int indexBuyCount, int indexBaseCount) {
    QList<double> result;
    double strategyData = 0;
    double votData = 0;
    double timeData = 0;
    int timeCount = 0;
//    qDebug() << "getHedgedData " << 0;
//    qDebug() << "oneTimeData: " << oneTimeData;
    for (QMap<QString, int>::const_iterator i = seocdebuyCountMap.begin(); i != seocdebuyCountMap.end(); ++i) {
        QString secode = i.key();
        if (secode == indexCode) {
            continue;
        }
        if (oneTimeData[secode][2] == "0.0000") {
            strategyData += oneTimeData[secode][3].toDouble() * i.value();
        } else {
            strategyData += oneTimeData[secode][2].toDouble() * i.value();
        }
        votData += oneTimeData[secode][4].toDouble();
        timeData += oneTimeData[secode][5].toDouble();
        ++timeCount;
    }
//    qDebug() << "getHedgedData " << 1;

    timeData = timeData / timeCount;
    timeData = transDateTime(timeData);
//    timeData = QDateTime::currentMSecsSinceEpoch();
    double indexData = oneTimeData[indexCode][2].toDouble();
    strategyData = strategyData / (indexBuyCount * indexBaseCount) - indexData;

    result.append(strategyData);
    result.append(votData);
    result.append(timeData);
    result.append(indexData);
    return result;
}

QList<double> getHedgedData(QMap<QString, QStringList> oneTimeData,
                            QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy) {
    QList<double> result;
    double strategyData = 0;
    double votData = 0;
    double timeData = 0;
    int timeCount = 0;
    double buyStrategyData = 0;
    double saleStrategyData = 0;
    for (QMap<QString, int>::const_iterator i = buyStrategy.begin(); i != buyStrategy.end(); ++i) {
        QString secode = i.key();
        if (oneTimeData[secode][2] == "0.0000") {
            buyStrategyData += oneTimeData[secode][3].toDouble() * i.value();
        } else {
            buyStrategyData += oneTimeData[secode][2].toDouble() * i.value();
        }
        votData += oneTimeData[secode][4].toDouble();
        timeData += oneTimeData[secode][5].toDouble();
        ++timeCount;
    }

    for (QMap<QString, int>::const_iterator i = saleStrategy.begin(); i != saleStrategy.end(); ++i) {
        QString secode = i.key();
        if (oneTimeData[secode][2] == "0.0000") {
            saleStrategyData += oneTimeData[secode][3].toDouble() * i.value();
        } else {
            saleStrategyData += oneTimeData[secode][2].toDouble() * i.value();
        }
    }

    timeData = timeData / timeCount;
    timeData = transDateTime(timeData);
//    timeData = QDateTime::currentMSecsSinceEpoch();
    strategyData = buyStrategyData - saleStrategyData;
    strategyData /= 300;

    result.append(strategyData);
    result.append(votData);
    result.append(timeData);
    return result;
}

double getHedgedSpread(QMap<QString, QStringList> oriData, QMap<QString, int> seocdebuyCountMap,
                                QString indexCode, int indexBuyCount, int indexBaseCount) {
    double result = 0;
    for (QMap<QString, int>::const_iterator i = seocdebuyCountMap.begin(); i != seocdebuyCountMap.end(); ++i) {
        QString secode = i.key();
        if (secode != indexCode) {
            if(oriData.find(secode) == oriData.end()) {
                qDebug() << "Lost secode: " << secode;
            } else {
                result += oriData[secode][0].toDouble() * i.value();
            }

        }
    }
    result = result / (indexBuyCount * indexBaseCount) - oriData[indexCode][0].toDouble();
    qDebug() << "result: " << result;
    return result;
}

double getHedgedSpread(QMap<QString, QStringList> oriData,
                       QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy) {
    double result;
    double buyStrategyData = 0;
    double saleStrategyData = 0;
    for (QMap<QString, int>::const_iterator i = buyStrategy.begin(); i != buyStrategy.end(); ++i) {
        buyStrategyData += oriData[i.key()][0].toDouble() * i.value();
    }

    for (QMap<QString, int>::const_iterator i = saleStrategy.begin(); i != saleStrategy.end(); ++i) {
        saleStrategyData += oriData[i.key()][0].toDouble() * i.value();
    }

    result = buyStrategyData - saleStrategyData;
    result /= 300;
    return result;
}

QList<QStringList> getRelativeProfitList(QList<QStringList> selectIndexData, QList<QStringList> hedgedIndexData) {
    //    printList(selectIndexData, "selectIndexData");
    //    printList(hedgedIndexData, "hedgedIndexData");
    QList<QStringList> result;
    for (int i = 0; i < selectIndexData.size(); ++i) {
        QStringList currData = selectIndexData[i];
        if (i == 0) {
            currData[2] = QString("%1").arg(0);
        } else {
            double selectProfit = (selectIndexData[i][2].toDouble() - selectIndexData[i-1][2].toDouble())
                                    / selectIndexData[i-1][2].toDouble();
            double hedgedProfit = (hedgedIndexData[i][2].toDouble() - hedgedIndexData[i-1][2].toDouble())
                                    / hedgedIndexData[i-1][2].toDouble();
            currData[2] = QString("%1").arg(selectProfit-hedgedProfit);
        }
        result.append(currData);
    }
    return result;
}

QList<QStringList> getCSSList(QList<QStringList> profiltList, int aveNumb, double csst12Rate,
                              double csstRate1, double csstRate2, double maxCSS, double minCSS) {
    QList<QStringList> result = profiltList;
    QList<double> acmuEarning;
    acmuEarning.append(1);
    QList<double> typList;
    typList.append(0);
    QList<double> csstList;
    QList<double> cssList;
    QList<double> mainValueList;
    for (int i = 0; i < aveNumb; ++i) {
        csstList.append(0);
        cssList.append(0);
        mainValueList.append(0);
    }
    mainValueList.append(0);

    for (int i = 1; i < profiltList.size(); ++i) {
        double currEarning = (1 + profiltList[i][2].toDouble()) * acmuEarning[i-1];
        acmuEarning.append(currEarning);
        typList.append((acmuEarning[i] + acmuEarning[i-1]) / 2);
    }

    //    qDebug() <<"csst12Rate: " << csst12Rate << "csstRate1: " << csstRate1 << ", csstRate2: " << csstRate2;
    for (int i = aveNumb; i < typList.size(); ++i) {
        double typAveValue = getAveValue(getSubList(typList, i-aveNumb+1, i+1));
        double csst11 = typList[i] - typAveValue;
        double csst12 = csst12Rate * getAveDev(getSubList(typList, i-aveNumb+1, i+1));
        double csst13 = csst11 / csst12;
        double csst;
        if (i == aveNumb) {
            csst = csst13;
        } else {
            csst = csstList[i-1] * csstRate1 + csst13 * csstRate2;
    //            qDebug() << "csst: " <<csst;
        }
        csstList.append(csst);

        double css = reboundValue(csst, maxCSS, minCSS);
        cssList.append(css);

        if (i > aveNumb) {
            mainValueList.append((cssList[i] + cssList[i-1]) / 2);
    //            qDebug() << "acmuEarning: " << acmuEarning [i] << "typ: " << typList[i]
    //                     << "csst11: " << csst11 << ", csst12: " << csst12
    //                     << ", csst13: " << csst13 << ", csst: " <<csst
    //                     << ", css: " << css <<", mainValue: " << mainValueList[i];
        }
    }

    for (int i = 0; i < profiltList.size(); ++i) {
        result[i][2] = QString("%1").arg(acmuEarning[i]);
        result[i].append(QString("%1").arg(typList[i]));
        result[i].append(QString("%1").arg(csstList[i]));
        result[i].append(QString("%1").arg(cssList[i]));
        result[i].append(QString("%1").arg(mainValueList[i]));
    }

    return result;
}

QList<double> getCSSList(QList<double> oriTypList, int aveNumb, double csst12Rate,
                              double csstRate1, double csstRate2, double maxCSS, double minCSS,
                              bool isPotentialEnergy) {
    QList<double> result;
    QList<double> csstList;
    QList<double> cssList;
    QList<double> typList;
    for (int i = 0; i < aveNumb; ++i) {
        csstList.append(0);
        cssList.append(0);
        result.append(0);
    }
    result.append(0);

    for (int i = 1; i < oriTypList.size(); ++i) {
        typList.append((oriTypList[i] + oriTypList[i-1]) / 2);
    }
//    qDebug() << typList;

//    qDebug() << aveNumb << csst12Rate << csstRate1 << csstRate2 << maxCSS << minCSS;

    for (int i = aveNumb-1; i < typList.size(); ++i) {
        double typAveValue = getAveValue(typList, i-aveNumb+1, i+1);
        double csst11 = typList[i] - typAveValue;
        double csst12 = csst12Rate * getAveDev(getSubList(typList, i-aveNumb+1, i+1));
        double csst13 = csst11 / csst12;
        double csst;
        if (i == aveNumb-1) {
            csst = csst13;
        } else {
//            qDebug() << csstList.last() << csst13;
            csst = csstList.last() * csstRate1 + csst13 * csstRate2;
        }
        csstList.append(csst);

//        qDebug() << QString("css12[%1]: %2, css13[%1]: %3, csst[%1]: %4")
//                    .arg(i-aveNumb+1).arg(csst12).arg(csst13).arg(csst);

        double css = reboundValue(csst, maxCSS, minCSS);
        cssList.append(css);
    }

    if (!isPotentialEnergy) {
        for (int i = aveNumb; i < cssList.size()-1; ++i) {
            result.append((cssList[i] + cssList[i+1]) / 2);
        }
    } else {
        result = cssList;
    }

    return result;
}

QList<QList<double>> getAVEList(QList<double> oridata, QList<int> aveNumb, QList<bool> isEMAList) {
    QList<QList<double>> result;
    for (int i = 0; i < aveNumb.size(); ++i) {
        QList<double> emptyData;
        result.append(emptyData);
    }

    for (int i = 0; i < aveNumb.size(); ++i) {
        int currAveNumb = aveNumb[i];
        if (isEMAList[i]) {
            result[i].append(oridata[0]);
            for (int j = 1; j < oridata.size(); ++j) {
                double emaValue = result[i].last() * (currAveNumb - 1) / (currAveNumb + 1)
                                + oridata[j] * 2 / (currAveNumb + 1);
                result[i].append(emaValue);
            }
        } else {
            for (int j = 0; j < oridata.size(); ++j) {
                int startPos = max(0, j - currAveNumb+1);
                result[i].append(getAveValue(oridata, startPos, j+1));
            }
        }
    }
    return result;
}

QList<double> getAVEList(QList<double> oridata, int aveNumb, bool isEMAList) {
    QList<double> result;
    if (isEMAList) {
        result.append(oridata[0]);
        for (int j = 1; j < oridata.size(); ++j) {
            double emaValue = result.last() * (aveNumb - 1) / (aveNumb + 1)
                            + oridata[j] * 2 / (aveNumb + 1);
            result.append(emaValue);
        }
    } else {
        for (int j = 0; j < oridata.size(); ++j) {
            int startPos = max(0, j-aveNumb+1);
            result.append(getAveValue(oridata, startPos, j+1));
        }
    }
    return result;
}






























