﻿#include "extractmarketdata.h"
#include "excel_func.h"
#include "readdatabasedata.h"
#include "toolfunc.h"
#include "xlsxdocument.h"
#include "io_func.h"
#include "secode_func.h"
#include <QDebug>
#pragma execution_character_set("utf-8")

ExtractMarketData::ExtractMarketData(QString dbhost, QString dataType,
                                     QString startDate, QString endDate,
                                     QStringList secodeList, QString desDirName,
                                     QStringList keyValueList, int threadCount,
                                     QObject *parent):
    m_dbhost(dbhost),m_dataType(dataType),
    m_startDate(startDate), m_endDate(endDate),
    m_secodeList(secodeList), m_desDirName(desDirName),
    m_keyValueList(keyValueList), m_readThreadCount(threadCount),
    QObject(parent)
{
    initCommonData();

    initWorkProgressDialog();

    initDatabase();

    initIndexTimeList();

    initReadThreadCount();

    initSecodeMap();

    registerMetatype();

    allocateThreadData();
}

ExtractMarketData::~ExtractMarketData() {
    qDebug() << "ExtractMarketData::~ExtractMarketData";
    for (int i = 0; i < m_ReadThreadList.size(); ++i) {
        if (!m_ReadThreadList[i]->isFinished()) {
            m_ReadThreadList[i]->quit();
        }
    }

    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }

    for (int i = 0; i < m_ReadThreadList.size(); ++i) {
        if (NULL != m_ReadThreadList[i]) {
            delete m_ReadThreadList[i];
            m_ReadThreadList[i] = NULL;
        }
    }

    if (NULL != m_workProgressDialog) {
        m_workProgressDialog->close();
        delete m_workProgressDialog;
        m_workProgressDialog = NULL;
    }
}

void ExtractMarketData::initCommonData() {
    m_startTime = QDateTime::currentDateTime();
//    printList(m_secodeList, "m_secodeList");
//    m_secodeList = getSubList(m_secodeList, 0, 8);
    m_currCompleteCount = 0;
    initKeyValueMap();
}

void ExtractMarketData::initKeyValueMap() {
    m_keyValueMap.insert("TOPEN", QString("开盘价"));
    m_keyValueMap.insert("TCLOSE", QString("收盘价"));
    m_keyValueMap.insert("HIGH", QString("最高价"));
    m_keyValueMap.insert("LOW", QString("最低价"));
    m_keyValueMap.insert("VATRUNOVER", QString("成交量"));
    m_keyValueMap.insert("VOTRUNOVER", QString("成交额"));
    m_keyValueMap.insert("YCLOSE", QString("昨收"));
    m_keyValueMap.insert("TURNOVER", QString("换手率"));

    m_keyValueMap.insert("TOPEN", QString("开盘价"));
    m_keyValueMap.insert("TCLOSE", QString("收盘价"));
    m_keyValueMap.insert("HIGH", QString("最高价"));
    m_keyValueMap.insert("LOW", QString("最低价"));
    m_keyValueMap.insert("VATRUNOVER", QString("成交量"));
    m_keyValueMap.insert("VOTRUNOVER", QString("成交额"));
    m_keyValueMap.insert("YCLOSE", QString("昨收"));
    m_keyValueMap.insert("TURNOVER", QString("换手率"));
}

void ExtractMarketData::initWorkProgressDialog() {
    m_workProgressDialog = new WorkProgressDialog();
    m_workProgressDialog->setWindowTitle(QString("进度条"));

    connect(m_workProgressDialog, SIGNAL(stopWork_signal()),
            this, SLOT(stopWork_slot()) );
}

void ExtractMarketData::initDatabase() {
    m_database = new Database("1", m_dbhost);
}

void ExtractMarketData::initSecodeMap() {
    m_secodeMap.insert(m_indexTimeStrList[0], 0);
    for (int i = 0; i < m_secodeList.size(); ++i) {
        m_secodeMap.insert(m_secodeList[i], i+1);
    }
//    printMap(m_secodeMap, "m_secodeMap");
}

void ExtractMarketData::initIndexTimeList() {
    m_indexTimeList = getIndexTimeList(m_startDate, m_endDate, m_dataType, m_dbhost);
    QStringList keyValueList;
    keyValueList << "TDATE" << "TIME";
    m_indexCode = "SH000001";
    m_indexTimeList = m_database->getDataByDate(m_startDate, m_endDate, keyValueList,
                                                m_indexCode, m_dataType);
    m_indexTimeStrList = getIndexTimeStrList(m_indexTimeList);
    m_indexTimeStrList.insert(0, "time/code");
}

