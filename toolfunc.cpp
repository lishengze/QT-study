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
#include "secode_func.h"

using std::sort;
using std::max;
using std::min;


/*
 * 控件处理;
//void updateProgramInfo(QTableView* programInfoTableView, QString message, QString remark) {
//    QStandardItemModel* testMode = dynamic_cast<QStandardItemModel*>(programInfoTableView->model ());
//    if (NULL != testMode) {
//        QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
//        int row = testMode->rowCount ();
//        testMode->setItem (row, 0,  new QStandardItem(datetime));
//        testMode->setItem (row, 1,  new QStandardItem(message));
//        testMode->setItem (row, 2,  new QStandardItem(remark));
//        QScrollBar* verScrollBar = programInfoTableView->verticalScrollBar();
//        verScrollBar->setSliderPosition(verScrollBar->maximum());
//    }
//}
*/
/*
控件处理函数

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

*/

/*
 * Excel 处理函数
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

*/

/*
 *时间处理函数
 *
//// 20170911 -> [2017, 9, 11]
//QList<int> getDateList(int intDate) {
//    QList<int> dateList;
//    int year = intDate / 10000;
//    int month = (intDate - year * 10000) / 100;
//    int day = intDate - year * 10000 - month * 100;
//    dateList.append (year);
//    dateList.append (month);
//    dateList.append (day);
//    return dateList;
//}

//// 112002 -> [11, 20 ,2]
//QList<int> getTimeList(int intDate) {
//    QList<int> timeList;
//    int hour = intDate / 10000;
//    int minute = (intDate - hour * 10000) / 100;
//    int second = intDate - hour * 10000 - minute * 100;
//    timeList.append (hour);
//    timeList.append (minute);
//    timeList.append (second);
//    return timeList;
//}

//QTime StockAmStartTime() {
//    return QTime(9, 30, 10);
//}

//QTime StockAmStopTime() {
//    return QTime(11, 30, 5);
//}

//QTime StockPmStartTime() {
//    return QTime(13, 0, 10);

//}

//QTime StockPmStopTime() {
//    return QTime(15, 0, 5);
//}

//bool isStockTrading(QTime currTime ) {
//    if ((currTime >= StockAmStartTime() && currTime <= StockAmStopTime()) ||
//         (currTime >= StockPmStartTime() && currTime <= StockPmStopTime())) {
//        return true;
//    } else {
//        return false;
//    }
//}

//bool isStockTradingStart(QTime currTime ) {
//    return currTime >= StockAmStartTime();
//}

//bool isStockTradingNotStart(QTime currTime ) {
//    return currTime < StockAmStartTime();
//}

//bool isStockNoonBreak(QTime currTime ) {
//  return currTime > StockAmStopTime() && currTime <StockPmStartTime();
//}

//bool isStockTradingOver(QTime currTime ) {
//    return currTime > StockPmStopTime();
//}

//bool isTradingTime(QTime time) {
//    QTime amStartTime = QTime(9,30,30);
//    QTime amStopTime = QTime(11, 30, 0);
//    QTime pmStopTime = QTime(15, 0, 0);
//    QTime pmStartTime = QTime(13, 0, 0);

//    if ((time >= amStartTime && time < amStopTime)||
//         (time >= pmStartTime && time < pmStopTime)) {
//        return true;
//    } else {
//        return false;
//    }
//}

//bool isTradingOver(QTime time) {
//    QTime pmStopTime = QTime(15, 0, 20);
//    if (time >= pmStopTime){
//        return true;
//    } else {
//        return false;
//    }
//}

//bool isTradingStart(QTime time) {
//    QTime amStartTime = QTime(9,30,6);
//    if (amStartTime > time) {
//        return false;
//    } else {
//        return true;
//    }
//}

*/

/*
 * Wind 相关处理函数
 *
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

*/





