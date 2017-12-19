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

QList<MACD> computeMACD(QList<double> oriData, int t1, int t2, int t3);

QList<double> computeMACDDoubleData(QList<double> oriData, int t1, int t2, int t3);

MACD computeMACDData(double newData, MACD oldData, int t1, int t2, int t3);

void ErrorMessage(QString msg);

void WarnMessage(QWidget* window, QString title, QString msg);

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

LPCWSTR transSecode(QStringList secodeList);

QString variantToQString(const LPVARIANT data);

LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd);

void writeWsqData(QString secode, QStringList data);

#endif // TOOLFUNC_H
