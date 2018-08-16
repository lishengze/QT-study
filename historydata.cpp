#include "historydata.h"
#include "toolfunc.h"
#include "secode_func.h"
#include "process_data_func.h"
#include "time_func.h"
#include "compute_func.h"
#include "io_func.h"
#pragma execution_character_set("utf-8")

HistoryData::HistoryData(int chartViewID, bool isBuySalePortfolio,
                         QString dbhost, QString databaseName,
                         bool isRealTime, int threadNumb, QList<QString> secodeNameList,
                         QList<int> macdTime, QString hedgeIndexCode,
                         QString startDate, QString endDate, QStringList keyValueList,
                         QMap<QString, int> seocdebuyCountMap, QMap<QString, int> buyStrategyMap,
                         QMap<QString, int> saleStrategyMap,
                         QObject *parent):
    m_chartViewID(chartViewID), m_isBuySalePortfolio(isBuySalePortfolio),
    m_dbhost(dbhost), m_databaseName(databaseName),
    m_isRealTime(isRealTime), m_threadNumb(threadNumb), m_secodeNameList(secodeNameList),
    m_macdTime(macdTime), m_hedgeIndexCode(hedgeIndexCode),
    m_startDate(startDate), m_endDate(endDate), m_keyValueList(keyValueList),
    m_seocdebuyCountMap(seocdebuyCountMap), m_buyStrategyMap(buyStrategyMap),
    m_saleStrategyMap(saleStrategyMap),
    QObject(parent)
{
    initCommonData();
    initSignalType();
    initThreadSecodeList();
    initReadDataSignal();
}

HistoryData::HistoryData(int chartViewID, QString dbhost, QString databaseName,
            QString selectIndex, QString hedgedIndex,
            QString startDate, QString endDate, bool isRealTime,
            int aveNumb, double css12Rate,
            double cssRate1, double cssRate2,
            double maxCSS, double minCSS,
            QObject *parent):
    m_chartViewID(chartViewID), m_dbhost(dbhost), m_databaseName(databaseName),
    m_selectIndex(selectIndex), m_hedgedIndex(hedgedIndex),
    m_startDate(startDate), m_endDate(endDate), m_isRealTime(isRealTime),
    m_aveNumb(aveNumb), m_css12Rate(css12Rate),
    m_cssRate1(cssRate1), m_cssRate2(cssRate2),
    m_maxCSS(maxCSS), m_minCSS(minCSS),
    QObject(parent)
{
    initCommonData();
    initDatabase(chartViewID);
}

HistoryData::HistoryData(QString dbhost, QString databaseName,
                         QString startDate, QString endDate, QString codeName,
                         QList<int> aveNumbList, QList<bool> isEMAList,
                         int mainAveNumb, int subAveNumb, int energyAveNumb,
                         double css12Rate, double mainCssRate1, double mainCssRate2,
                         double energyCssRate1, double energyCssRate2,
                         double maxCSS, double minCSS,
                         QObject *parent):
    m_dbhost(dbhost), m_databaseName(databaseName),
    m_startDate(startDate), m_endDate(endDate), m_singleCodeName(codeName),
    m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
    m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
    m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
    m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
    m_maxCSS(maxCSS), m_minCSS(minCSS),
    QObject(parent)
{
    initCommonData();
    initDatabase(0);
}

HistoryData::~HistoryData() {
    for (int i = 0; i < m_dataProcessThreadList.size(); ++i) {
         delete m_dataProcessThreadList[i];
         m_dataProcessThreadList[i] = NULL;
    }

    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         delete m_dataReaderThreadList[i];
         m_dataReaderThreadList[i] = NULL;
    }

    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
}

void HistoryData::initCommonData() {
    m_database = NULL;
    m_readDataThreadCount = 0;
}

void HistoryData::initSignalType() {
    if (m_isRealTime) {
        m_signalType = "RealTimeData";
    } else {
        m_signalType = "HistoryData";
    }
}

