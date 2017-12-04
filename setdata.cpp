#include <iostream>
#include <iomanip>
#include <QSqlDatabase>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include "windows.h"
#include <qDebug>
#include <QList>
#include <QPointF>
#include <QLineSeries>
#include <QDateTime>
#include "setdata.h"
#include "toolfunc.h"
using namespace std;

QSqlDatabase createODBCConn() {
    QSqlDatabase db = QSqlDatabase::addDatabase ("QODBC");
    QString databaseName = QString::fromLocal8Bit ("SqlServer");
    db.setHostName("localhost");
    db.setDatabaseName (databaseName);
    db.setUserName ("sa");
    db.setPassword ("sasa");

    if (!db.open ()) {
        qDebug() << "Open database: " + databaseName  + " failed!";
        qDebug() << db.lastError ();
    } else {
        qDebug() << "Open database: " + databaseName  + " successfully!";
    }
    return db;
}

QList<QPointF> getOpenPrice() {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     int x = 0;
     QList<QPointF> pointList;
     while(queryObj.next ()) {
        pointList.append (QPointF(x++, queryObj.value (0).toFloat ()));
//        if (x < 10) qDebug() << queryObj.value (0).toFloat ();
     }
    db.close ();
    return pointList;
}

QList<QPointF> getTimeData() {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TDATE], [TIME], [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     QList<QPointF> pointList;
     while(queryObj.next ()) {
        QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
        QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
        QDateTime xValue;
        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
        float yValue = queryObj.value (2).toFloat ();
        pointList.append (QPointF(xValue.toMSecsSinceEpoch (), yValue));
     }
     db.close ();
    return pointList;
}

QList<QPointF> getTimeData(QString startDate, QString endDate, QString keyValue, QString tableName, QString databaseName){
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = QString("select [TDATE], [TIME], [") + keyValue + QString("] ") \
                   + QString("from [") + databaseName + "].[dbo].[" + tableName + "] " \
                   + "where TDATE <= " + endDate + " and TDATE >= " + startDate;
    qDebug() <<sqlstr;
    queryObj.exec(sqlstr);

     QList<QPointF> pointList;
     while(queryObj.next ()) {
        QList<int> tmpDate = getDateList(queryObj.value (0).toInt ());
        QList<int> tmpTime = getTimeList (queryObj.value (1).toInt ());
        QDateTime xValue;
        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
        float yValue = queryObj.value (2).toFloat ();
        pointList.append (QPointF(xValue.toMSecsSinceEpoch (), yValue));
     }
     db.close ();
    return pointList;
}

void testSetTimeData(QLineSeries* series) {
    int date[3] = {20170101, 20170202, 20170303};
    int time[3] = {93000, 103000, 113000};
    double yValue[3] = {12.1, 12.2, 12.3};

    for (int i = 0; i < 3; ++i) {
        QList<int> tmpDate = getDateList(date[i]);
        QList<int> tmpTime = getTimeList (time[i]);
        QDateTime xValue;
        xValue.setDate (QDate(tmpDate[0], tmpDate[1], tmpDate[2]));
        xValue.setTime (QTime(tmpTime[0], tmpTime[1], tmpTime[2]));
        series->append(xValue.toMSecsSinceEpoch(), yValue[i]);
    }
}

void setSeriresData(QLineSeries* series) {
    QSqlDatabase db = createODBCConn();
    QSqlQuery queryObj(db);
    QString sqlstr = "select [TOPEN] from [MarketData].[dbo].[SH600000]";
    queryObj.exec(sqlstr);

     int x = 0;
     QList<QPointF> pointList;
     while(queryObj.next ()) {
        QPointF point = QPointF(x++, queryObj.value (0).toFloat ());
        pointList.append (point);
        series->append(x, queryObj.value (0).toFloat ());
        if (x < 10) qDebug() << queryObj.value (0).toFloat ();
     }
     int numRows = queryObj.at ();
     qDebug() << "numRows: " << numRows;
     qDebug() << "ListCount: " << pointList.count ();
}

