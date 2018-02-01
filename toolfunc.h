#ifndef TOOLFUNC_H
#define TOOLFUNC_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QWidget>
#include <QTableView>
#include "macd.h"
#include "WAPIWrapperCpp.h"

QList<int> getDateList(int intDate);

QList<int> getTimeList(int intDate);

QString removePathName(QString fullFileName);

QList<QPointF> sortPointFList(QList<QPointF> oridata);

QList<QPointF> mergeSortedPointedList(QList<QPointF> firstList, int firstBuyCount,
                                      QList<QPointF> secondList, int secondBuyCount);

//QList<MACD> computeMACD(QList<double> oriData, int t1, int t2, int t3);

QList<double> computeMACDDoubleData(QList<double> oriData, int t1, int t2, int t3);

//MACD computeMACDData(double newData, MACD oldData, int t1, int t2, int t3);

void ErrorMessage(QString msg);

void WarnMessage(QWidget* window, QString title, QString msg);

QList<double> getChartYvalueRange(QList<QPointF> pointList );

QList<double> getChartYvalueRange(QList<double> yValueList );

QList<double> getMACDRange(QList<MACD> oriData);

QString transOffsetSecondsToTime(qint64 offSecs);

QList<int> getNumbList(int dataNumb, int desNumb);

void OutputMsg(QWidget* window,QString msg);

QList<QString> getAddedList(QList<QString> oriList, QList<QString> afterAddList);

QList<QString> getEXCELPidList();

QList<QString> getProcessPid(QString taskList, QString processName);

void killProcessByPid(QString pid);

QString getIndexCode();

void updateProgramInfo(QTableView* programInfoTableView, QString message, QString remark="");

QString getWindSecode(QString secode);

QList<QString> getIndexCode(QString style="wind");

LPCWSTR transSecode(QStringList secodeList);

LPCWSTR transSecodeB(QStringList secodeList);

LPCWSTR transSecode(QString qString);

QString completeSecode(QString secode, QString style="tinysoft") ;

QString variantToQString(const LPVARIANT data);

LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd);

//void testSpread(QMap<QString, QStringList> data);

//void writeWsqData(QString secode, QStringList data);

double getAveValue(QList<double>);

bool isTradingTime(QTime time);

bool isTradingStart();

bool isTradingOver(QTime time);

QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList);

QList<QString> getExcelFileName(QString dirName);

#endif // TOOLFUNC_H