void HistoryData::initDatabase(int chartViewID) {
    QString databaseID = QString("%1").arg(chartViewID + 10);
    m_database = new Database(databaseID, m_dbhost);
}

void HistoryData::initThreadSecodeList() {
    for (int i = 0; i < m_threadNumb; ++i) {
        QStringList emptyList;
        m_threadSecodeList.append (emptyList);
    }

    for (int i = 0; i < m_secodeNameList.size(); i+=m_threadNumb) {
        for (int j = 0; j < m_threadNumb && i + j < m_secodeNameList.size(); ++j) {
            m_threadSecodeList[j].append (m_secodeNameList[i+j]);
        }
    }
}

void HistoryData::initReadDataSignal() {
    for (int i = 0; i < m_threadSecodeList.size (); ++i) {
        DataRead* curDataReader;
        if (m_threadSecodeList[i].size() > 0) {
            QString databaseId = QString("%1").arg(i + m_chartViewID * m_threadNumb + 10);
            if (m_isRealTime) {
                curDataReader = new DataRead(databaseId, m_dbhost, m_databaseName, m_threadSecodeList[i]);
            } else {
                curDataReader = new DataRead(databaseId, m_dbhost, m_databaseName, m_threadSecodeList[i],
                                             m_startDate, m_endDate, m_keyValueList);
            }
        }
        QThread* curThread = new QThread();
        curDataReader->moveToThread(curThread);

        connect (curThread, SIGNAL(finished()),
                 curDataReader, SLOT(deleteLater()));

        connect (this, SIGNAL(startReadDataSignal(QString)),
                 curDataReader, SLOT(receiveStartReadData(QString)));

        connect (curDataReader, SIGNAL(sendHistoryData(QMap<QString, QList<QStringList>>)),
                 this, SLOT(receiveOriginalData(QMap<QString, QList<QStringList>>)), Qt::QueuedConnection);

        curThread->start();
        m_dataReaderList.append(curDataReader);
        m_dataReaderThreadList.append(curThread);
    }
}

void HistoryData::getHistData() {
    qDebug() << "HistoryData::getHistData: " << QThread::currentThreadId() << ", signalType: " << m_signalType;
    emit startReadDataSignal(m_signalType);
}

void HistoryData::receiveOriginalData (QMap<QString, QList<QStringList>> subThreadData) {
    QMutexLocker locker(&m_mutex);
    m_completeTableData.unite (subThreadData);
    ++m_readDataThreadCount;
    emit tableViewInfoSignal(QString(QString("完成股票指数读取数目: %1"))
                             .arg(m_completeTableData.size ()));

    if (m_readDataThreadCount == m_threadNumb) {

        releaseDataReaderSrc ();
        m_readDataThreadCount = 0;
        if (m_completeTableData.find("Error")!= m_completeTableData.end()) {
            emit tableViewInfoSignal(QString("Error: 读取数据出错"));
        } else {
            startProcessData();
        }
        m_completeTableData.clear ();
    }
}

void HistoryData::initProcessDataSignal() {
    int oridataCount = 0;
    for (int i = 0; i < m_secodeNameList.size (); ++i) {
        oridataCount += m_completeTableData[m_secodeNameList[i]].size ();
    }
    emit tableViewInfoSignal(QString(QString("原始数据获取完成, 原始数据总数: %1"))
                             .arg(oridataCount));

    DataProcess* curDataProcess;
    if (m_isBuySalePortfolio) {
        curDataProcess = new DataProcess(m_isRealTime, m_isBuySalePortfolio, m_completeTableData,
                                         m_buyStrategyMap, m_saleStrategyMap, m_macdTime);
    } else {
        curDataProcess = new DataProcess(m_isRealTime, m_isBuySalePortfolio, m_completeTableData,
                                         m_seocdebuyCountMap, m_hedgeIndexCode, m_macdTime);
    }

    QThread* curDataProcessThread = new QThread();
    curDataProcess->moveToThread (curDataProcessThread);

    connect (curDataProcessThread, SIGNAL(finished()),
             curDataProcess, SLOT(deleteLater()));

    connect (this, SIGNAL(startProcessDataSignal(QString)),
             curDataProcess, SLOT(receiveOrigianlHistoryData(QString)));

    connect(curDataProcess, SIGNAL(getProcessedData_signal(QList<QList<double>>)),
            this, SLOT(getProcessedData_slot (QList<QList<double>>)));

    curDataProcessThread->start ();
    m_dataProcessList.append (curDataProcess);
    m_dataProcessThreadList.append (curDataProcessThread);
}