void ExtractMarketData::initReadThreadCount() {
    m_readThreadCount = 16;
    if (m_secodeList.size() < m_readThreadCount) {
        m_readThreadCount = m_secodeList.size();
    }
}

void ExtractMarketData::registerMetatype() {
    qRegisterMetaType<QList<QList<QStringList>>>("QList<QList<QStringList>>");
}

void ExtractMarketData::allocateThreadData() {
    for (int i = 0; i <m_readThreadCount; ++i) {
        QStringList emptyStringList;
        m_readThreadData.append(emptyStringList);
    }
    for (int i = 0; i < m_secodeList.size(); i+= m_readThreadCount) {
        for (int j = 0; j < m_readThreadCount && i + j < m_secodeList.size(); ++j) {
            m_readThreadData[j].append(m_secodeList[i+j]);
        }
    }
}

void ExtractMarketData::createReadThreads() {
    for (int i = 0; i < m_readThreadData.size(); ++i) {
        QStringList currThreadData = m_readThreadData[i];
        QThread* currThread = new QThread();
        ReadDatabaseData* currReadDatabaseData = new ReadDatabaseData(m_dbhost, 1, m_dataType, m_startDate, m_endDate,
                                                                      currThreadData, m_indexTimeList, m_keyValueList);
        currReadDatabaseData->moveToThread(currThread);

        connect(currThread, SIGNAL(finished()),
                currReadDatabaseData, SLOT(deleteLater()));

        connect(this, SIGNAL(startReadMarketData_signal()),
                currReadDatabaseData, SLOT(startReadMarketData_slot()));

        connect(currReadDatabaseData, SIGNAL(readMarketDataComplete_signal(QList<QList<QStringList>>)),
                this, SLOT(readMarketDataComplete_slot(QList<QList<QStringList>>)));

        connect(currReadDatabaseData, SIGNAL(readOneMarketDataComplete_signal()),
                this, SLOT(readOneMarketDataComplete_slot()) );

        connect(currReadDatabaseData, SIGNAL(testSignal_signal()),
                this, SLOT(testSignal_slot()));

        m_ReadThreadList.append(currThread);
        m_ReadDataObjList.append(currReadDatabaseData);
    }

    for (QThread* thread:m_ReadThreadList) {
        thread->start();
    }
}

void ExtractMarketData::startReadData() {
    m_workProgressDialog->setWorkInfo(QString("开始读取数据"));
    m_workProgressDialog->setRange(m_secodeList.size(), 0);
    m_workProgressDialog->updateWorkState(0);
    m_workProgressDialog->show();

    createReadThreads();
    emit startReadMarketData_signal();
}

void ExtractMarketData::testSignal_slot() {
    qDebug() << "ExtractMarketData::testSignal_slot()";
}

void ExtractMarketData::readOneMarketDataComplete_slot() {
    QMutexLocker locker(&m_readOneDataCompleteMutex);
    m_currCompleteCount += 1;
    //    qDebug() << QString("Complete: %1, %2 left.")
    //                .arg(m_currCompleteCount).arg( m_secodeList.size() - m_currCompleteCount);
    m_workProgressDialog->updateWorkState(m_currCompleteCount);
}

void ExtractMarketData::readMarketDataComplete_slot(QList<QList<QStringList>> threadResult) {
    QMutexLocker locker(&m_oneThreadCompleteMutex);
    m_sumResult += threadResult;
//    qDebug() << QString("m_sumResult.size: %1, threadResult.size: %2")
//                .arg(m_sumResult.size()).arg(threadResult.size());
    threadResult.clear();
    if (m_sumResult.size() == m_secodeList.size()) {
        m_workProgressDialog->disableStopButton();

        storeData();

        m_endTime = QDateTime::currentDateTime();
        float costSeconds = (m_endTime.toMSecsSinceEpoch() - m_startTime.toMSecsSinceEpoch())/1000;
        qDebug() << "costSeconds: " << costSeconds;
        m_workProgressDialog->setWorkInfo(QString("所有数据文件已写入完成, 共耗时: %1 秒").arg(costSeconds));
        emit extractMarketDataComplete_signal(m_desFileNameList);
    }
}

