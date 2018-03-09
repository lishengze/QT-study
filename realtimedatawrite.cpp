#include <QDebug>
#include <QThread>
#include "realtimedatawrite.h"
#include "toolfunc.h"

RealTimeDataWrite::RealTimeDataWrite(QTableView* programInfoTableView, QTableView* errorMsgTableView,
                                     QString dbConnID, QString dbName, QString dbHost,
                                     QObject* parent):
    m_programInfoTableView(programInfoTableView), m_errorMsgTableView(errorMsgTableView),
    m_dbConnID(dbConnID), m_dbHost(dbHost), m_dbName(dbName),
    QObject(parent)
{
    m_realtimeDataNumb = 0;
    m_writeRealTimeDataCount = 0;
    m_currCompleteThreadCount = 0;
    m_currSuccessNumb = 0;
    m_subWriteThreadNumb = 0;

    m_testWriteStartTime = 0;
    m_testWriteSumTime = 0;
    m_testWriteAveTime = 0;
    m_testWriteCount = 0;

    initDatabase();
    registerParams();
}

void RealTimeDataWrite::initDatabase() {
    m_realtimeDatabase = new RealTimeDatabase(m_dbConnID, m_dbName, m_dbHost);
}

void RealTimeDataWrite::registerParams() {
    qRegisterMetaType<QList<QString>>("QList<QString>");
}

void RealTimeDataWrite::checkDatabase() {
    QString clearMsg;
    if (!m_realtimeDatabase->checkdataTime()) {
        clearDatabase();
    } else {
        updateProgramInfo(m_programInfoTableView, QString("实时Datebase已经存在"));
    }

    QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);
    if (compTableMsg != "SUCCESS") {
        updateProgramInfo(m_errorMsgTableView, QString("补完Database 失败, 错误消息为: %1").arg(compTableMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("根据股票代码补全Database"));
    }
}

