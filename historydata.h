#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include <QTableView>
#include <QMutexLocker>
#include <QMutex>

#include "database.h"
#include "dataread.h"
#include "dataprocess.h"
#include "dataread.h"
#include "toolfunc.h"
#include "process_data_func.h"

class HistoryData : public QObject
{
    Q_OBJECT
public:
    HistoryData(int chartViewID, bool isBuySalePortfolio,
                QString dbhost, QString databaseName,
                bool isRealTime, int threadNumb, QList<QString> secodeNameList,
                QList<int> macdTime, QString hedgeIndexCode,
                QString startDate="", QString endDate="", QStringList keyValueList=EmpytStringList(),
                QMap<QString, int> seocdebuyCountMap = EmpytQStringIntMap(),
                QMap<QString, int> buyStrategyMap = EmpytQStringIntMap(),
                QMap<QString, int> saleStrategyMap = EmpytQStringIntMap(),
                QObject *parent = 0);

    HistoryData(int chartViewID, QString dbhost, QString databaseName,
                QString selectIndex, QString hedgedIndex,
                QString startDate, QString endDate, bool isRealTime,
                int aveNumb, double css12Rate,
                double cssRate1, double cssRate2,
                double m_maxCSS, double m_minCSS,
                QObject *parent = 0);

    ~HistoryData();

    void initCommonData();
    void initSignalType();
    void initDatabase(int chartViewID);

    void initThreadSecodeList();
    void initReadDataSignal();
    void initProcessDataSignal();

    void startProcessData ();

    void setLatestIndexData(QList<QStringList> selectIndexHistData,
                            QList<QStringList> hedgedIndexHistData);

    void releaseDataReaderSrc ();
    void releaseDataProcessSrc ();

signals:
    void receiveMarketHistData_Signal(QList<QList<double>> allData);
    void startReadDataSignal(QString dataType);
    void startProcessDataSignal(QString dataType);
    void tableViewInfoSignal(QString msg);

    void sendLatestHistIndexData_signal(QString, double, double, QList<int>);
    void sendHistIndexData_signal(QList<QString>, QList<double>, QList<double>,
                                  QList<double>, QList<double>, QList<double>);
    void sendHistIndexError_signal(QString);

public slots:
    void getHistData();
    void receiveOriginalData(QMap<QString, QList<QStringList>> subThreadData);
    void getProcessedData_slot(QList<QList<double>> allData);
    void getIndexHistData_slot();

private:
    bool                               m_isRealTime;
    int                                m_threadNumb;

    QString                            m_signalType;
    mutable QMutex                    m_mutex;

    int                                m_chartViewID;
    bool                               m_isBuySalePortfolio;

    QString                            m_dbhost;
    QString                            m_databaseName;
    QString                            m_startDate;
    QString                            m_endDate;

    QString                            m_hedgeIndexCode;
    QList<int>                         m_macdTime;
    QStringList                        m_keyValueList;

    QMap<QString, int>                 m_buyStrategyMap;
    QMap<QString, int>                 m_saleStrategyMap;
    QMap<QString, int>                 m_seocdebuyCountMap;
    QList<QString>                     m_secodeNameList;

    QList<DataProcess*>                m_dataProcessList;
    QList<QThread*>                    m_dataProcessThreadList;

    QList<QStringList>                 m_threadSecodeList;
    QList<DataRead*>                   m_dataReaderList;
    QList<QThread*>                    m_dataReaderThreadList;
    int                                m_readDataThreadCount;
    QMap<QString, QList<QStringList>>  m_completeTableData;

    // 指数对冲信息;
    Database*                          m_database;
    QString                            m_selectIndex;
    QString                            m_hedgedIndex;
    int                                m_aveNumb;
    double                             m_css12Rate;
    double                             m_cssRate1;
    double                             m_cssRate2;
    double                             m_maxCSS;
    double                             m_minCSS;
};

#endif // HISTORYDATA_H
