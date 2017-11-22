#include "test.h"
#include "setdata.h"
#include <QDebug>

void testReadExcelData () {
    QString excelFilePath = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
//    readBasicDataFromExcel(excelFilePath);
    readStrategyDataFromExcel(excelFilePath);
}

void testGetExcelFileName() {
    QString dirName = "E:/github/study/QT/Creator/chapter4/layout";
    dirName = "E:/DataBase/original-data-20160910-20170910-1m";
    getExcelFileName(dirName);
}