void readBasicDataFromExcel(QString excelFilePath) {
    QAxObject excel("Excel.Application");
    excel.setProperty("Visible", true);
    QAxObject *work_books = excel.querySubObject("WorkBooks");
    work_books->dynamicCall("Open (const QString&)", QString(excelFilePath));

    QVariant title_value = excel.property("Caption");  //获取标题
    qDebug()<<QString("excel title : ")<<title_value;

    QAxObject *work_book = excel.querySubObject("ActiveWorkBook");
    QAxObject *work_sheets = work_book->querySubObject("Sheets");  //Sheets也可换用WorkSheets

    int sheet_count = work_sheets->property("Count").toInt();  //获取工作表数目
    qDebug()<<QString("sheet count : ")<<sheet_count;

    for(int i=1; i<=sheet_count; i++)
    {
        QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", i);  //Sheets(int)也可换用Worksheets(int)
        QString work_sheet_name = work_sheet->property("Name").toString();  //获取工作表名称
        QString message = QString("sheet ")+QString::number(i, 10)+ QString(" name");
        qDebug()<<message<<work_sheet_name;
    }

    if(sheet_count > 0)
    {
        QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", 1);
        QAxObject *used_range = work_sheet->querySubObject("UsedRange");
        QAxObject *rows = used_range->querySubObject("Rows");
        QAxObject *columns = used_range->querySubObject("Columns");
        int row_start = used_range->property("Row").toInt();  //获取起始行
        int column_start = used_range->property("Column").toInt();  //获取起始列
        int row_count = rows->property("Count").toInt();  //获取行数
        int column_count = columns->property("Count").toInt();  //获取列数
        qDebug() << "row_count: " << row_count;
        qDebug() << "column_count: " << column_count;
        qDebug() << "row_start: " << row_start;
        qDebug() << "column_start: " << column_start;

//        QVariant cell_value = used_range->dynamicCall("Value");
//        qDebug() << cell_value;

        for(int i = 0; i < row_count; ++i)
        {
            for(int j = 0; j < column_count; ++j)
            {
                QAxObject *cell = work_sheet->querySubObject("Cells(int,int)", i+row_start, j+column_start);
                QVariant cell_value = cell->dynamicCall("Value2()");  //获取单元格内容
//                QVariant cell_value = used_range->dynamicCall("Value");
                QString message = QString("row-")+QString::number(i, 10)+QString("-column-")+QString::number(j, 10)+QString(":");
//                qDebug()<<message<<cell_value;
                qDebug() << cell_value;
            }
        }
    }
}

QList<strategy_ceil> readStrategyDataFromExcel(QString excelFilePath) {
    qDebug() << "open file " << excelFilePath;
    QAxObject excel("Excel.Application");  // 花费时间最长;
    excel.setProperty("Visible", false);  // 花费时间次之,其余操作不花时间;
    QAxObject *work_books = excel.querySubObject("WorkBooks");
    work_books->dynamicCall("Open (const QString&)", QString(excelFilePath));

    QAxObject *activate_work_book = excel.querySubObject("ActiveWorkBook");
    QAxObject *work_sheets = activate_work_book->querySubObject("Sheets");  //Sheets也可换用WorkSheets
    int sheet_count = work_sheets->property("Count").toInt();  //获取工作表数目

    qDebug() << "start read data ....";
    QList<strategy_ceil> strategy;

    if(sheet_count > 0)
    {
        QAxObject *work_sheet = activate_work_book->querySubObject("Sheets(int)", 1);
        QAxObject *used_range = work_sheet->querySubObject("UsedRange");

        // 一次读取整张活动表的数据;
        QVariant sheetData = used_range->dynamicCall ("Value2()");
        QVariantList sheetDataList = sheetData.toList ();
        for (int i = 0; i< sheetDataList.count (); ++i) {
            QVariantList varLstData = sheetDataList[i].toList();
            QString secode = varLstData[0].toString ();          //获取股票代码
            int buyCount = varLstData[1].toInt ();               //获取购买量
            strategy.append (strategy_ceil(secode, buyCount));
        }
    }
    work_books->dynamicCall ("Close()");
    return strategy;
}

