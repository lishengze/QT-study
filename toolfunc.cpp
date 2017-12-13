#include "toolfunc.h"
#include <QStringList>
#include <algorithm>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QWidget>
#include <QStringList>
#include <QProcess>
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

void ErrorMessage(QString msg) {
    qDebug() << msg;
}

void WarnMessage(QWidget* window, QString title, QString msg) {
    QMessageBox::warning(window, title, msg);
}

QList<double> getMACDRange(QList<MACD> oriData) {
    double maxValue = -1000000000000000000.0;
    double minValue = 10000000000000000000.0;
    for (int i = 0; i < oriData.size(); ++i) {
        maxValue = max(max(maxValue, oriData[i].m_diff), max(oriData[i].m_dea, oriData[i].m_macd));
        minValue = min(min(minValue, oriData[i].m_diff), min(oriData[i].m_dea, oriData[i].m_macd));
    }

    int rangeInterval = 6;
    maxValue += (maxValue - minValue) / rangeInterval;
    minValue -= (maxValue - minValue) / rangeInterval;
    QList<double> result;
    result.append (minValue);
    result.append (maxValue);
    return result;
}

QString transOffsetSecondsToTime(qint64 offSecs) {
    QDateTime tmpDatetime;
    tmpDatetime = tmpDatetime.toOffsetFromUtc(offSecs);
    return tmpDatetime.toString ();
}

QList<int> getNumbList(int dataNumb, int desNumb) {
    QList<int> result;
    int interval = (dataNumb-1) / (desNumb-1);
    for (int i = interval; i < dataNumb; i += interval) {
        result.append(i);
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























