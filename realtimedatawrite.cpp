#include <QDebug>
#include <QThread>
#include "realtimedatawrite.h"
#include "toolfunc.h"

RealTimeDataWrite::RealTimeDataWrite(QTableView* programInfoTableView, QTableView* errorMsgTableView,
                                     QString dbConnID, QString dbHost,
                                     QObject* parent):
    m_programInfoTableView(programInfoTableView), m_errorMsgTableView(errorMsgTableView),
    m_dbConnID(dbConnID), m_dbHost(dbHost),
    QObject(parent)
{
    initCommonData();
    initDatabase();
    registerParams();
    initMonitorTimer();
}

void RealTimeDataWrite::initCommonData() {
    m_currProccessState = false;
    m_dataTimeTable = "DataTime";
    m_realtimeDataNumb = 0;
    m_writeMinimumTime = 2000;
    m_writeRealTimeDataCount = 0;
    m_currCompleteThreadCount = 0;
    m_currSuccessNumb = 0;
    m_subWriteThreadNumb = 0;

    m_testWriteStartTime = 0;
    m_testWriteSumTime = 0;
    m_testWriteAveTime = 0;
    m_testWriteCount = 0;

    m_waitWriteTime = 10;
}


void RealTimeDataWrite::initDatabase() {
//    m_dbName ="Test";
//    m_dbHost = "localhost";
    m_dbName = "MarketData_RealTime";
    m_realtimeDatabase = new RealTimeDatabase(m_dbConnID, m_dbName, m_dbHost);
    updateProgramInfo(m_programInfoTableView,
                      QString::fromLocal8Bit("当前写入的数据库为: %1, %2")
                      .arg(m_dbName).arg(m_dbHost));
}

void RealTimeDataWrite::initMonitorTimer() {
    m_monitorExceptionWaitTime = 10000;
    updateProgramInfo(m_programInfoTableView,
                      QString::fromLocal8Bit("写入异常监听时间为: %1 毫秒")
                      .arg(m_monitorExceptionWaitTime));
    connect(&m_monitorExceptionTimer, SIGNAL(timeout()),
             this, SLOT(monitorException_slot()));
}

void RealTimeDataWrite::startMonitorTimer() {
    m_currProccessState = false;
    m_monitorExceptionTimer.start(m_monitorExceptionWaitTime);
//    updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("开启了写入监听器"));

    qDebug() << QString::fromLocal8Bit("开启了写入监听器, 剩余时间: %1")
                .arg(m_monitorExceptionTimer.remainingTime());
}

void RealTimeDataWrite::stopMonitorTimer() {
    m_currProccessState = true;
    if (m_monitorExceptionTimer.isActive()) {
        m_monitorExceptionTimer.stop();
//        qDebug() << "m_monitorExceptionTimer state: " << m_monitorExceptionTimer.isActive();
    }
//    updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("关闭了写入监听器"));

    qDebug() << QString::fromLocal8Bit("关闭了写入监听器, 剩余时间: %1")
                .arg(m_monitorExceptionTimer.remainingTime());
}

void RealTimeDataWrite::monitorException_slot() {
    // 若是失败， 放弃此次写入，重置写入信息与监听器信息，并发送信号给读取线程;
    qDebug() << "Write::monitorException_slot" << QTime::currentTime();
    if (false == m_currProccessState) {
        resetWriteSource();
        emit stopMonitorTimer_signal();
        updateProgramInfo(m_errorMsgTableView,
                          QString::fromLocal8Bit("此时插入实时数据失败, 失败原因: 超时"));
        emit writeComplete_signal(-1);
    } else {
        updateProgramInfo(m_errorMsgTableView,
                          QString::fromLocal8Bit("此时插入实时数据成功, 耗时: %1 毫秒")
                                                .arg(m_testCurrWriteTime));
    }
}

void RealTimeDataWrite::registerParams() {
    qRegisterMetaType<QList<QString>>("QList<QString>");
}

