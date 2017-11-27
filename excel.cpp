#include <QDebug>
#include "excel.h"

Excel::Excel():
    m_excel(NULL)
{
    m_excel = new QAxObject("Excel.Application");  // 花费时间最长;
    m_excel->setProperty("Visible", false);  // 花费时间次之,其余操作不花时间;
    m_work_books = m_excel->querySubObject("WorkBooks");
}

Excel::~Excel () {
    if (NULL != m_excel) {
        delete m_excel;
        m_excel = NULL;
    }
}

QList<strategy_ceil> Excel::readStrategyDataFromExcel(QString excelFilePath) {
    m_work_books->dynamicCall("Open (const QString&)", QString(excelFilePath));

    QAxObject *activate_work_book = m_excel->querySubObject("ActiveWorkBook");
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
    m_work_books->dynamicCall ("Close()");
    return strategy;
}

