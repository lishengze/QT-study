#ifndef TIME_FUNC_H
#define TIME_FUNC_H
#include <QList>
#include <QTableView>
#include <QDate>
#include <QTime>
#include <QFile>

QDateTime transIntDateTime(long long data);

QDateTime transQStringDateTime(QString data, QString separator=" ");

QDate transIntDate(int data);

QTime transIntTime(int data);

qint64 transDateTime(double oridata);

QString transOffsetSecondsToTime(qint64 offSecs);

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

QDate getDate(QString date);

QTime StockAmStartTime();

QTime StockAmStopTime();

QTime StockPmStartTime();

QTime StockPmStopTime();

bool isStockTrading(QTime time = QTime::currentTime());

bool isStockTradingNotStart(QTime time = QTime::currentTime());

bool isStockNoonBreak(QTime time = QTime::currentTime());

bool isStockTradingOver(QTime time = QTime::currentTime());

bool isTradingDay(QDate day);

bool isTradingOver(QTime time = QTime::currentTime());

bool isTradingStart(QTime time = QTime::currentTime());

QDate getNextTradingData(QDate date);

bool waitForNextTradingDay(QTableView* programInfoTableView);

bool isMinuteType(QString dataType);

QString getPreDate(QString oridata, QString timeType, int timeNumb);

int getStartIndex(QString startDate, QList<QString>& timeList);
#endif // TIME_FUNC_H