void HistoryData::startProcessData () {
    initProcessDataSignal();
    emit startProcessDataSignal("all");
//    qDebug() << "Start Process History Data!!";
    emit tableViewInfoSignal(QString("开始计算历史数据"));
}

void HistoryData::getProcessedData_slot (QList<QList<double>> allData) {
//    qDebug() << "HistoryData::receiveProcessedHistoryData: " << QThread::currentThreadId();
    emit receiveMarketHistData_Signal(allData);
    releaseDataProcessSrc();
}

void HistoryData::releaseDataReaderSrc () {
//    qDebug() << "ReleaseDataReaderThread:  " <<  m_dataReaderThreadList.size();
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         m_dataReaderThreadList[i]->quit ();
    }
}

void HistoryData::releaseDataProcessSrc () {
//    qDebug() << "ReleaseDataProcessThread: " <<  m_dataProcessThreadList.size();
    for (int i = 0; i < m_dataProcessThreadList.size (); ++i) {
        m_dataProcessThreadList[i]->quit ();
    }
}

void HistoryData::setLatestIndexData(QList<QStringList> selectIndexHistData,
                                     QList<QStringList> hedgedIndexHistData) {
    int datanumb = selectIndexHistData.size();
    QString latestTime = QString("%1 %2").arg(selectIndexHistData[datanumb-1][0])
                                          .arg(selectIndexHistData[datanumb-1][1]);
    double  latestSelectIndexPrice = selectIndexHistData[datanumb-1][2].toDouble();
    double  latestHedgedIndexPrice = hedgedIndexHistData[datanumb-1][2].toDouble();
    QList<int> standardTimeList = m_database->getOneDayTimeList(getCompleteIndexCode(m_hedgedIndex), m_databaseName);
    emit sendLatestHistIndexData_signal(latestTime,
                                        latestSelectIndexPrice,
                                        latestHedgedIndexPrice,
                                        standardTimeList);
}

