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
#include <QDir>
#include <QFileInfo>
#include <QScrollBar>
#include "xlsxdocument.h"
#include "macd.h"
using std::sort;
using std::max;
using std::min;


// 20170911 -> [2017, 9, 11]
QList<int> getDateList(int intDate) {
    QList<int> dateList;
    int year = intDate / 10000;
    int month = (intDate - year * 10000) / 100;
    int day = intDate - year * 10000 - month * 100;
    dateList.append (year);
    dateList.append (month);
    dateList.append (day);
    return dateList;
}

// 112002 -> [11, 20 ,2]
QList<int> getTimeList(int intDate) {
    QList<int> timeList;
    int hour = intDate / 10000;
    int minute = (intDate - hour * 10000) / 100;
    int second = intDate - hour * 10000 - minute * 100;
    timeList.append (hour);
    timeList.append (minute);
    timeList.append (second);
    return timeList;
}

QString removePathName(QString fullFileName) {
    QStringList tmpList = fullFileName.split ("/");
    QString fileName = tmpList.at(tmpList.size ()-1);
    fileName.remove(".xlsx");
    return fileName;
}

bool compQPointF(QPointF pointA, QPointF pointB) {
    return pointA.x() < pointB.x();
}

QList<QPointF> sortPointFList(QList<QPointF> oridata) {
    sort(oridata.begin (), oridata.end (), compQPointF);
    return oridata;
}

bool compQString(QStringList strListA, QStringList strListB) {
    return strListA[0].toLongLong() < strListB[0].toLongLong();
}

QList<QStringList> sortQStringList(QList<QStringList> oridata) {
    sort(oridata.begin (), oridata.end (), compQString);
    return oridata;
}

QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
                                      QList<QPointF> secondList, int secondBuyCount) {

    int firstIter = 0;
    int secondIter = 0;
    QList<QPointF> result;
    while (firstIter < firstList.size() && secondIter < secondList.size ()) {
        if (firstList[firstIter].x () < secondList[secondIter].x ()) {
            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
            ++firstIter;
        } else if (firstList[firstIter].x () >  secondList[secondIter].x ()) {
            result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
            ++secondIter;
        } else {
            result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount + secondList[secondIter].y () * secondBuyCount));
            ++firstIter;
            ++secondIter;
        }
    }
    while (firstIter < firstList.size()) {
        result.append (QPointF(firstList[firstIter].x (), firstList[firstIter].y () * firstBuyCount));
        ++firstIter;
    }
    while( secondIter < secondList.size ()) {
        result.append (QPointF(secondList[secondIter].x (), secondList[secondIter].y () * secondBuyCount));
        ++secondIter;
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

void ErrorMessage(QString msg) {
    qDebug() << msg;
}

void WarnMessage(QWidget* window, QString title, QString msg) {
    QMessageBox::warning(window, title, msg);
}

QList<double> getChartYvalueRange(QList<QPointF> pointList ) {
    double maxValue = -1000000000000000000.0;
    double minValue = 10000000000000000000.0;
    for (int i = 0; i < pointList.size(); ++i) {
        maxValue = max(maxValue, pointList.at(i).y());
        minValue = min(minValue, pointList.at (i).y ());
    }

    int rangeInterval = 6;
    maxValue += (maxValue - minValue) / rangeInterval;
    minValue -= (maxValue - minValue) / rangeInterval;
    QList<double> result;
    result.append (minValue);
    result.append (maxValue);
    return result;
}

QList<double> getChartYvalueRange(QList<double> yValueList ) {
    QList<double> result;
    if (yValueList.size() == 0) {
        result.append(0);
        result.append(1);
    } else {
        double maxValue = -1000000000000000000.0;
        double minValue = 10000000000000000000.0;
        for (int i = 0; i < yValueList.size(); ++i) {
            maxValue = max(maxValue, yValueList[i]);
            minValue = min(minValue, yValueList[i]);
        }

        int rangeInterval = 6;
        maxValue += (maxValue - minValue) / rangeInterval;
        minValue -= (maxValue - minValue) / rangeInterval;

        if (maxValue == minValue) {
            double addedRange = abs(maxValue) / rangeInterval;
            maxValue = maxValue + addedRange;
            minValue = minValue - addedRange;
        }

//        if (abs(maxValue - minValue) / abs(minValue) < 0.2) {
//            maxValue += max(abs(maxValue),  abs(minValue)) / 4;
//            minValue -= max(abs(maxValue),  abs(minValue)) / 4;
//        }
        result.append (minValue);
        result.append (maxValue);
    }

    return result;
}

QList<double> getMACDRange(QList<MACD> oriData) {
    QList<double> result;
    if (oriData.size() == 0) {
        result.append(0);
        result.append(1);
    } else {
        double maxValue = -1000000000000000000.0;
        double minValue = 10000000000000000000.0;
        for (int i = 0; i < oriData.size(); ++i) {
            maxValue = max(max(maxValue, oriData[i].m_diff), max(oriData[i].m_dea, oriData[i].m_macd));
            minValue = min(min(minValue, oriData[i].m_diff), min(oriData[i].m_dea, oriData[i].m_macd));
        }

        int rangeInterval = 6;
        maxValue += (maxValue - minValue) / rangeInterval;
        minValue -= (maxValue - minValue) / rangeInterval;

        if (maxValue == minValue) {
            if (maxValue != 0) {
                double addedRange = abs(maxValue) / rangeInterval;
                maxValue = maxValue + addedRange;
                minValue = minValue - addedRange;
            } else {
                maxValue = 0.5;
                minValue = -0.5;
            }
        }

        result.append (minValue);
        result.append (maxValue);
    }

    return result;
}

QString transOffsetSecondsToTime(qint64 offSecs) {
    QDateTime tmpDatetime;
    tmpDatetime = tmpDatetime.toOffsetFromUtc(offSecs);
    return tmpDatetime.toString ();
}

QList<int> getNumbList(int dataNumb, int intervalNumb) {
    QList<int> result;
    if (dataNumb <= intervalNumb) {
        for (int i = 0; i < dataNumb; ++i) {
            result.append(i);
        }
    } else {
        int intervalLength = dataNumb / intervalNumb;
        int leftInterval = dataNumb % intervalNumb - 1;
        for (int i = 0, tmpIntervalNumb = 0; tmpIntervalNumb < intervalNumb; ++tmpIntervalNumb) {
            result.append(i);
            if (tmpIntervalNumb < leftInterval) {
                i += intervalLength + 1;
            } else {
                i += intervalLength;
            }
//            qDebug() << "i: " <<i;
        }
        result.append(dataNumb - 1);
    }
    return result;
}

void OutputMsg(QWidget* window,QString msg) {
    QMessageBox::information(window, "TableDataNumb", msg);
}

QList<QString> getAddedList(QList<QString> listA, QList<QString> listB) {
    QList<QString> result;

//    for(int i = 0; i < listA.size (); ++i) {
//        bool curAStringStrInListB = false;
//        for (int j = 0; j < listB.size(); ++j) {
//            if (listA[i] == listB[j]) {
//                curAStringStrInListB = true;
//                break;
//            }
//        }
//        if (!curAStringStrInListB) {
//            result.append (listA[i]);
//        }
//    }

    for(int i = 0; i < listB.size (); ++i) {
        bool curBStringStrInListA = false;
        for (int j = 0; j < listA.size(); ++j) {
            if (listB[i] == listA[j]) {
                curBStringStrInListA = true;
                break;
            }
        }
        if (!curBStringStrInListA) {
            result.append (listB[i]);
        }
    }
    return result;
}

QList<QString> getEXCELPidList() {
    QProcess p(0);
    p.start ("tasklist");
    p.waitForStarted();
    p.waitForFinished();

    QString taskStr = QString::fromLocal8Bit(p.readAllStandardOutput());
    QString processName = "EXCEL.EXE";
    return getProcessPid(taskStr, processName);
}

QList<QString> getProcessPid(QString taskStr, QString processName) {
    QStringList taskList = taskStr.split ("\r\n");
    QList<QString> result;
    for (int i = 0; i < taskList.size (); ++i) {
       if (taskList[i].startsWith(processName)) {
           QStringList tmpStrList = taskList[i].split(" ");
           tmpStrList.removeAll ("");
           result.append (tmpStrList[1]);
       }
    }
    return result;
}

void killProcessByPid(QString pid) {
    QProcess p(0);
    p.start ("TASKKILL /F /T /pid " + pid);
    p.waitForStarted();
    p.waitForFinished();

    QString taskStr = QString::fromLocal8Bit(p.readAllStandardOutput());
}

void updateProgramInfo(QTableView* programInfoTableView, QString message, QString remark) {
    QStandardItemModel* testMode = dynamic_cast<QStandardItemModel*>(programInfoTableView->model ());
    if (NULL != testMode) {
        QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
        int row = testMode->rowCount ();
        testMode->setItem (row, 0,  new QStandardItem(datetime));
        testMode->setItem (row, 1,  new QStandardItem(message));
        testMode->setItem (row, 2,  new QStandardItem(remark));
        QScrollBar* verScrollBar = programInfoTableView->verticalScrollBar();
        verScrollBar->setSliderPosition(verScrollBar->maximum());
    }
}

QString getWindSecode(QString secode) {
    QString windSecode;
    for (int j = 2; j < secode.size(); ++j) {
        windSecode.append(secode[j]);
    }
    windSecode.append('.');
    windSecode.append(secode[0]);
    windSecode.append(secode[1]);
    return windSecode;
}

QString getCompleteSecode(QString secode, QString style, QString letterCase) {
    while (secode.size() < 6) {
        secode.prepend("0");
    }

    if (secode.length() > 6) {
        return secode;
    }

    if (style == "tinysoft") {
        if (secode.startsWith("6")) {
            if (letterCase == "capital") {
                secode.prepend("SH");
            } else if (letterCase == "lower") {
                secode.prepend("sh");
            }
        } else {
            if (letterCase == "capital") {
                secode.prepend("SZ");
            } else if (letterCase == "lower") {
                secode.prepend("sz");
            }
        }
    }

    if (style == "wind") {
        if (secode.startsWith("6")) {
            if (letterCase == "capital") {
                secode.append(".SH");
            } else if (letterCase == "lower") {
                secode.append(".sh");
            }
        } else {
            if (letterCase == "capital") {
                secode.append(".SZ");
            } else if (letterCase == "lower") {
                secode.append(".sz");
            }
        }
    }

    return secode;
}

QString getSimpleSecode(QString secode) {
//    qDebug() << "secode: " << secode;
    while(secode.startsWith("0")) {
        secode.remove(0,1);
    }
//    qDebug() << "secode: " << secode;
    return secode;
}

QString getCompleteIndexCode(QString secode, QString style) {
    while (secode.size() < 6) {
        secode.prepend("0");
    }

    if (style == "tinysoft") {
        if (secode.startsWith("0")) {
            secode.prepend("SH");
        } else {
            secode.prepend("SZ");
        }
    }

    if (style == "wind") {
        if (secode.startsWith("0")) {
            secode.append(".SH");
        } else {
            secode.append(".SZ");
        }
    }

    return secode;
}

QMap<QString, int> transStrategyMap(QMap<QString, int> oriStrategyMap, QString codeStyle) {
    QMap<QString, int> result;
    for (QMap<QString, int>::const_iterator i = oriStrategyMap.begin(); i != oriStrategyMap.end(); ++i) {
        result.insert(getCompleteSecode(i.key(), codeStyle), i.value());
    }
    return result;
}

double getAveValue(QList<double> oriData) {
    double result = 0;
    for (int i = 0; i < oriData.size(); ++i) {
        result += oriData[i];
    }
    return result/oriData.size();
}

QTime StockAmStartTime() {
    return QTime(9, 30, 10);
}

QTime StockAmStopTime() {
    return QTime(11, 30, 5);
}

QTime StockPmStartTime() {
    return QTime(13, 0, 10);

}

QTime StockPmStopTime() {
    return QTime(15, 0, 5);
}

bool isStockTrading() {
    if ((QTime::currentTime() >= StockAmStartTime() && QTime::currentTime() <= StockAmStopTime()) ||
         (QTime::currentTime() >= StockPmStartTime() && QTime::currentTime() <= StockPmStopTime())) {
        return true;
    } else {
        return false;
    }
}

bool isStockTradingNotStart() {
    return QTime::currentTime() < StockAmStartTime();
}

bool isStockNoonBreak() {
  return QTime::currentTime() > StockAmStopTime() && QTime::currentTime() <StockPmStartTime();
}

bool isStockTradingOver() {
    return QTime::currentTime() > StockPmStopTime();
}

bool isTradingTime(QTime time) {
    QTime amStartTime = QTime(9,30,30);
    QTime amStopTime = QTime(11, 30, 0);
    QTime pmStopTime = QTime(15, 0, 0);
    QTime pmStartTime = QTime(13, 0, 0);

    if ((time >= amStartTime && time < amStopTime)||
         (time >= pmStartTime && time < pmStopTime)) {
        return true;
    } else {
        return false;
    }
}

bool isTradingOver(QTime time) {
    QTime pmStopTime = QTime(15, 0, 20);
    if (time >= pmStopTime){
        return true;
    } else {
        return false;
    }
}

bool isTradingStart(QTime time) {
    QTime amStartTime = QTime(9,30,6);
    if (amStartTime > time) {
        return false;
    } else {
        return true;
    }
}

void testCLosePrice(QMap<QString, QList<QStringList>> oriData) {
    QList<QString> secodeList = oriData.keys();
    for (int i = 0; i < secodeList.size(); ++i) {
        QString secode = secodeList[i];
        QList<QStringList> currData = oriData[secode];
        if (currData.size() == 0) {
            qDebug() << "secode : " << secode << " has no data";
        } else {
            qDebug() << secode << ", TCLOSE: " << currData[currData.size()-1];
        }
    }
}

QList<QString> getExcelFileName(QString dirName) {
    QDir *dir=new QDir(dirName);
    QStringList filter;
    filter<<"*.xlsx";
    dir->setNameFilters(filter);
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    QList<QString> filaPathList;

    for (int i = 0; i < fileInfo->count (); ++i) {
        filaPathList.append (fileInfo->at(i).filePath());
    }
    delete dir;
    delete fileInfo;
    return filaPathList;
}

QList<QFileInfo> getExcelFileInfo(QString dirName) {
    QDir *dir=new QDir(dirName);
    QStringList filter;
    filter<<"*.xlsx";
    dir->setNameFilters(filter);
    QList<QFileInfo> filaInfoList = dir->entryInfoList(filter);
    return filaInfoList;
}

QList<QFileInfo> getDirName(QString dirName) {
    QDir *dir=new QDir(dirName);
    dir->setFilter(QDir::Dirs|QDir::Files);
    dir->setSorting(QDir::DirsFirst);
    QFileInfoList fileInfo = dir->entryInfoList();

//    qDebug() << "fileInfo.size: " << fileInfo.size();
    QList<QFileInfo> dirList;

    for (int i = 0; i < fileInfo.count (); ++i) {
        if (fileInfo.at(i).isDir()) {
            dirList.append (fileInfo.at(i));
        }
    }
    return dirList;
}

QStandardItemModel* getStandardItemModel(QList<QString> valueList) {
    QStandardItemModel* standardItemModel = new QStandardItemModel();
    for (int i = 0; i < valueList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(valueList[i]));
    }
    return standardItemModel;
}