void RealTimeDataWrite::clearDatabase() {
    QString clearMsg = "SUCCESS";
    QString curMsg = "";
    QList<QString> tableList = m_realtimeDatabase->getTableList(m_dbName);
    tableList.removeOne("secodeList");
    for (int i = 0; i < tableList.size(); ++i) {
        QString tmpMsg = dropTable(tableList[i]);
        if (tmpMsg != "SUCCESS") {
            curMsg += tmpMsg + "\n";
        }
    }
    if (curMsg != "") {
        clearMsg = curMsg;
    }

    if (clearMsg != "SUCCESS") {
        updateProgramInfo(m_errorMsgTableView, QString("清空Database 失败, 错误消息为: %1").arg(clearMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("完成Database清空工作"));
    }

    QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);
    if (compTableMsg != "SUCCESS") {
        updateProgramInfo(m_errorMsgTableView, QString("补完Database 失败, 错误消息为: %1").arg(compTableMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("根据策略股票代码补全Database"));
    }
}

void RealTimeDataWrite::setSecodeList_slot(QList<QString> data) {
    m_secodeList = data;
    checkDatabase();
    emit setRealTimeData_signal();
}

QList<QMap<QString, QStringList>> RealTimeDataWrite::allocateData(QMap<QString, QStringList> oriData) {
    m_testWriteStartTime = QDateTime::currentDateTime().currentMSecsSinceEpoch();
    QList<QMap<QString, QStringList>> result;
    m_subWriteThreadNumb = oriData.size() / 150;
    if (m_subWriteThreadNumb == 0) {
        m_subWriteThreadNumb = 1;
    }

    for (int i = 0; i < m_subWriteThreadNumb; ++i) {
        result.append (EmptyQStringQStringListMap());
    }

    QList<QString> secodeList = oriData.keys();
    for (int i = 0; i < secodeList.size(); i+=m_subWriteThreadNumb) {
        for (int j = 0; j < m_subWriteThreadNumb && i + j < secodeList.size(); ++j) {
            QString secode = secodeList[i+j];
            result[j].insert(secode, oriData[secode]);
        }
    }
    return result;
}

void RealTimeDataWrite::createSubWriteThreads(QList<QMap<QString, QStringList>> data) {
    ++m_writeRealTimeDataCount;
    for (int i = 0; i < data.size(); ++i) {
        SubWriteClass* subWriteObj = new SubWriteClass(data[i], QString("%1").arg(10 + i +  m_writeRealTimeDataCount * (m_subWriteThreadNumb + 1)),
                                                       m_dbName, m_dbHost);
        QThread* subWriteThread = new QThread();

        subWriteObj->moveToThread(subWriteThread);

        connect(subWriteThread, SIGNAL(finished()),
                subWriteObj, SLOT(deleteLater()));

        connect(this, SIGNAL(writeRealTimeData_signal()),
                subWriteObj, SLOT(writeRealTimeData_slot()));

        connect(subWriteObj, SIGNAL(writeRealTimeResult_signal(QList<QString>)),
                this,SLOT(writeRealTimeResult_slot(QList<QString>)));

        subWriteThread -> start();
        m_subWriteObjList.append(subWriteObj);
        m_subWriteThreadList.append(subWriteThread);
    }
}

void RealTimeDataWrite::writeRealTimeResult_slot(QList<QString> result) {
    QMutexLocker locker(&m_mutex);
    m_currSuccessNumb += result[result.size()-1].toInt();
    result.removeLast();
    m_currWriteReuslt += result;
//    qDebug() << "m_currSuccessNumb: " << m_currSuccessNumb;
    qDebug() << "m_currCompleteThreadCount: " << m_currCompleteThreadCount;
    if (++m_currCompleteThreadCount == m_subWriteThreadNumb) {

        updateProgramInfo(m_programInfoTableView,
                          QString("插入 %1 实时数据, 数据总数: %2").arg(m_currSuccessNumb).arg(++m_realtimeDataNumb));

        for (QString msg : m_currWriteReuslt) {
            updateProgramInfo(m_errorMsgTableView, QString("插入实时数据失败, 失败消息: %1").arg(msg));
        }

        releaseSubClassThreads();

        int costMSecs = QDateTime::currentDateTime().currentMSecsSinceEpoch() - m_testWriteStartTime;
        m_testWriteSumTime += costMSecs;
        m_testWriteAveTime = m_testWriteSumTime/++m_testWriteCount;

        qDebug() << "m_currSuccessNumb: " << m_currSuccessNumb
                 << "errorMsg_numb: " << m_currWriteReuslt.size()
                 << ", costMSecs: " << costMSecs
                 << ", costAveMSecs: " << m_testWriteAveTime;

        if (costMSecs < 1000) {
            QThread::msleep(1000-costMSecs);
        }

        m_currCompleteThreadCount = 0;
        m_currSuccessNumb = 0;
        m_currWriteReuslt.clear();

        emit writeComplete_signal();
    }
}

void RealTimeDataWrite::releaseSubClassThreads() {
    for (int i = 0; i < m_subWriteThreadList.size(); ++i) {
         m_subWriteThreadList[i]->quit ();
    }
}

void RealTimeDataWrite::writeRealTimeData_slot(QMap<QString, QStringList> result) {
//    qDebug() << "RealTimeDataWrite::writeRealTimeData_slot, thread: " << QThread::currentThreadId();
    QList<QMap<QString, QStringList>> data = allocateData(result);
    createSubWriteThreads(data);
    emit writeRealTimeData_signal();
}

void RealTimeDataWrite::writePreCloseData_slot(QMap<QString, QStringList> result) {
    qDebug() << "RealTimeDataWrite::writePreCloseData";
    QString tableName = "PreCloseData";
    QList<QString> tableList = m_realtimeDatabase->getTableList(m_realtimeDatabase->getDatabaseName());
    if (tableList.indexOf(tableName) < 0 ) {
        for (QMap<QString, QStringList>::iterator it = result.begin();
             it != result.end(); ++it) {
            m_realtimeDatabase->insertPreCloseData(tableName, it.value());
        }
        updateProgramInfo(m_programInfoTableView, QString("插入昨收数据"));
    } else {
        updateProgramInfo(m_programInfoTableView, QString("昨收数据已经存在"));
    }
}

RealTimeDataWrite::~RealTimeDataWrite() {
    qDebug() << "m_subWriteThreadList.size: "  << m_subWriteThreadList.size();
    for (int i = 0; i < m_subWriteThreadList.size(); ++i) {
         m_subWriteThreadList[i]->wait();
         m_subWriteThreadList[i]->quit();
    }

    for (int i = 0; i < m_subWriteThreadList.size(); ++i) {
         delete m_subWriteThreadList[i];
         m_subWriteThreadList[i] = NULL;
    }

    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
}