void HistoryData::getIndexHistData_slot() {
    QStringList histKeyValueList;
    histKeyValueList << "TDATE" << "TIME" << "TCLOSE";
    QList<QStringList> selectIndexHistData = m_database->getDataByDate(m_startDate, m_endDate, histKeyValueList,
                                                                      getCompleteIndexCode(m_selectIndex), m_databaseName);
    QList<QStringList> hedgedIndexHistData = m_database->getDataByDate(m_startDate, m_endDate, histKeyValueList,
                                                                      getCompleteIndexCode(m_hedgedIndex), m_databaseName);

    if (selectIndexHistData.size() != hedgedIndexHistData.size()) {
        QString message = "选中的指数与对冲指数历史数据不完整";
        qDebug() << message;
        emit sendHistIndexError_signal(message);
    } else {
        QString latestDate = hedgedIndexHistData.last()[0];
        qDebug() << "latestDate: " << latestDate;
        if (m_isRealTime && latestDate != QDate::currentDate().toString("yyyyMMdd")) {
            QStringList todayKeyValueList;
            todayKeyValueList << "日期" << "时间" << "最新成交价";
            QList<QStringList> selectIndexTodayData = m_database->getAllRealtimeData(getCompleteIndexCode(m_selectIndex, "wind"),
                                                                                     todayKeyValueList);
            QList<QStringList> hedgedIndexTodayData = m_database->getAllRealtimeData(getCompleteIndexCode(m_hedgedIndex, "wind"),
                                                                                     todayKeyValueList);
            QList<int> standardTimeList = m_database->getOneDayTimeList(getCompleteIndexCode(m_hedgedIndex), m_databaseName);
            QList<QStringList> selectIndexTransData = transRealTimeDataToMinuteData(selectIndexTodayData, standardTimeList);
            QList<QStringList> hedgedIndexTransData = transRealTimeDataToMinuteData(hedgedIndexTodayData, standardTimeList);
            resizeMinuteData(selectIndexTransData, hedgedIndexTransData);
            selectIndexHistData += selectIndexTransData;
            hedgedIndexHistData += hedgedIndexTransData;
        }
        setLatestIndexData(selectIndexHistData, hedgedIndexHistData);
        QList<QStringList> profiltList = getRelativeProfitList(selectIndexHistData, hedgedIndexHistData);
        QList<QStringList> cssList = getCSSList(profiltList, m_aveNumb, m_css12Rate,
                                                m_cssRate1, m_cssRate2, m_maxCSS, m_minCSS);
        QList<QString> timeList;
        QList<double> earningList;
        QList<double> typeList;
        QList<double> preCSSList;
        QList<double> CSSTList;
        QList<double> CSSList;
        for (int i = 0; i < cssList.size(); ++i) {
            timeList.append(QString("%1 %2").arg(cssList[i][0]).arg(cssList[i][1]));
            earningList.append(cssList[i][2].toDouble());
            typeList.append(cssList[i][3].toDouble());
            CSSTList.append(cssList[i][4].toDouble());
            preCSSList.append(cssList[i][5].toDouble());
            CSSList.append(cssList[i][6].toDouble());
        }
//        printList(cssList, "cssList");
        emit sendHistIndexData_signal(timeList, earningList, typeList, CSSTList, preCSSList, CSSList);
    }

}

void HistoryData::getCSSData_slot() {
    QStringList keyList;
    keyList << "TDATE" << "TIME" << "TCLOSE" << "TOPEN";
    qDebug() << m_startDate << m_endDate << m_singleCodeName << m_databaseName;
    QList<QStringList> oriDatabaseData = m_database->getDataByDate(m_startDate, m_endDate, keyList,
                                                                   m_singleCodeName, m_databaseName);
//    printList(oriDatabaseData, "oriDatabaseData");
    QList<QString> timeList;
    QList<double> typList;
    QList<double> closeList;

    for (int i = 0; i < oriDatabaseData.size(); ++i) {
        timeList.append(QString("%1 %2").arg(oriDatabaseData[i][0]).arg(oriDatabaseData[i][1]));
        closeList.append(oriDatabaseData[i][2].toDouble());
        typList.append((oriDatabaseData[i][2].toDouble() + oriDatabaseData[i][3].toDouble()) / 2);
    }
    QList<QList<double>> aveList = getAVEList(closeList, m_aveNumbList, m_isEMAList);
    aveList.insert(0, closeList);
//    qDebug() << typList;
//    printList(aveList, "aveList");

    QList<double> mainList = getCSSList(typList,  m_mainAveNumb, m_css12Rate,
                                        m_mainCssRate1, m_mainCssRate2, m_maxCSS, m_minCSS,false);
    QList<double> subValueList = getCSSList(typList,  m_subAveNumb, m_css12Rate,
                                        m_mainCssRate1, m_mainCssRate2, m_maxCSS, m_minCSS,false);
    QList<double> energyValueList = getCSSList(typList,  m_energyAveNumb, m_css12Rate,
                                            m_energyCssRate1, m_energyCssRate2, m_maxCSS, m_minCSS, true);
//    qDebug() << mainList;
//    qDebug() << subValueList;
//    qDebug() << energyValueList;

    QList<QList<double>> cssList;
    cssList.append(mainList);
    cssList.append(subValueList);
    cssList.append(energyValueList);

    emit sendCSSData_signal(timeList, aveList, cssList);
}

