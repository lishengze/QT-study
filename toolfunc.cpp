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
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QScrollBar>
#include <QFileInfo>
#include "xlsxdocument.h"
//#include "macd.h"
using std::sort;
using std::max;
using std::min;


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
        QStringList messageList = message.split("\n");
        messageList.removeAll("");
        messageList.removeAll(" ");
        for (QString msg:messageList) {
            QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
            int row = testMode->rowCount ();
            testMode->setItem (row, 0,  new QStandardItem(datetime));
            testMode->setItem (row, 1,  new QStandardItem(msg));
            testMode->setItem (row, 2,  new QStandardItem(remark));
            QScrollBar* verScrollBar = programInfoTableView->verticalScrollBar();
            verScrollBar->setSliderPosition(verScrollBar->maximum());
        }
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
    int size = static_cast<int>(data.length());
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //end with '\0'
    return buffer;
}

LPCWSTR transSecodeList(QStringList secodeList) {
    QString result="";
    for (int i = 0; i < secodeList.size(); ++i) {
        result += secodeList[i] + ',';
    }
    result.remove(result.size()-1, result.size()-1);

    string data = result.toStdString();
    int size = static_cast<int>(data.length());
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //end with '\0'
    return buffer;
}

LPCWSTR transSecode(QString qString) {
    string data = qString.toStdString();
    int size = static_cast<int>(data.length());
    wchar_t *buffer = new wchar_t[size + 1];
    MultiByteToWideChar(CP_ACP, 0, data.c_str(), size, buffer, size * sizeof(wchar_t));
    buffer[size] = 0;  //end with '\0'
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

QList<QString> getIndexCode(QString style) {
    QList<QString> indexCodeList;
    if (style == "wind") {
        indexCodeList << "000300.SH" << "000016.SH" << "000852.SH"
                      << "000904.SH" << "000905.SH" << "000906.SH" << "399903.SZ";
    }
    if (style == "tinysoft") {
        indexCodeList  << "SH000300" << "SH000016" << "SH000852"
                        << "SH000904" << "SH000905" << "SH000906" <<"SZ399903";
    }
    if (style == "ori") {
        indexCodeList << "000300" << "000016" << "000852"
                       << "000904" << "000905" << "000906" << "399903";
    }

    return indexCodeList;
}

QString completeSecode(QString secode, QString style) {
    while (secode.size() < 6) {
        secode.prepend("0");
    }

    if (style == "tinysoft") {
        if (secode.startsWith("6")) {
            secode.prepend("SH");
        } else {
            secode.prepend("SZ");
        }
    }

    if (style == "wind") {
        if (secode.startsWith("6")) {
            secode.append(".SH");
        } else {
            secode.append(".SZ");
        }
    }

    return secode;
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
        str = QString((QChar*)data->bstrVal, (int)(wcslen(data->bstrVal)));
    break;
    case VT_DATE:
//        str = QString::fromUtf16((ushort*)LPCTSTR(COleDateTime(data->date).Format(L"%Y%m%d%h%m%s")));
        str = QString("%1").arg(data->date);
        break;
    }
    return str;
}

//功能：WSQ回调函数输出行情数值至控制台
int g_count = 0;
LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd)
{
    qDebug() << "Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
             << ", g_count: " << ++g_count << ", reqid: " << reqid;
    reqid;
    int codelen = wd.GetCodesLength();
    int fieldslen = wd.GetFieldsLength();
    int colnum = fieldslen + 1;
    QMap<QString, QStringList> tmpResult;
    cout  << "WindCodes    ";
    for (int i =1;i < colnum; ++i)
    {
        QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
        cout << outfields.toStdString() << "    ";
    }
    cout << endl;
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
            cout << temp.toStdString() << "    ";
        }
        cout << endl;
//        writeWsqData(codes, singleCodeData);
        tmpResult.insert(codes, singleCodeData);
    }
//    testSpread(tmpResult);
    return 0;
}

double getAveValue(QList<double> oriData) {
    double result = 0;
    for (int i = 0; i < oriData.size(); ++i) {
        result += oriData[i];
    }
    return result/oriData.size();
}

QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList) {
    bool bOutputMsg = true;
//    qDebug() << "toolFunc::wsqSnaphootData";
//    qDebug() << "Thread: " << QThread::currentThreadId();
    QMap<QString, QStringList> result;
    QTime curTime = QTime::currentTime();
    QTime amStopTime = QTime(11, 30, 10);
    QTime pmStartTime = QTime(13, 0, 0);
    if (curTime >= amStopTime && pmStartTime >= curTime) return result;
    int errcode;
    if (true) {
        WindData wd;
        LPCWSTR windcodes = transSecodeList(secodeList);
//        LPCWSTR windcodes = TEXT("000001.SZ");
        if (bOutputMsg) wcout << "windcodes: " << windcodes << endl;
        LPCWSTR indicators = TEXT("rt_date,rt_time,rt_latest,rt_pre_close,rt_amt");
        LPCWSTR options = TEXT("");
        errcode = CWAPIWrapperCpp::wsq(wd, windcodes, indicators, options);
        if (bOutputMsg) qDebug() << "startWsqOneTime errcode: " << errcode;
        if (errcode == 0) {
            int codelen = wd.GetCodesLength();
            int fieldslen = wd.GetFieldsLength();
            int colnum = fieldslen + 1;
            QString datetime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            if (bOutputMsg)  cout  << "WindCodes    ";
            for (int i =1;i < colnum; ++i)
            {
                QString outfields = QString::fromStdWString(wd.GetFieldsByIndex(i-1));
                if (bOutputMsg) cout << outfields.toStdString() << "    ";
            }
            if (bOutputMsg) cout << endl;
            for (int i = 0; i < codelen; ++i)
            {
                QStringList singleCodeData;
                QString codes = QString::fromStdWString(wd.GetCodeByIndex(i));
                if (bOutputMsg)  cout << codes.toStdString() << "    ";
                for (int j = 0; j < fieldslen; ++j)
                {
                    VARIANT var;
                    wd.GetDataItem(0,i,j,var);
                    QString temp = variantToQString(&var);
                    singleCodeData.append(temp);
                    if (bOutputMsg) cout << temp.toStdString() << "    ";
                }
                singleCodeData.append(datetime);
                if (bOutputMsg) cout << endl;
                result.insert(codes, singleCodeData);
            }
        } else {
//            emit startWsqFailed(errcode, reqID);
        }
        delete[] windcodes;
    } else {
        errcode = -1;
        qDebug() << "Login first!";
    }
//    qDebug() << "result: " << result;
    return result;
}

QMap<QString, QStringList> EmptyQStringQStringListMap() {
    QMap<QString, QStringList> result;
    return result;
}










