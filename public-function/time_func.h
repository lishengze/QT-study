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

qint64 transToEpochTime(double oridata);

QString transOffsetSecondsToTime(qint64 offSecs);

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

QDate getDate(QString date);

QTime getTime(QString time);

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

void waitToTime(QTime waitedTime);

bool isMinuteType(QString dataType);

QString getPreDate(QString oridata, QString timeType, int timeNumb);

int getStartIndex(QString startDate, QList<QString>& timeList);

int getStartIndex(QString startDate, QList<double>& timeList);

QList<QList<int>> getShortedStartEndDateList(int oriStartDate, int oriEndDate,
                                             QString databaseName, int dataNumbOnce = 2000);
/*
time: TDATE , TIME;
is timea late than timeb
*/
bool isTimeLate(QStringList timea, QStringList timeb);

/*
time: TDATE , TIME;
is timea Earily than timeb
*/
bool isTimeEarly(QStringList timea, QStringList timeb);

/*
获取不同数据频率下，20个交易日中的交易数据数目;
*/
int getTimeStartPos(QString databaseName, int tradingDays=20);

int getDelayStartDate(QList<QStringList>& oriDBData, int delayDays = 30);

#endif // TIME_FUNC_H
