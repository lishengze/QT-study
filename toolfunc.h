#ifndef TOOLFUNC_H
#define TOOLFUNC_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QWidget>
#include <QTableView>
#include <QTime>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndexList>
#include "macd.h"


/*
 * 控件处理函数
void updateProgramInfo(QTableView* programInfoTableView, QString message, QString remark="");
*/

/*
 * 控件处理函数
QMap <int, int>  getSelectRows(QTableView* tableView);

QList<QString> getSelectTableData(QTableView* tableView, QMap<int, int> selectInfo);
*/

/*
Excel 处理函数

QList<QString> getExcelFileName(QString dirName);

QList<QFileInfo> getExcelFileInfo(QString dirName);

int writeFileInfo(QString fileName, QString colName,
                   QString value, QString sheetName = "Sheet1");

QList<QStringList> readExcelData(QString fileName, QString sheetName="Sheet1");

*/

/*
时间处理函数

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

bool isTradingTime(QTime time = QTime::currentTime());

bool isTradingOver(QTime time = QTime::currentTime());

bool isTradingStart(QTime time = QTime::currentTime());

QTime StockAmStartTime();

QTime StockAmStopTime();

QTime StockPmStartTime();

QTime StockPmStopTime();

bool isStockTrading(QTime currTime = QTime::currentTime());

bool isStockTradingStart(QTime currTime = QTime::currentTime());

bool isStockTradingNotStart(QTime currTime = QTime::currentTime());

bool isStockNoonBreak(QTime currTime = QTime::currentTime());

bool isStockTradingOver(QTime currTime = QTime::currentTime());

*/
#endif // TOOLFUNC_H