QStandardItemModel* getStandardItemModel(QList<QFileInfo> fileInfoList) {
    QStandardItemModel* standardItemModel = new QStandardItemModel();
    for (int i = 0; i < fileInfoList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(fileInfoList[i].fileName()));
    }
    return standardItemModel;
}

qint64 transDateTime(double oridata) {
    long long  datetime = long long (oridata);
    int year = datetime / 10000000000;
    datetime = datetime % 10000000000;
    int month = datetime / 100000000;
    datetime = datetime % 100000000;
    int day = datetime / 1000000;
    datetime = datetime % 1000000;
    int hour = datetime / 10000;
    datetime = datetime % 10000;
    int miniute = datetime / 100;
    datetime = datetime % 100;
    int secs = datetime;
//    qDebug() << "oriData: " << oridata;
//    qDebug() << "year: " << year << ", month: " << month << ", day: " << day << ", "
//             << "hour: " << hour << ", miniute: " << miniute << ", secs: " << secs;
    QDate curDate = QDate(year, month, day);
    QTime curTime = QTime(hour, miniute, secs);
    QDateTime DateTime = QDateTime(curDate, curTime);
//    qDebug()<< "DateTime: " << DateTime;
    return DateTime.toMSecsSinceEpoch();
}

QList<QPointF> getStrategyPointList(QMap<QString, QList<QStringList>> oriData,
                                    QMap<QString, int> seocdebuyCountMap) {
    QList<QString> keyList = seocdebuyCountMap.keys ();
    QList<QPointF> pointDataList;
    for (int i = 0; i < keyList.size (); ++i) {
        QString key = keyList[i];
        QList<QStringList> tmpStringList = oriData[key];
        QList<QPointF> tmpPointData;
        for (int i = 0; i < tmpStringList.size (); ++i) {
            tmpPointData.append (QPointF(tmpStringList[i][0].toDouble(),
                                 tmpStringList[i][1].toDouble() * seocdebuyCountMap[key]));
        }
        tmpPointData = sortPointFList(tmpPointData);
        pointDataList = mergeSortedPointedList (pointDataList, 1, tmpPointData, 1);
    }
    return pointDataList;
}

