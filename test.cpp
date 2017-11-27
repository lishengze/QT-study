#include <QList>
#include <QPointF>
#include <QDebug>
#include <iostream>
#include "test.h"
#include "setdata.h"
#include "toolfunc.h"
#include "strategy.h"
#include "tabledata.h"
#include "excel.h"

using namespace  std;

void testReadExcelData () {
    QString excelFilePath = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
//    readBasicDataFromExcel(excelFilePath);
    readStrategyDataFromExcel(excelFilePath);
}

void testGetExcelFileName() {
    QString dirName = "E:/github/study/QT/Creator/chapter4/layout";
//    dirName = "E:/DataBase/original-data-20160910-20170910-1m";
    QList<QString> fileNameList = getExcelFileName(dirName);
    qDebug() << fileNameList;
}

void testRemovePathName () {
    QString fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
    QString pureFileName = removePathName (fullFileName);
    qDebug() << "FullFileName: " << fullFileName;
    qDebug() << "PureFileName: " << pureFileName;
}

void testReadStrategyDataFromExcel() {
    QString fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
    QList<strategy_ceil> m_currStrategy = readStrategyDataFromExcel (fullFileName);
    for (int i = 0; i < m_currStrategy.size(); ++i) {
        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
    }
}

void testExcel() {
    Excel excelObj;
    QString fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test.xlsx";
    QList<strategy_ceil> m_currStrategy = excelObj.readStrategyDataFromExcel (fullFileName);
    for (int i = 0; i < m_currStrategy.size(); ++i) {
        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
    }

    fullFileName = "E:/github/study/QT/Creator/chapter4/layout/test1.xlsx";
    m_currStrategy = excelObj.readStrategyDataFromExcel (fullFileName);
    for (int i = 0; i < m_currStrategy.size(); ++i) {
        qDebug() << "secode: " << m_currStrategy.at(i).m_secode
                 << " buyCount: " << m_currStrategy.at(i).m_buyCount;
    }
}

void testSortPointList() {
    QList<QPointF> oriData;
    int dataNumb = 5;
    for (int i = 0; i < dataNumb; ++i) {
        oriData.append (QPointF(dataNumb-i, i));
    }
    qDebug() << "oridata: " << oriData;

    QList<QPointF>sortedData = sortPointFList(oriData);
    qDebug() << "sortedData: " << sortedData;
}

void testMergeSortedList() {
    QList<QPointF> firstList;
    QList<QPointF> secondList;
    int numb = 10;
    int index = -15;
    for (int i = 0; i<numb; ++i) {
        firstList.append (QPointF(i, i));
        secondList.append (QPointF(i + index, i));
    }
    qDebug() << "firstList: " << firstList;
    qDebug() << "secondList: " << secondList;

    QList<QPointF> mergedList = mergeSortedPointedList (firstList, 1, secondList, 1);
    qDebug() << "mergedList: " << mergedList;
}

void testTableData () {
    TableData tmpdata(20170111, 110911, 13.0);
    cout << tmpdata;
}
