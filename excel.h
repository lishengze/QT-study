#ifndef EXCEL_H
#define EXCEL_H
#include <QAxObject>
#include <QList>
#include <QProcess>
#include <QList>
#include <QString>
#include "strategy.h"

class Excel
{
public:
    Excel();
    ~Excel();
    QList<strategy_ceil> readStrategyDataFromExcel(QString excelFilePath);
    QList<QString> getSecodeFromeExcel(QString excelFilePath, int sheetIndex = 1);

private:
    QAxObject* m_excel;
    QAxObject* m_work_books;
    QList<QString> m_createdPidList;
};

#endif // EXCEL_H