bool RealTimeDataWrite::checkDatabase() {
    bool result = true;
    if (m_realtimeDatabase->checkdataTime(m_dataTimeTable)) {
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("实时数据表已经创建好"));
    } else {
        if (clearDatabase()) {
            QString compTableMsg = m_realtimeDatabase->completeTable(m_secodeList);
            QString compFutureMsg = m_realtimeDatabase->completeFutureTable(m_futureList);
            QString compDatatimeMsg = m_realtimeDatabase->completeDataTimeTable(m_dataTimeTable);

            if (compTableMsg != "SUCCESS") {
                result = false;
                updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("创建股票数据表失败, 错误消息为: %1").arg(compTableMsg));
            } else {
                updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("创建股票数据表成功"));
            }

            if (compFutureMsg != "SUCCESS") {
                result = false;
                updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("创建期货数据表失败, 错误消息为: %1").arg(compFutureMsg));
            } else {
                updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("创建期货数据表成功"));
            }

            if (compDatatimeMsg != "SUCCESS") {
                result = false;
                updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("创建数据时间表失败, 错误消息为: %1").arg(compFutureMsg));
            } else {
                updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("创建数据时间表成功"));
            }
        } else {
            result = false;
        }
    }
    return result;
}

bool RealTimeDataWrite::clearDatabase() {
    bool result = true;
    QString clearMsg = "SUCCESS";
    QString curMsg = "";
    QList<QString> tableList = m_realtimeDatabase->getTableList(m_dbName);
    QString programStartTable = "ProgramInfo";
    tableList.removeOne(programStartTable);
    for (int i = 0; i < tableList.size(); ++i) {
        QString tmpMsg = m_realtimeDatabase->dropTable(tableList[i]);
        if (tmpMsg != "SUCCESS") {
            curMsg += tmpMsg + "\n";
        }
    }
    if (curMsg != "") {
        clearMsg = curMsg;
    }

    if (clearMsg != "SUCCESS") {
        result = false;
        updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("清空昨日实时交易数据失败, 错误消息为: %1").arg(clearMsg));
    } else {
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("清空昨日实时交易数据成功，准备创建当日实时数据表"));
    }
    return result;
}

void RealTimeDataWrite::setTableList_slot(QList<QString> secodeList, QList<QString> futureList) {
    m_futureList = futureList;
    m_secodeList = secodeList;
    m_realtimeDataNumb = 0;
    if (checkDatabase()) {
        emit setTableListComplete_signal();
    }
}

void RealTimeDataWrite::writePreCloseData_slot(QMap<QString, QStringList> preCloseData) {
    qDebug() << "RealTimeDataWrite::writePreCloseData";
    QString tableName = "PreCloseData";
    QList<QString> tableList = m_realtimeDatabase->getTableList(m_realtimeDatabase->getDatabaseName());
    bool result = true;
    if (tableList.indexOf(tableName) < 0 ) {
        if (m_realtimeDatabase->createPreCloseTable(tableName) ) {
            for (QMap<QString, QStringList>::iterator it = preCloseData.begin();
                 it != preCloseData.end(); ++it) {
                if (!m_realtimeDatabase->insertPreCloseData(tableName, it.value())) {
                    result = false;
                    updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("插入股票 %1,昨收数据失败").arg(tableName));
                    break;
                }
            }
            if (result) {
                updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("成功插入当日昨收数据"));
            }
        } else {
            updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("创建昨收数据表失败"));
        }
    } else {
        updateProgramInfo(m_programInfoTableView, QString::fromLocal8Bit("昨收数据已经存在"));
    }
    if (result) {
         emit setPrecloseDataComplete_signal();
    }
}

void RealTimeDataWrite::writeRealTimeData_slot(QMap<QString, QStringList> secodeResult, QMap<QString, QStringList> futureResult) {
// 设置标记位，并开始计时，规定时间后 标记位为false, 视为失败;
    emit startMonitorTimer_signal();

    writeFutureData(futureResult);

    QList<QMap<QString, QStringList>> data = allocateData(secodeResult);
    createSubWriteThreads(data);
    emit writeRealTimeData_signal();
}

void RealTimeDataWrite::writeFutureData(QMap<QString, QStringList> futureResult) {
    for (QMap<QString ,QStringList>::iterator it = futureResult.begin();
         it != futureResult.end(); ++it) {
        if (m_realtimeDatabase->insertFutureData(it.key(), it.value())) {
            ++m_currSuccessNumb;
        } else {
            updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("插入%1 实时数据失败").arg(it.key()));
        }
    }
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
        QString currConnId = QString("%1").arg(5 + i +  m_writeRealTimeDataCount * (m_subWriteThreadNumb + 1));
