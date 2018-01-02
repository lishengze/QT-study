#include "dataread.h"

DataRead::DataRead(QString databaseConnID, QString dbhost,
                   QString dbName, QStringList tableNameList,
                   QString startDate, QString endDate, QStringList keyValueList,
                   QObject *parent):
    m_dbName(dbName), m_tableNameList(tableNameList),
    m_startDate(startDate), m_endDate(endDate), m_keyValueList(keyValueList),
    QObject(parent)
{
    m_database = new Database(QString(databaseConnID), dbhost);
}

void DataRead::receiveStartReadData(QString dataType) {
//    qDebug() << "DataRead::receiveStartReadData: " << QThread::currentThreadId();
    if (dataType == "HistoryData") {
        emit sendHistoryData (readHistoryData());
    }
    if (dataType == "RealTimeData") {
        emit sendRealTimeData (readRealTimeData());
    }
}

QMap<QString, QList<QStringList>> DataRead::readHistoryData () {
    qDebug() << "DataRead::readHistoryData: " << QThread::currentThreadId();
    QMap<QString, QList<QStringList>> result;
    for (int i = 0; i < m_tableNameList.size (); ++i) {
        QString secode = m_tableNameList[i];
        QString tableName = m_tableNameList[i];
        result.insert (secode, m_database->getOriChartData(m_startDate, m_endDate, m_keyValueList, tableName, m_dbName));
    }
    return result;
}

QMap<QString, QList<QStringList>> DataRead::readRealTimeData () {
    qDebug() << "DataRead::readRealTimeData: " << QThread::currentThreadId();
    QMap<QString, QList<QStringList>> result = m_database->getSnapShootHistoryData(m_tableNameList);
    return result;
}

DataRead::~DataRead () {
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
//    qDebug() << "~DataRead";
}
