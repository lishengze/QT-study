#include "historydata.h"
#include "toolfunc.h"

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
    m_seocdebuyCountMap(seocdebuyCountMap), m_buyStrategyMap(buyStrategyMap), m_saleStrategyMap(saleStrategyMap),
    QObject(parent)
{
    initCommonData();
    initSignalType();
    initThreadSecodeList();
    initReadDataSignal();
}

void HistoryData::initCommonData() {
    m_readDataThreadCount = 0;
}

void HistoryData::initSignalType() {
    if (m_isRealTime) {
        m_signalType = "RealTimeData";
    } else {
        m_signalType = "HistoryData";
    }
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

//    qDebug() << "HistoryData::initReadDataSignal: " << QThread::currentThreadId();
}

void HistoryData::getHistData() {
    qDebug() << "HistoryData::getHistData: " << QThread::currentThreadId() << ", signalType: " << m_signalType;
    emit startReadDataSignal(m_signalType);
}

void HistoryData::receiveOriginalData (QMap<QString, QList<QStringList>> subThreadData) {
    QMutexLocker locker(&m_mutex);
    m_completeTableData.unite (subThreadData);
    ++m_readDataThreadCount;

//    qDebug() << "HistoryData::receiveOriginalData: " << QThread::currentThreadId()
//             << ", dataNumb: " << subThreadData.size ()
//             << ", m_readDataThreadCount: " << m_readDataThreadCount << "\n";

    emit tableViewInfoSignal(QString(QString::fromLocal8Bit("完成股票指数读取数目: %1"))
                             .arg(m_completeTableData.size ()));

    if (m_readDataThreadCount == m_threadNumb) {

        releaseDataReaderSrc ();
        m_readDataThreadCount = 0;
        if (m_completeTableData.find("Error")!= m_completeTableData.end()) {
            emit tableViewInfoSignal(QString::fromLocal8Bit("Error: 读取数据出错"));
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
    emit tableViewInfoSignal(QString(QString::fromLocal8Bit("原始数据获取完成, 原始数据总数: %1"))
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

    connect(curDataProcess, SIGNAL(sendAllData(QList<QList<double>>)),
            this, SLOT(receiveProcessedData (QList<QList<double>>)));

    curDataProcessThread->start ();
    m_dataProcessList.append (curDataProcess);
    m_dataProcessThreadList.append (curDataProcessThread);
}

void HistoryData::startProcessData () {
    initProcessDataSignal();
    emit startProcessDataSignal("all");
//    qDebug() << "Start Process History Data!!";
    emit tableViewInfoSignal(QString::fromLocal8Bit("开始计算历史数据"));
}

void HistoryData::receiveProcessedData (QList<QList<double>> allData) {
//    qDebug() << "HistoryData::receiveProcessedHistoryData: " << QThread::currentThreadId();
    emit receiveHistDataSignal(allData);
    releaseDataProcessSrc();
}

void HistoryData::releaseDataReaderSrc () {
    qDebug() << "ReleaseDataReaderThread:  " <<  m_dataReaderThreadList.size();
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         m_dataReaderThreadList[i]->quit ();
    }
}

void HistoryData::releaseDataProcessSrc () {
    qDebug() << "ReleaseDataProcessThread: " <<  m_dataProcessThreadList.size();
    for (int i = 0; i < m_dataProcessThreadList.size (); ++i) {
        m_dataProcessThreadList[i]->quit ();
    }
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
}