void ExtractMarketData::stopWork_slot() {
    qDebug() << "ExtractMarketData::stopWork_slot ";
    for (int i = 0; i < m_ReadThreadList.size(); ++i) {
        disconnect(m_ReadDataObjList[i], SIGNAL(readMarketDataComplete_signal(QList<QList<QStringList>>)),
                this, SLOT(readMarketDataComplete_slot(QList<QList<QStringList>>)));

        disconnect(m_ReadDataObjList[i], SIGNAL(readOneMarketDataComplete_signal()),
                this, SLOT(readOneMarketDataComplete_slot()));

        if (NULL != m_ReadThreadList[i]) {
            m_ReadThreadList[i]->terminate();
        }
    }
    m_workProgressDialog->updateWorkState(0);
}

void ExtractMarketData::storeData() {
    QMap<QString, QList<QStringList>> excelData;
    for (QString keyValue:m_keyValueList) {
        if (keyValue == "ROE") continue;
        QList<QStringList> emptyData;
        emptyData.append(m_indexTimeStrList);
        for(int i = 0; i < m_secodeList.size()+1; ++i) {
            QStringList emptyStringList;
            emptyData.append(emptyStringList);
        }
        excelData.insert(keyValue, emptyData);
    }

    m_workProgressDialog->setWorkInfo(QString("开始处理数据"));
    for (int i = 0; i < m_sumResult.size(); ++i) {
        QList<QStringList> currSecodeData = m_sumResult[i];
        QString currSecode = currSecodeData[0][0];
        currSecodeData.pop_front();

        if (m_keyValueList.indexOf("ROE") >= 0) {
            QList<QString> singleResult = currSecodeData.last();
            QString currKey = "ROE";
            int secodePos = m_secodeMap[currSecode];
            currSecodeData.pop_back();
            singleResult.insert(0, currSecode);
            excelData[currKey][secodePos-1] = singleResult;
        }

        for (int j = 0; j < m_keyValueList.size(); ++j) {
            QString currKey = m_keyValueList[j];
            if (currKey == "ROE") continue;
            int secodePos = m_secodeMap[currSecode];
            QList<QString> singleResult = getSingleColData(currSecodeData, j);
            singleResult.insert(0, currSecode);
            excelData[currKey][secodePos] = singleResult;
        }
    }
    m_sumResult.clear();

    m_workProgressDialog->setWorkInfo(QString("准备写入数据"));
    for (QString keyValue:m_keyValueList) {
        QString currDesFileName = m_desDirName +  QString("%1_%2_%3_%4_%5.xlsx")
                .arg(m_dataType).arg(m_startDate).arg(m_endDate)
                .arg(m_keyValueMap[keyValue]).arg(m_secodeList.size());
        m_desFileNameList.append(currDesFileName);

        writeMatrixExcelData(currDesFileName, excelData[keyValue], keyValue, true);
        excelData[keyValue].clear();
    }
}

int ExtractMarketData::writeMatrixExcelData(QString fileName, QList<QStringList>& oriData,
                                            QString sheetName, bool isTranspose) {
    checkFile(fileName);
    QXlsx::Document xlsx(fileName);


    QStringList sheetNames = xlsx.sheetNames();
    if (sheetNames.indexOf(sheetName) < 0) {
        if (!xlsx.addSheet(sheetName)) {
            return -1;
        }
    }
    xlsx.moveSheet(sheetName, 0);
    if (!xlsx.selectSheet(sheetName)) {
        return -2;
    }

    QXlsx::Format numberFormat;
    numberFormat.setNumberFormat("0.0000");

    m_workProgressDialog->setWorkInfo(QString("开始写入数据文件: %1").arg(fileName));
    m_workProgressDialog->setRange(oriData.length(), 0);
    for (int rowIndex = 0; rowIndex < oriData.length(); ++rowIndex) {
        for (int colIndex = 0; colIndex < oriData[rowIndex].length(); ++colIndex) {
            if (isTranspose) {
              if (colIndex != 0 && rowIndex != 0) {
                xlsx.write(colIndex+1, rowIndex+1, oriData[rowIndex][colIndex], numberFormat);
              }else {
                xlsx.write(colIndex+1, rowIndex+1, oriData[rowIndex][colIndex]);
              }

            } else {
                if (colIndex != 0 && rowIndex != 0) {
                    xlsx.write(rowIndex+1, colIndex+1, oriData[rowIndex][colIndex], numberFormat);
                }else {
                    xlsx.write(rowIndex+1, colIndex+1, oriData[rowIndex][colIndex]);
                }

            }
        }
        m_workProgressDialog->updateWorkState(rowIndex+1);
    }
    xlsx.save();
    return 1;
}