QList<double> getHedgedData(QList<QPointF> pointDataList, QMap<QString, QStringList> indexHedgeData,
                            int indexBuyCount, int indexBaseCount) {
    QList<double> strategyData;
    QList<QString> indexHedgedataTimeKey = indexHedgeData.keys ();
    for (int i = 0; i < pointDataList.size(); ++i) {
        QString timeKeyStr = QDateTime::fromMSecsSinceEpoch(qint64(pointDataList[i].x())).toString ("yyyyMMddhhmmss");
        if (indexHedgedataTimeKey.indexOf (timeKeyStr) >= 0) {
            strategyData.append(pointDataList[i].y() / (indexBuyCount * indexBaseCount)
                                - indexHedgeData[timeKeyStr][0].toDouble());
        } else {
            strategyData.append(pointDataList[i].y() / (indexBuyCount* indexBaseCount));
        }
    }
    return strategyData;
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
    strategyData = strategyData / (indexBuyCount * indexBaseCount) - oneTimeData[indexCode][2].toDouble();

    result.append(strategyData);
    result.append(votData);
    result.append(timeData);
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
            result += oriData[secode][0].toDouble() * i.value();
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

QMap<QString, int> EmpytQStringIntMap() {
    QMap<QString, int> data;
    return data;
}

QStringList EmpytStringList() {
    QStringList data;
    return data;
}

bool isSecodeValid(QString secode) {
    bool result = true;
    if (secode == "") {
        result = false;
    }

    return result;
}

void writePortfolio(QMap<QString, double> buyPortfolio, QMap<QString, double> salePortfolio, QString fileName) {

}

void initExcelFile(QString fileName) {
    QFile currFile(fileName);
    if (!currFile.exists()) {
        QXlsx::Document initxlsx;
        initxlsx.saveAs(fileName);

        QXlsx::Document xlsx(fileName);
        QStringList sheetNames = xlsx.sheetNames();
//        qDebug() << "init sheetNames: " << sheetNames;
        for (QString sheetname : sheetNames) {
            if (xlsx.deleteSheet(sheetname)) {
//                qDebug() << "delete " << sheetname << " successfully!";
            } else {
//                qDebug() << "delete " << sheetname << " failed!";
            }
        }
        xlsx.save();
    }
}

int writePortfolio(QMap<QString, double> portfolio, QString fileName,
                    QString accountName, QString type, QString sheetName) {
    initExcelFile(fileName);

    QXlsx::Document xlsx(fileName);
    QStringList sheetNames = xlsx.sheetNames();

    if (sheetNames.indexOf(sheetName) < 0) {
        if (!xlsx.addSheet(sheetName)) {
            return -1;
        }
    }

    xlsx.moveSheet(sheetName, 0);

    if (!xlsx.selectSheet(sheetName)) {
        return -2;
    }

    if (accountName == "ori" || accountName == "") {
        int index = 1;
        for (QMap<QString, double>::iterator it = portfolio.begin();
             it != portfolio.end(); ++it, ++index) {
            xlsx.write(index, 1, it.key());
            xlsx.write(index, 2, it.value());
        }
    }

    if (accountName.indexOf("1") >= 0) {
        xlsx.write("A1", QString::fromLocal8Bit("买卖"));
        xlsx.write("B1", QString::fromLocal8Bit("证券代码"));
        xlsx.write("C1", QString::fromLocal8Bit("数量"));

        QString keyWord;
        if (type == "buy") {
            keyWord =  QString::fromLocal8Bit("买入");
        } else if (type == "sale") {
            keyWord =  QString::fromLocal8Bit("卖出");
        }

        int index = 2;
        for (QMap<QString, double>::iterator it = portfolio.begin();
             it != portfolio.end(); ++it) {
            xlsx.write(QString("A%1").arg(index), keyWord);
            xlsx.write(QString("B%1").arg(index), getCompleteSecode(it.key(), "tinysoft", "lower"));
            xlsx.write(QString("C%1").arg(index), it.value());
            ++index;
        }
    }

    if (accountName.indexOf("2") >= 0) {
        xlsx.write("A1", QString::fromLocal8Bit("证券代码"));
        xlsx.write("B1", QString::fromLocal8Bit("证券名称"));
        xlsx.write("C1", QString::fromLocal8Bit("交易市场"));
        xlsx.write("D1", QString::fromLocal8Bit("委托方向"));
        xlsx.write("E1", QString::fromLocal8Bit("价格类型"));
        xlsx.write("F1", QString::fromLocal8Bit("委托价格"));
        xlsx.write("G1", QString::fromLocal8Bit("委托数量"));
        xlsx.write("H1", QString::fromLocal8Bit("委托金额"));

        QString keyWord;
        if (type == "buy") {
            keyWord = "1";
        } else if (type == "sale") {
            keyWord = "2";
        }
//        qDebug() << accountName << ", keyWord: " << keyWord;
        int index = 2;
        for (QMap<QString, double>::iterator it = portfolio.begin();
             it != portfolio.end(); ++it) {
            xlsx.write(QString("A%1").arg(index), getSimpleSecode(it.key()));
            xlsx.write(QString("D%1").arg(index), keyWord);
            xlsx.write(QString("G%1").arg(index), it.value());
            ++index;
        }
    }

    xlsx.save();

//    qDebug() << "sheetNames: " << xlsx.sheetNames();

    return 1;
}

int writeFileInfo(QString fileName, QString colName, QString value, QString sheetName) {
    QFile tmpfile(fileName);
    if (tmpfile.exists()) {
        QXlsx::Document xlsx(fileName);
        if (!xlsx.selectSheet(sheetName)) {
            if (!xlsx.addSheet(sheetName)) return -2;
            if (!xlsx.selectSheet(sheetName)) return -1;
        }
        xlsx.write(colName, value);
        xlsx.save();
    } else {
        QXlsx::Document xlsx;
        if (!xlsx.selectSheet(sheetName)) {
            if (!xlsx.addSheet(sheetName)) return -2;
            if (!xlsx.selectSheet(sheetName)) return -1;
        }
        xlsx.write(colName, value);
        xlsx.saveAs(fileName);
    }
    return 1;
}

QString readlFileData(QString fileName, int row, int col,
                      QString sheetName) {
    QXlsx::Document xlsx(fileName);
    if (!xlsx.selectSheet(sheetName)) {
        return "NULL";
    } else {
        if (xlsx.cellAt(row, col) != 0) {
            return xlsx.cellAt(row, col)->value().toString();
        } else {
            return "NULL";
        }
    }
}

QMap <int, int> getSelectRows(QTableView* tableView)
{
    QItemSelectionModel *selections =  tableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    QMap<int, int> result;
    foreach (QModelIndex index, selected)
    {
        result.insert(index.row(),index.column());
    }
   return result;
}

QList<QString> getSelectTableData(QTableView* tableView, QMap<int, int> selectInfo) {
    QList<QString> result;
    QAbstractItemModel* itemModel = tableView->model ();
    for (QMap<int, int>::iterator it = selectInfo.begin();
         it != selectInfo.end(); ++it) {
        QModelIndex indextemp = itemModel->index(it.key(), it.value());
        QString data = itemModel->data(indextemp).toString();
        result.append(data);
    }
    return result;
}

QMap<QString, double> readExcelMapDouble(QString fileName, QString sheetName) {
    QList<QStringList> oridata = readExcelData(fileName, sheetName);
    QMap<QString, double> result;
    for (int i = 0; i < oridata.size(); ++i) {
        if (oridata[i].size() < 2) {
            result.insert("Error", -1);
            break;
        } else {
            for (int j = 0; j < 2; ++j) {
                result.insert(oridata[i][0], oridata[i][1].toDouble());
            }
        }
    }
    return result;
}

QMap<QString, int> readExcelMapInt(QString fileName, QString sheetName) {
    QList<QStringList> oridata = readExcelData(fileName, sheetName);
    QMap<QString, int> result;
    for (int i = 0; i < oridata.size(); ++i) {
        if (oridata[i].size() < 2) {
            result.insert("Error", -1);
            break;
        } else {
            for (int j = 0; j < 2; ++j) {
                result.insert(oridata[i][0], oridata[i][1].toInt());
            }
        }
    }
    return result;
}

QList<QStringList> readExcelData(QString fileName, QString sheetName) {
    QList<QStringList> result;
    QXlsx::Document xlsx(fileName);
    xlsx.selectSheet(sheetName);
    QXlsx::CellRange range = xlsx.dimension();
    int rowCount = range.rowCount();
    int colCount = range.columnCount();
    qDebug() <<"fileName: " << fileName <<"rowCount: " << rowCount << ", colCount" << colCount;

//    int defaultColNumb = 2;
//    if (colCount < defaultColNumb) {
//        defaultColNumb = colCount;
//    }

    for (int i = 1; i <= rowCount; i++){
        QStringList tmpData;
         for (int j = 1; j <= colCount; j++){
             if(int(xlsx.cellAt(i,j)) != 0) {
                 tmpData.append(xlsx.cellAt(i, j)->value().toString());
             } else {
                 break;
             }
         }
         if (tmpData.size() > 0) {
            result.append(tmpData);
         }
     }
    return result;
}

double getPortfolioAmount(QMap<QString, double> priceMap, QMap<QString, double> porfolioMap) {
    double sum = 0.0;
    for (QMap<QString, double>::iterator it = porfolioMap.begin();
         it != porfolioMap.end(); ++it) {
        sum += priceMap[it.key()] * it.value();
    }
    return sum;
}

QMap<QString, QString> getPortfolioAmountMap(QList<QFileInfo> fileInfoList) {
    QMap<QString, QString> result;
    for (QFileInfo tmpFileInfo : fileInfoList) {
        result.insert(tmpFileInfo.fileName(), readlFileData(tmpFileInfo.filePath(), 1, 1, "Sheet2"));
    }
    return result;
}

//void writePortfolio(QMap<QString, double> portfolio, QString fileName,
//                    QString accountName, QString type) {
//    QXlsx::Document xlsx;
//    QStringList sheetNames = xlsx.sheetNames();
//    if (sheetNames.indexOf(sheetName) >= 0) {
//        xlsx.selectSheet(sheetName);
//    }

//    if (accountName == "ori" || accountName == "") {
//        int index = 0;
//        for (QMap<QString, double>::iterator it = portfolio.begin();
//             it != portfolio.end(); ++it) {
//            xlsx.write(QString("A%1").arg(++index), it.key());
//            xlsx.write(QString("B%1").arg(index), it.value());
//        }
//    }

//    if (accountName.indexOf("1") >= 0) {
//        xlsx.write("A1", QString::fromLocal8Bit("买卖"));
//        xlsx.write("B1", QString::fromLocal8Bit("证券代码"));
//        xlsx.write("C1", QString::fromLocal8Bit("数量"));

//        QString keyWord;
//        if (type == "buy") {
//            keyWord =  QString::fromLocal8Bit("买入");
//        } else if (type == "sale") {
//            keyWord =  QString::fromLocal8Bit("卖出");
//        }

////        qDebug() << accountName << ", keyWord: " << keyWord;

//        int index = 2;
//        for (QMap<QString, double>::iterator it = portfolio.begin();
//             it != portfolio.end(); ++it) {
//            xlsx.write(QString("A%1").arg(index), keyWord);
//            xlsx.write(QString("B%1").arg(index), getCompleteSecode(it.key(), "tinysoft", "lower"));
//            xlsx.write(QString("C%1").arg(index), it.value());
//            ++index;
//        }
//    }

//    if (accountName.indexOf("2") >= 0) {
//        xlsx.write("A1", QString::fromLocal8Bit("证券代码"));
//        xlsx.write("B1", QString::fromLocal8Bit("证券名称"));
//        xlsx.write("C1", QString::fromLocal8Bit("交易市场"));
//        xlsx.write("D1", QString::fromLocal8Bit("委托方向"));
//        xlsx.write("E1", QString::fromLocal8Bit("价格类型"));
//        xlsx.write("F1", QString::fromLocal8Bit("委托价格"));
//        xlsx.write("G1", QString::fromLocal8Bit("委托数量"));
//        xlsx.write("H1", QString::fromLocal8Bit("委托金额"));

//        QString keyWord;
//        if (type == "buy") {
//            keyWord = "1";
//        } else if (type == "sale") {
//            keyWord = "2";
//        }
////        qDebug() << accountName << ", keyWord: " << keyWord;
//        int index = 2;
//        for (QMap<QString, double>::iterator it = portfolio.begin();
//             it != portfolio.end(); ++it) {
//            xlsx.write(QString("A%1").arg(index), getSimpleSecode(it.key()));
//            xlsx.write(QString("D%1").arg(index), keyWord);
//            xlsx.write(QString("G%1").arg(index), it.value());
//            ++index;
//        }
//    }

//    xlsx.saveAs(fileName);
//}

//extern QMap<QString, double> g_seocdebuyCountMap;
//extern QList<QString> g_secodeNameList;

//void testSpread(QMap<QString, QStringList> data) {
//    double strategyData = 0;
//    QString hedgeIndexCode = "000300.SH";
//    int hedgeIndexCount = 2;
//    for (int i = 0; i < g_secodeNameList.size(); ++i) {
//        QString secode = g_secodeNameList[i];
//        if (secode == hedgeIndexCode) {
//            continue;
//        }
//        if (data[secode][2] == "0.0000") {
//            strategyData += data[secode][3].toDouble() * g_seocdebuyCountMap[secode];
//        } else {
//            strategyData += data[secode][2].toDouble() * g_seocdebuyCountMap[secode];
//        }
//    }

//    strategyData = strategyData / (hedgeIndexCount * 300) - data[hedgeIndexCode][2].toDouble();
//    qDebug() << "spread: " << strategyData << ", time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
//}

////功能：WSQ回调函数输出行情数值至控制台
//int g_count = 0;
//LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd)
//{
//    qDebug() << "Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
//             << ", g_count: " << ++g_count << ", reqid: " << reqid;
//    reqid;
//    int codelen = wd.GetCodesLength();
//    int fieldslen = wd.GetFieldsLength();
//    int colnum = fieldslen + 1;
//    QMap<QString, QStringList> tmpResult;
//    cout  << "WindCodes    ";
//    for (int i =1;i < colnum; ++i)
//    {
//        QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
//        cout << outfields.toStdString() << "    ";
//    }
//    cout << endl;
//    for (int i = 0; i < codelen; ++i)
//    {
//        QStringList singleCodeData;
//        QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
////        cout << codes.toStdString() << "    ";
//        for (int j = 0; j < fieldslen; ++j)
//        {
//            VARIANT var;
//            wd.GetDataItem(0,i,j,var);
//            QString temp = variantToQString(&var);
//            singleCodeData.append(temp);
//            cout << temp.toStdString() << "    ";
//        }
//        cout << endl;
//        writeWsqData(codes, singleCodeData);
//        tmpResult.insert(codes, singleCodeData);
//    }
////    testSpread(tmpResult);
//    return 0;
//}

//QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList) {
//    bool bOutputMsg = false;
////    qDebug() << "toolFunc::wsqSnaphootData";
////    qDebug() << "Thread: " << QThread::currentThreadId();
//    QMap<QString, QStringList> result;
//    QTime curTime = QTime::currentTime();
//    QTime amStopTime = QTime(11, 30, 10);
//    QTime pmStartTime = QTime(13, 0, 0);
//    if (curTime >= amStopTime && pmStartTime >= curTime) return result;
//    int errcode;
//    if (true) {
//        WindData wd;
//        LPCWSTR windcodes = transSecodeB(secodeList);
//        if (bOutputMsg) wcout << "windcodes: " << windcodes << endl;
//        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_latest,rt_pre_close,rt_amt");
//        LPCWSTR options = TEXT("");
//        errcode = CWAPIWrapperCpp::wsq(wd, windcodes, indicators, options);
//        if (bOutputMsg) qDebug() << "startWsqOneTime errcode: " << errcode;
//        if (errcode == 0) {
//            int codelen = wd.GetCodesLength();
//            int fieldslen = wd.GetFieldsLength();
//            int colnum = fieldslen + 1;
//            if (bOutputMsg)  cout  << "WindCodes    ";
//            for (int i =1;i < colnum; ++i)
//            {
//                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
//                if (bOutputMsg) cout << outfields.toStdString() << "    ";
//            }
//            if (bOutputMsg) cout << endl;
//            for (int i = 0; i < codelen; ++i)
//            {
//                QStringList singleCodeData;
//                QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
//                if (bOutputMsg)  cout << codes.toStdString() << "    ";
//                for (int j = 0; j < fieldslen; ++j)
//                {
//                    VARIANT var;
//                    wd.GetDataItem(0,i,j,var);
//                    QString temp = variantToQString(&var);
//                    singleCodeData.append(temp);
//                    if (bOutputMsg) cout << temp.toStdString() << "    ";
//                }
//                if (bOutputMsg) cout << endl;
//                result.insert(codes, singleCodeData);
//            }
//        } else {
////            emit startWsqFailed(errcode, reqID);
//        }
//        delete[] windcodes;
//    } else {
//        errcode = -1;
//        qDebug() << "Login first!";
//    }
//    return result;
//}

//QString variantToQString(const LPVARIANT data)
//{
//    QString str;
//    switch (data->vt)
//    {
//    case VT_INT:
//        str = QString::number(data->intVal);
//        break;
//    case VT_I4:
//        str = QString::number(data->lVal);
//        break;
//    case VT_I8:
//        str = QString::number(data->llVal);
//        break;
//    case VT_R4:
//        str = QString("%1").arg(data->fltVal, 0, 'f', 4);
//        break;
//    case VT_R8:
//        str = QString("%1").arg(data->dblVal, 0, 'f', 4);
//        break;
//    case VT_EMPTY:
//        break;
//    case VT_BSTR:
//        str = QString((QChar*)data->bstrVal, (int)(wcslen(data->bstrVal)));
//    break;
//    case VT_DATE:
////        str = QString::fromUtf16((ushort*)LPCTSTR(COleDateTime(data->date).Format(L"%Y%m%d%h%m%s")));
//        str = QString("%1").arg(data->date);
//        break;
//    }
//    return str;
//}

//LPCWSTR transSecode(QStringList secodeList) {
//    QString result="";
//    for (int i = 0; i < secodeList.size(); ++i) {
//        QString curSecode = secodeList[i];
//        QString transSecode;
//        for (int j = 2; j < curSecode.size(); ++j) {
//            transSecode.append(curSecode[j]);
//        }
//        transSecode.append('.');
//        transSecode.append(curSecode[0]);
//        transSecode.append(curSecode[1]);
//        result += transSecode + ',';
//    }
//    result.remove(result.size()-1, result.size()-1);

//    string data = result.toStdString();
//    int size = static_cast<int>(data.length());
//    wchar_t *buffer = new wchar_t[size + 1];
//    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
//    buffer[size] = 0;  //end with '\0'
//    return buffer;
//}

//LPCWSTR transSecodeB(QStringList secodeList) {
//    QString result="";
//    for (int i = 0; i < secodeList.size(); ++i) {
//        result += secodeList[i] + ',';
//    }
//    result.remove(result.size()-1, result.size()-1);

//    string data = result.toStdString();
//    int size = static_cast<int>(data.length());
//    wchar_t *buffer = new wchar_t[size + 1];
//    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
//    buffer[size] = 0;  //end with '\0'
//    return buffer;
//}

//LPCWSTR transSecode(QString qString) {
//    string data = qString.toStdString();
//    int size = static_cast<int>(data.length());
//    wchar_t *buffer = new wchar_t[size + 1];
//    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
//    buffer[size] = 0;  //end with '\0'
//    return buffer;
//}