QList<strategy_ceil> readStrategyDataFromExcelOld(QString excelFilePath) {
    qDebug() << "open file " << excelFilePath;
    QAxObject excel("Excel.Application");
    excel.setProperty("Visible", false);
    QAxObject *work_books = excel.querySubObject("WorkBooks");
    work_books->dynamicCall("Open (const QString&)", QString(excelFilePath));

    QAxObject *work_book = excel.querySubObject("ActiveWorkBook");
//    QVariant title_value = excel.property("Caption");  //获取标题
//    qDebug()<<QString("excel title : ")<<title_value;

    QAxObject *work_sheets = work_book->querySubObject("Sheets");  //Sheets也可换用WorkSheets
    int sheet_count = work_sheets->property("Count").toInt();  //获取工作表数目
//    qDebug()<<QString("sheet count : ")<<sheet_count;
//    for(int i=1; i<=sheet_count; i++)
//    {
//        QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", i);  //Sheets(int)也可换用Worksheets(int)
//        QString work_sheet_name = work_sheet->property("Name").toString();  //获取工作表名称
//        QString message = QString("sheet ")+QString::number(i, 10)+ QString(" name");
////        qDebug()<<message<<work_sheet_name;
//    }

    qDebug() << "start read data ....";
    QList<strategy_ceil> strategy;
    if(sheet_count > 0)
    {
        QAxObject *work_sheet = work_book->querySubObject("Sheets(int)", 1);
        QAxObject *used_range = work_sheet->querySubObject("UsedRange");        

//        QAxObject *rows = used_range->querySubObject("Rows");
//        QAxObject *columns = used_range->querySubObject("Columns");
//        int row_start = used_range->property("Row").toInt();  //获取起始行
//        int column_start = used_range->property("Column").toInt();  //获取起始列
//        int row_count = rows->property("Count").toInt();  //获取行数
//        int column_count = columns->property("Count").toInt();  //获取列数
//        qDebug() << "row_count: " << row_count;
//        qDebug() << "column_count: " << column_count;
//        qDebug() << "row_start: " << row_start;
//        qDebug() << "column_start: " << column_start;
//        QAxObject *cell;
//        for(int i = 0; i < row_count; ++i)
//        {
//            cell = work_sheet->querySubObject("Cells(int,int)", i+row_start, 0 + column_start);
//            QString secode = cell->dynamicCall("Value2()").toString ();  //获取股票代码
//            cell = work_sheet->querySubObject("Cells(int,int)", i+row_start, 1 + column_start);
//            int buyCount = cell->dynamicCall("Value2()").toInt ();  //获取购买量
//            strategy.append (strategy_ceil(secode, buyCount));
//        }

        // 一次读取整张活动表的数据;
        QVariant sheetData = used_range->dynamicCall ("Value2()");
        QVariantList sheetDataList = sheetData.toList ();
        for (int i = 0; i< sheetDataList.count (); ++i) {
            QVariantList varLstData = sheetDataList[i].toList();
            QString secode = varLstData[0].toString ();          //获取股票代码
            int buyCount = varLstData[1].toInt ();               //获取购买量
            strategy.append (strategy_ceil(secode, buyCount));
        }
    }
    work_books->dynamicCall ("Close()");
    return strategy;
}

QList<QString> getExcelFileName(QString dirName) {
    QDir *dir=new QDir(dirName);
    QStringList filter;
    filter<<"*.xlsx";
    dir->setNameFilters(filter);
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    QList<QString> filaPathList;

//    qDebug() << "fileCount: " <<fileInfo->count ();
    for (int i = 0; i < fileInfo->count (); ++i) {
        filaPathList.append (fileInfo->at(i).filePath());
    }
    return filaPathList;
}
