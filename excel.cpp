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

    qDebug() << "start read: " << excelFilePath;
    QList<strategy_ceil> strategy;
    QMap<QString, int> strategy_list;

    if(sheet_count > 0) {
        QAxObject *work_sheet = activate_work_book->querySubObject("Sheets(int)", 1);
        QAxObject *used_range = work_sheet->querySubObject("UsedRange");

        // 一次读取整张活动表的数据;
        QVariant sheetData = used_range->dynamicCall ("Value2()");
        QVariantList sheetDataList = sheetData.toList ();
        for (int i = 0; i< sheetDataList.count (); ++i) {
            QVariantList varLstData = sheetDataList[i].toList();
            QString secode = varLstData[0].toString ();          //获取股票代码
            int buyCount = varLstData[1].toInt ();               //获取购买量

            QList<QString> keyValueList = strategy_list.keys();
            if (keyValueList.indexOf(secode) != -1) {
                strategy_list[secode] += buyCount;
            } else {
                strategy_list.insert(secode, buyCount);
            }

        }
    }

    QList<QString> keyValueList = strategy_list.keys();
    for (int i = 0; i < keyValueList.size(); ++i) {
        QString secode = keyValueList[i];
        strategy.append (strategy_ceil(secode, strategy_list[secode]));
    }


//    activate_work_book->dynamicCall("Close()");  //关闭文件
    m_work_books->dynamicCall ("Close()");
//    m_work_books->dynamicCall ("Close(Boolean)", false);
    return strategy;
}

QMap<QString, int> Excel::readStrategyData(QString excelFilePath, int sheetIndex) {
    m_work_books->dynamicCall("Open (const QString&)", QString(excelFilePath));
    QAxObject *activate_work_book = m_excel->querySubObject("ActiveWorkBook");
    QAxObject *work_sheets = activate_work_book->querySubObject("Sheets");  //Sheets也可换用WorkSheets
    int sheet_count = work_sheets->property("Count").toInt();  //获取工作表数目

    qDebug() << "start read: " << excelFilePath << ", sheetindex: " << sheetIndex;
    QList<strategy_ceil> strategy;
    QMap<QString, int> strategy_list;

    if(sheet_count > 0) {
        QAxObject *work_sheet = activate_work_book->querySubObject("Sheets(int)", sheetIndex);
        QAxObject *used_range = work_sheet->querySubObject("UsedRange");

        // 一次读取整张活动表的数据;
        QVariant sheetData = used_range->dynamicCall ("Value2()");
        QVariantList sheetDataList = sheetData.toList ();
        for (int i = 0; i< sheetDataList.count (); ++i) {
            QVariantList varLstData = sheetDataList[i].toList();
            QString secode = varLstData[0].toString ();          //获取股票代码
            int buyCount = varLstData[1].toInt ();               //获取购买量

            QList<QString> keyValueList = strategy_list.keys();
            if (keyValueList.indexOf(secode) != -1) {
                strategy_list[secode] += buyCount;
            } else {
                strategy_list.insert(secode, buyCount);
            }

        }
    }
    return strategy_list;
}

QList<QMap<QString, int>> Excel::getBuySaleStrategy(QString excelFilePath) {
    QMap<QString, int> buyStrategy = readStrategyData(excelFilePath, 1);
    QMap<QString, int> saleStrategy = readStrategyData(excelFilePath, 1);
    QList<QMap<QString, int>> result;
    result.append(buyStrategy);
    result.append((saleStrategy));
    return result;
}