//        qDebug() << "currConnId: " << currConnId;
        SubWriteClass* subWriteObj = new SubWriteClass(data[i], currConnId, m_dbName, m_dbHost);
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

void RealTimeDataWrite::releaseSubClassThreads() {
    qDebug() << "Releae SubWriteThread: " << m_subWriteThreadList.size();
    for (int i = 0; i < m_subWriteThreadNumb ; ++i) {
        int subThreadIndex = m_subWriteThreadList.size() - i - 1;
        SubWriteClass* subWriteObj = m_subWriteObjList[subThreadIndex];

        disconnect(this, SIGNAL(writeRealTimeData_signal()),
                subWriteObj, SLOT(writeRealTimeData_slot()));

        disconnect(subWriteObj, SIGNAL(writeRealTimeResult_signal(QList<QString>)),
                this,SLOT(writeRealTimeResult_slot(QList<QString>)));

        if (!m_subWriteThreadList[subThreadIndex]->isFinished()) {
             m_subWriteThreadList[subThreadIndex]->quit();
        }
//        qDebug() << subThreadIndex;
    }
}

void RealTimeDataWrite::writeRealTimeResult_slot(QList<QString> result) {
    if (!m_monitorExceptionTimer.isActive()) {
        qDebug() << QString::fromLocal8Bit("此次写入已经超时，停止接受数据，等待下次写入唤醒");
        return;
    }

    QMutexLocker locker(&m_mutex);
    m_currSuccessNumb += result[result.size()-1].toInt();
    result.removeLast();
    m_currWriteErrorReuslt += result;
//    qDebug() << "m_currCompleteThreadCount: " << m_currCompleteThreadCount;
    if (++m_currCompleteThreadCount == m_subWriteThreadNumb) {
        updateProgramInfo(m_programInfoTableView,
                          QString::fromLocal8Bit("插入 %1 实时数据, 数据总数: %2")
                          .arg(m_currSuccessNumb).arg(++m_realtimeDataNumb));

        if (m_currWriteErrorReuslt.size() > 0) {
            updateProgramInfo(m_errorMsgTableView, QString::fromLocal8Bit("插入实时数据出现失败, 失败数目为: %1")
                                                   .arg(m_currWriteErrorReuslt.size()));
        }

        m_testCurrWriteTime = QDateTime::currentDateTime().currentMSecsSinceEpoch() - m_testWriteStartTime;
        m_testWriteSumTime += m_testCurrWriteTime;
        m_testWriteAveTime = m_testWriteSumTime/++m_testWriteCount;

        qDebug() << "m_currSuccessNumb: " << m_currSuccessNumb << ", "
                 << "errorMsg_numb: " << m_currWriteErrorReuslt.size() << ", "
                 << "costMSecs: " << m_testCurrWriteTime <<", "
                 << "costAveMSecs: " << m_testWriteAveTime;

        if (m_testCurrWriteTime < m_writeMinimumTime) {
            QThread::msleep(m_writeMinimumTime - m_testCurrWriteTime);
        }

        resetWriteSource();

        emit stopMonitorTimer_signal();

        emit writeComplete_signal(0);
    }
}

void RealTimeDataWrite::resetWriteSource() {
    releaseSubClassThreads();
    m_currCompleteThreadCount = 0;
    m_currSuccessNumb = 0;
    m_currWriteErrorReuslt.clear();    
}

RealTimeDataWrite::~RealTimeDataWrite() {
    emit stopMonitorTimer_signal();
    qDebug() << "m_subWriteThreadList.size: "  << m_subWriteThreadList.size();
    for (int i = 0; i < m_subWriteThreadList.size(); ++i) {
        if (!m_subWriteThreadList[i]->isFinished()) {
             m_subWriteThreadList[i]->quit();
        }
    }

    for (int i = 0; i < m_subWriteObjList.size(); ++i) {
        if (m_subWriteObjList[i] != NULL) {
            delete m_subWriteObjList[i];
            m_subWriteObjList[i] = NULL;
        }
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


