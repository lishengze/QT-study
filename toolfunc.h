#ifndef TOOLFUNC_H
#define TOOLFUNC_H
#include <QString>
#include <QList>
#include <QPointF>
#include <QWidget>
#include <QTableView>
#include <QFileInfo>
#include "macd.h"
#include "WAPIWrapperCpp.h"

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

LPCWSTR transSecodeList(QStringList secodeList);

LPCWSTR transSecode(QString qString);

QString completeSecode(QString secode, QString style="tinysoft") ;

QString variantToQString(const LPVARIANT data);

LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd);

//void testSpread(QMap<QString, QStringList> data);

//void writeWsqData(QString secode, QStringList data);

double getAveValue(QList<double>);

QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList);

QMap<QString, QStringList> EmptyQStringQStringListMap();

template<class Type>
void printList(QList<Type> oridata, QString msg) {
    qDebug() << msg ;
    qDebug() << "oridata.size: " << oridata.size();
    for (int i = 0; i < oridata.size(); ++i) {
        qDebug() << oridata[i];
    }
}

template<class keyType, class valueType>
void printMap(QMap<keyType, valueType> data, QString message) {
    qDebug() << "message: " << message;
    for ( QMap<keyType, valueType>::iterator it = data.begin();
          it != data.end(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }
}

template<class T>
QList<T> getSubList(QList<T> oriData, int startIndex, int endIndex) {
    QList<T> result;
    if (startIndex < 0) return result;

    if (endIndex > oriData.size()) {
        endIndex = oriData.size();
    }

    for (int i = startIndex; i < endIndex; ++i) {
        result.append(oriData[i]);
    }
    return result;
}

#endif // TOOLFUNC_H
