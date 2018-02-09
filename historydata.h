#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include <QTableView>
#include <QMutexLocker>
#include <QMutex>

#include "dataread.h"
#include "dataprocess.h"
#include "dataread.h"
#include "toolfunc.h"

class HistoryData : public QObject
{
    Q_OBJECT
public:
    HistoryData(int chartViewID, QString dbhost, QString databaseName,
                bool isRealTime, int threadNumb, QList<QString> secodeNameList,
                QList<int> macdTime, QString hedgeIndexCode,
                QString startDate="", QString endDate="", QStringList keyValueList=EmpytStringList(),
                QMap<QString, int> seocdebuyCountMap = EmpytQStringIntMap(),
                QMap<QString, int> buyStrategyMap = EmpytQStringIntMap(),
                QMap<QString, int> saleStrategyMap = EmpytQStringIntMap(),
                QObject *parent = 0);

    ~HistoryData();

    void initCommonData();
    void initSignalType();

    void initThreadSecodeList();
    void initReadDataSignal();
    void initProcessDataSignal();

    void startProcessData ();

    void releaseDataReaderSrc ();
    void releaseDataProcessSrc ();

signals:
    void receiveHistDataSignal(QList<QList<double>> allData);
    void startReadDataSignal(QString dataType);
    void startProcessDataSignal(QString dataType);
    void tableViewInfoSignal(QString msg);

public slots:
    void getHistData();
    void receiveOriginalData(QMap<QString, QList<QStringList>> subThreadData);
    void receiveProcessedData(QList<QList<double>> allData);

private:
    bool m_isRealTime;
    int m_threadNumb;
    QString m_signalType;
    mutable QMutex m_mutex;

    int m_chartViewID;
    QString m_dbhost;
    QString m_databaseName;
    QString m_hedgeIndexCode;
    QList<int> m_macdTime;
    QString m_startDate;
    QString m_endDate;
    QStringList m_keyValueList;

    QMap<QString, int> m_buyStrategyMap;
    QMap<QString, int> m_saleStrategyMap;
    QMap<QString, int> m_seocdebuyCountMap;
    QList<QString> m_secodeNameList;

    QList<DataProcess*> m_dataProcessList;
    QList<QThread*> m_dataProcessThreadList;

    QList<QStringList> m_threadSecodeList;
    QList<DataRead*> m_dataReaderList;
    QList<QThread*> m_dataReaderThreadList;
    int m_readDataThreadCount;
    QMap<QString, QList<QStringList>> m_completeTableData;
};

#endif // HISTORYDATA_H
