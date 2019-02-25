#include "dataread.h"
#include "io_func.h"

DataRead::DataRead(QString dbhost, QString dbName, QStringList tableNameList,
                   QString startDate, QString endDate, QStringList keyValueList,
                   QObject *parent):
    m_dbName(dbName),  m_tableNameList(tableNameList),
    m_startDate(startDate), m_endDate(endDate), m_keyValueList(keyValueList),
    QObject(parent)
{
    m_database = new Database(dbhost);
}


void DataRead::setTableNameList(QList<QString> codeList)
{
    m_tableNameList = codeList;
}

void DataRead::getOrinPortfolioData_slot(QString dataType) {
    if (dataType == "HistoryData") 
    {        
        // qDebug() << "Emit HistoryData: " << QThread::currentThreadId();
        emit sendHistoryData (readHistoryData());
       
    }
    if (dataType == "RealTimeData") 
    {        
        // qDebug() << "Emit RealTimeData: " << QThread::currentThreadId();
        emit sendHistoryData (readRealTimeData());   
    }

    if (dataType == "All")
    {
        QMap<QString, QList<QStringList>> longHistResult = readHistoryData();
        QMap<QString, QList<QStringList>> todyHistResult = readRealTimeData();
        emit sendHistoryData (longHistResult.unite(todyHistResult));
    }
}

QMap<QString, QList<QStringList>> DataRead::readHistoryData () 
{
    QMap<QString, QList<QStringList>> result = m_database->getLongTimeHistoryData(m_startDate, m_endDate,
                                                                                  m_keyValueList, m_tableNameList, m_dbName);
    return result;
}

QMap<QString, QList<QStringList>> DataRead::readRealTimeData () 
{
    QMap<QString, QList<QStringList>> result = m_database->getSnapShootHistoryData(m_tableNameList);
    return result;
}

DataRead::~DataRead () 
{
    if (NULL != m_database) 
    {
        delete m_database;
        m_database = NULL;
    }
}
