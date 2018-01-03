#include <QDebug>
#include "excel.h"
#include "toolfunc.h"

Excel::Excel():
    m_excel(NULL),
    m_work_books(NULL)
{
    QList<QString> pidListBeforeCreate = getEXCELPidList ();
    m_excel = new QAxObject("Excel.Application");  // 花费时间最长;
    QList<QString> pidListAfterCreate = getEXCELPidList ();
    m_excel->setProperty("Visible", false);        // 花费时间次之,其余操作不花时间;
    m_createdPidList = getAddedList(pidListBeforeCreate, pidListAfterCreate);
    m_work_books = m_excel->querySubObject("WorkBooks");
}

Excel::~Excel () {
    if (NULL != m_excel) {
        m_excel->dynamicCall ("Quit(void)");
        delete m_excel;
        m_excel = NULL;
    }

    for (int i = 0; i < m_createdPidList.size (); ++i) {
        killProcessByPid (m_createdPidList[i]);
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

//    activate_work_book->dynamicCall("Close()");  //关闭文件
    m_work_books->dynamicCall ("Close()");
//    m_work_books->dynamicCall ("Close(Boolean)", false);
    return strategy;
}

