#include "readdatabasedata.h"
#include <QString>
#include <QThread>
#include <QDebug>
#include "excel_func.h"
#include "secode_func.h"

ReadDatabaseData::ReadDatabaseData():QObject(0)
{

}

ReadDatabaseData::ReadDatabaseData(QString dbhost, int dbConnectID,QString dataType,
                                   QString stateDate, QString endDate, QStringList secodeList,
                                   QList<QStringList> indexTimeList, QStringList keyValueList,
                                   QObject *parent ):
    m_dbhost(dbhost), m_dbConnectID(dbConnectID), m_dataType(dataType),
    m_startDate(stateDate), m_endDate(endDate), m_secodeList(secodeList),
    m_indexTimeList(indexTimeList), m_keyValueList(keyValueList),
    QObject(parent)
{
    m_database = NULL;
    m_keyValueList.insert(0, "TDATE");
    m_keyValueList.insert(1, "TIME");
    initDatabase();
}

ReadDatabaseData::ReadDatabaseData(QString dbhost, int dbConnectID,
                                   QString date, QString indexCode, QObject *parent):
    m_dbhost(dbhost), m_dbConnectID(dbConnectID),
    m_indexDate(date),m_weightIndexCode(indexCode),
    QObject(parent)
{
    m_database = NULL;
    initDatabase();
}

ReadDatabaseData::ReadDatabaseData(QString dbhost, int dbConnectID,
                                   QString date, QStringList industryList,
                                   QObject *parent):
    m_dbhost(dbhost), m_dbConnectID(dbConnectID),
    m_industryDate(date), m_industryList(industryList),
    QObject(parent)
{
    m_database = NULL;
    initDatabase();
}

void ReadDatabaseData::initDatabase() {
    m_database = new Database(QString("%1").arg(m_dbConnectID), m_dbhost);
}

void ReadDatabaseData::releaseDatabase() {
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
}



void ReadDatabaseData::startReadMarketData_slot() {
    qDebug() << "receive startReadMarketData_signal" << QThread::currentThreadId();
    QList<QList<QStringList>> sumResult;
    for (QString secode : m_secodeList) {
//        if (m_dataType.indexOf("FundamentData") >= 0 && !(m_keyValueList.size() == 1 && m_keyValueList[0] == "ROE")) {
//            m_keyValueList.insert(0, "TDATE");
//        }
        QStringList ROEResult;
        if (m_keyValueList.indexOf("ROE") >= 0) {
            m_keyValueList.removeAll("ROE");
            QStringList roeKeyValueList;
            roeKeyValueList << "TDATE" << "ROE";
            QString databaseName = "ROEData";
            QList<QStringList> tmpROEResult = m_database->getDataByDate(m_startDate, m_endDate, roeKeyValueList,
                                                                        completeSecode(secode), databaseName);

            for (int i = 0; i < tmpROEResult.size(); ++i) {
                ROEResult.append(QString("%1 %2").arg(tmpROEResult[i][0]).arg(tmpROEResult[i][1]));
            }
        }
        QList<QStringList> ori_result = m_database->getDataByDate(m_startDate, m_endDate, m_keyValueList,
                                                                  completeSecode(secode), m_dataType);

        qDebug() << "Ori "<< secode << ori_result.size() << ", m_indexTimeList.size: " << m_indexTimeList.size();
        completeExcelData(ori_result, m_indexTimeList, m_keyValueList.size(), secode);
        qDebug() << "Com "<< secode << ori_result.size() << ", m_indexTimeList.size: " << m_indexTimeList.size();

        if (ori_result.size() != m_indexTimeList.size()) {
            qDebug() << secode << ori_result.size() << ", m_indexTimeList.size: " << m_indexTimeList.size();
        }

        QStringList title;
        title << secode;
        ori_result.insert(0, title);
        if (ROEResult.size() > 0) {
            ori_result.append(ROEResult);
        }
        sumResult.append(ori_result);
        emit readOneMarketDataComplete_signal();
    }
}

void ReadDatabaseData::startReadWeightData_slot() {
    qDebug() << "receive startReadWeightData_signal" << QThread::currentThreadId();

    QList<QStringList> ori_result = m_database->getWeightData(m_indexDate, m_weightIndexCode);

    QStringList title;
    title.append(m_weightIndexCode);
    title.append(m_indexDate);
    ori_result.insert(0, title);

    QStringList colStr;
    colStr.append(QString::fromLocal8Bit("指数代码"));
    colStr.append(QString::fromLocal8Bit("指数名称"));
    colStr.append(QString::fromLocal8Bit("指数成分日"));
    colStr.append(QString::fromLocal8Bit("指数截止日"));
    colStr.append(QString::fromLocal8Bit("股票代码"));
    colStr.append(QString::fromLocal8Bit("股票名称"));
    colStr.append(QString::fromLocal8Bit("比例"));
    colStr.append(QString::fromLocal8Bit("排名"));
    colStr.append(QString::fromLocal8Bit("数据来源"));
    ori_result.insert(1, colStr);

    emit readOneMarketDataComplete_signal();
    emit readWeightDataComplete_signal(ori_result);

    qDebug() << m_weightIndexCode << QThread::currentThreadId() << "Done!";
}

void ReadDatabaseData::startReadIndustryData_slot() {
    qDebug() << "receive startReadIndustryData_slot" << QThread::currentThreadId();
    QList<QStringList> ori_result = m_database->getIndustryData(m_industryList, m_industryDate);

    QStringList title;
    title << m_industryDate;
    ori_result.insert(0, title);

    QStringList colStr;
    for (int i = 0; i < m_industryList.size(); ++i) {
        colStr.append(m_industryList[i]);
    }
    ori_result.insert(1, colStr);
    qDebug() << ori_result.size();

    emit readOneIndustryDataComplete_signal();
    emit readIndustryDataComplete_signal(ori_result);

    qDebug() << m_industryDate << QThread::currentThreadId() << "Done!";
}

ReadDatabaseData::~ReadDatabaseData() {
    qDebug() << "ReadDatabaseData::~ReadDatabaseData";
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
}
