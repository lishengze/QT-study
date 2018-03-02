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
#include "macd.h"
//#include "WAPIWrapperCpp.h"

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

QString getCompleteSecode(QString secode, QString style);

QString getCompleteIndexCode(QString secode, QString style);

QMap<QString, int> transStrategyMap(QMap<QString, int> oriStrategyMap, QString codeStyle);

//void testSpread(QMap<QString, QStringList> data);

void writeWsqData(QString secode, QStringList data);

double getAveValue(QList<double>);

bool isTradingTime(QTime time = QTime::currentTime());

bool isTradingOver(QTime time = QTime::currentTime());

bool isTradingStart(QTime time = QTime::currentTime());

void testCLosePrice(QMap<QString, QList<QStringList>> oriData);

qint64 transDateTime(double oridata);

QList<QString> getExcelFileName(QString dirName);

QList<QFileInfo> getExcelFileInfo(QString dirName);

QList<QFileInfo> getDirName(QString dirName);

QStandardItemModel* getStandardItemModel(QString title, QList<QString> valueList);

QStandardItemModel* getStandardItemModel(QString title, QList<QFileInfo> valueList);

QList<QPointF> getStrategyPointList(QMap<QString, QList<QStringList>> oriData,
                                    QMap<QString, int> seocdebuyCountMap);

QList<double> getHedgedData(QList<QPointF> pointDataList, QMap<QString, QStringList> indexHedgeData,
                            int indexBuyCount, int indexBaseCount);

QList<QPointF> getHedgedData(QList<QPointF> buyPointDataList, QList<QPointF> salePointDataList);

QList<double> getHedgedData(QMap<QString, QStringList> oneTimeData, QMap<QString, int> seocdebuyCountMap,
                            QString indexCode, int indexBuyCount, int indexBaseCount);

QList<double> getHedgedData(QMap<QString, QStringList> oneTimeData,
                            QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy);

double getHedgedSpread(QMap<QString, QStringList> preCloseData, QMap<QString, int> seocdebuyCountMap,
                                QString indexCode, int indexBuyCount, int indexBaseCount);

double getHedgedSpread(QMap<QString, QStringList> preCloseData,
                         QMap<QString, int> buyStrategy, QMap<QString, int> saleStrategy);


QMap<QString, int> EmpytQStringIntMap();

QStringList EmpytStringList();

bool isSecodeValid(QString secode);

void writePortfolio(QMap<QString, int> portfolio, QString fileName);

template<class keyType, class valueType>
void printMap(QMap<keyType, valueType> data, QString message) {
    qDebug() << "message: " << message;
    for ( QMap<keyType, valueType>::iterator it = data.begin();
          it != data.end(); ++it) {
        qDebug() << it.key() << ": " << it.value();
    }
}

//LPCWSTR transSecode(QStringList secodeList);

//LPCWSTR transSecodeB(QStringList secodeList);

//LPCWSTR transSecode(QString qString);

//QString variantToQString(const LPVARIANT data);

//LONG WINAPI wsqCallBack( ULONGLONG reqid, const WindData &wd);

//QMap<QString, QStringList> wsqSnaphootData(QStringList secodeList);

#endif // TOOLFUNC_H
