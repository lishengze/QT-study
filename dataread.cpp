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

void DataRead::getOrinPortfolioData_slot(QString dataType) {
    if (dataType == "HistoryData") 
    {
        emit sendHistoryData (readHistoryData());
    }
    if (dataType == "RealTimeData") 
    {
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

    for (QMap<QString, QList<QStringList>>::const_iterator it = result.begin();
        it != result.end(); ++it)
    {
        // qDebug() << it.key() << it.value().size();
        // printList(it.value(), it.key());
    }

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
