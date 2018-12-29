#ifndef EXCEL_FUNC_H
#define EXCEL_FUNC_H

#include <QString>
#include <QList>
#include <QFileInfo>

QList<QFileInfo> getExcelFileInfo(QString dirName);

QList<QString> getExcelFileName(QString dirName);

QList<QString> readExcelSecodeList(QString fileName, QString sheetName="Sheet1",
                                   int startRow=1, QString style = "ori");

QList<QStringList> readExcelData(QString fileName,  QString sheetName="Sheet1");

QMap<QString, int> readExcelMapInt(QString fileName, QString sheetName="Sheet1");

QMap<QString, double> readExcelMapDouble(QString fileName, QString sheetName="Sheet1");

void checkFile(QString fileName);

int writeFileInfo(QString fileName, QString colName, QString value, QString sheetName="Sheet1");

int writeMatrixData(QString fileName, QList<QStringList>& oriData,
                    QString sheetName="Sheet1", bool isTranspose=false, bool isNewFile=true);

int writeOneColData(QString fileName, QStringList oriData, int targetCol,
                    int startRow = 0, QString sheetName="Sheet1");

int writeOneRowData(QString fileName, QStringList oriData, int targetRow,
                    int startCol = 0, QString sheetName="Sheet1");

int completeExcelData(QList<QStringList>& ori_result, QList<QStringList>& index_time_list, int keyCount, QString secode="");

QStringList getIndexTimeStrList(QList<QStringList> indexTimeList);

QList<QStringList> getIndexTimeList(QString startDate, QString endDate,
                                    QString databaseName, QString dbhost="localhost");

QList<QString> getSingleColData(QList<QStringList> ori_result, int targetCol);

void initExcelFile(QString fileName);

int writePortfolio(QMap<QString, double> portfolio, QString fileName,
                    QString accountName, QString type, QString sheetName="Sheet1");

QString readlFileData(QString fileName, int row, int col, QString sheetName = "Sheet1");

double getPortfolioAmount(QMap<QString, double> priceMap, QMap<QString, double> porfolioMap);

QMap<QString, QString> getPortfolioAmountMap(QList<QFileInfo> fileInfoList);


void TestExcelMain();

#endif // EXCEL_FUNC_H
