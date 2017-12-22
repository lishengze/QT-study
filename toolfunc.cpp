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
#include "macd.h"
using std::sort;
using std::max;
using std::min;

extern QMap<QString, QList<QStringList>> g_wsqData;
extern QMutex g_wsqMutex;

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
        programInfoTableView->setRowHeight (row, 20);
    }
}

LPCWSTR transSecode(QStringList secodeList) {
    QString result="";
    for (int i = 0; i < secodeList.size(); ++i) {
        QString curSecode = secodeList[i];
        QString transSecode;
        for (int j = 2; j < curSecode.size(); ++j) {
            transSecode.append(curSecode[j]);
        }
        transSecode.append('.');
        transSecode.append(curSecode[0]);
        transSecode.append(curSecode[1]);
        result += transSecode + ',';
    }
    result.remove(result.size()-1, result.size()-1);

    string data = result.toStdString();
    size_t size = data.length();
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //确保以 '\0' 结尾
    return buffer;
}

LPCWSTR transSecode(QString qString) {
    string data = qString.toStdString();
    size_t size = data.length();
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //确保以 '\0' 结尾
    return buffer;
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

QString variantToQString(const LPVARIANT data)
{
    QString str;
    switch (data->vt)
    {
    case VT_INT:
        str = QString::number(data->intVal);
        break;
    case VT_I4:
        str = QString::number(data->lVal);
        break;
    case VT_I8:
        str = QString::number(data->llVal);
        break;
    case VT_R4:
        str = QString("%1").arg(data->fltVal, 0, 'f', 4);
        break;
    case VT_R8:
        str = QString("%1").arg(data->dblVal, 0, 'f', 4);
        break;
    case VT_EMPTY:
        break;
    case VT_BSTR:
        str = QString((QChar*)data->bstrVal, wcslen(data->bstrVal));
    break;
    case VT_DATE:
//        str = QString::fromUtf16((ushort*)LPCTSTR(COleDateTime(data->date).Format(L"%Y%m%d%h%m%s")));
        str = QString("%1").arg(data->date);
        break;
    }
    return str;
}

//功能：WSQ回调函数输出行情数值至控制台
//int g_count = 0;
LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd)
{
//    cout <<"reqid: " << reqid << " ++g_count: " << ++g_count << endl;
//    cout <<"reqid: " << reqid << " --g_count: " << --g_count << endl;
//    cout << "reqid: " << reqid << endl;
    int codelen = wd.GetCodesLength();
    int fieldslen = wd.GetFieldsLength();
    int colnum = fieldslen + 1;
//    cout  << "WindCodes    ";
//    for (int i =1;i < colnum; ++i)
//    {
//        QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
//        cout << outfields.toStdString() << "    ";
//    }
//    cout << endl;
    for (int i = 0; i < codelen; ++i)
    {
        QStringList singleCodeData;
        QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
//        cout << codes.toStdString() << "    ";
        for (int j = 0; j < fieldslen; ++j)
        {
            VARIANT var;
            wd.GetDataItem(0,i,j,var);
            QString temp = variantToQString(&var);
            singleCodeData.append(temp);
//            cout << temp.toStdString() << "    ";
        }
//        cout << endl;
        writeWsqData(codes, singleCodeData);
    }
    return 0;
}

void writeWsqData(QString secode, QStringList data) {
    QList<QString> keys = g_wsqData.keys();
    if (keys.indexOf(secode) < 0) {
        QList<QStringList> initData;
        initData.append(data);
        g_wsqData.insert(secode, initData);
    }
    QList<QStringList> currData = g_wsqData[secode];
    QStringList latestData = currData[currData.size()-1];
    if (latestData[1].toDouble() < data[1].toDouble()) {
        g_wsqData[secode].append(data);
    }
//    qDebug() << g_wsqData;
}

double getAveValue(QList<double> oriData) {
    double result = 0;
    for (int i = 0; i < oriData.size(); ++i) {
        result += oriData[i];
    }
    return result/oriData.size();
}
















