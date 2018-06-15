#ifndef TIME_FUNC_H
#define TIME_FUNC_H
#include <QList>
#include <QTableView>
#include <QDate>
#include <QTime>
#include <QFile>

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

QDate getDate(QString date);

QTime StockAmStartTime();

QTime StockAmStopTime();

QTime StockPmStartTime();

QTime StockPmStopTime();

bool isStockTrading();

bool isStockTradingNotStart();

bool isStockNoonBreak();

bool isStockTradingOver();

bool isTradingDay(QDate day);

bool waitForNextTradingDay(QTableView* programInfoTableView);

#endif // TIME_FUNC_H
