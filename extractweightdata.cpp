#include <QDebug>
#include "extractweightdata.h"
#include "excel_func.h"
#include "readdatabasedata.h"
#include "toolfunc.h"
#include "xlsxdocument.h"
#include "time_func.h"

ExtractWeightData::ExtractWeightData(QString dbhost, int dbConnectID,
                                     QString startDate, QString endDate,
                                     QStringList indexCodeList, QString desDirName,
                                     int threadCount,QObject *parent) :
    m_dbConnectID(dbConnectID), m_dbhost(dbhost),
    m_startDate(startDate), m_endDate(endDate),
    m_indexCodeList(indexCodeList), m_desDirName(desDirName),
    m_readThreadCount(threadCount), QObject(parent)
{
    initCommonData();

    initDateList();

    allocateThreadData();

    initReadThreadCount();

    initWorkProgressDialog();

    initIndexCodeMap();

    registerMetatype();

    createReadThreads();
}

ExtractWeightData::~ExtractWeightData() {
    for (int i = 0; i < m_ReadThreadList.size(); ++i) {
        if (!m_ReadThreadList[i]->isFinished()) {
            m_ReadThreadList[i]->quit();
        }
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

void ExtractWeightData::initCommonData() {
    m_startTime = QDateTime::currentDateTime();
    m_currCompleteCount = 0;
}

void ExtractWeightData::initDateList() {
    QDate startDate = getDate(m_startDate);
    QDate endDate = getDate(m_endDate);
    for (QDate currDate = startDate; currDate<= endDate; currDate = currDate.addDays(1)) {
        m_dateList.append(currDate.toString("yyyyMMdd"));
    }
}

void ExtractWeightData::initWorkProgressDialog() {
    m_workProgressDialog = new WorkProgressDialog();
    m_workProgressDialog->setWindowTitle(QString::fromLocal8Bit("进度条"));

    connect(m_workProgressDialog, SIGNAL(stopWork_signal()),
            this, SLOT(stopWork_slot()) );
}

void ExtractWeightData::initReadThreadCount() {
    if (m_readThreadCount > m_readThreadData.size()) {
        m_readThreadCount = m_readThreadData.size();
    }
}

void ExtractWeightData::initIndexCodeMap() {
    m_indexCodeMap.insert("SH000016", QString::fromLocal8Bit("上证50"));
    m_indexCodeMap.insert("SH000300", QString::fromLocal8Bit("沪深300"));
    m_indexCodeMap.insert("SH000852", QString::fromLocal8Bit("中证1000"));
    m_indexCodeMap.insert("SH000904", QString::fromLocal8Bit("中证200"));
    m_indexCodeMap.insert("SH000905", QString::fromLocal8Bit("中证500"));
    m_indexCodeMap.insert("SH000906", QString::fromLocal8Bit("中证800"));
    m_indexCodeMap.insert("SZ399903", QString::fromLocal8Bit("中证100"));
}

void ExtractWeightData::registerMetatype() {
    qRegisterMetaType<QList<QStringList>>("QList<QStringList>");
}

void ExtractWeightData::allocateThreadData() {
    for (int i = 0; i < m_indexCodeList.size(); ++i) {
        for (int j = 0; j < m_dateList.size(); ++j) {
            QStringList tmpResult;
            tmpResult << m_indexCodeList[i] << m_dateList[j];
            m_readThreadData.append(tmpResult);
        }
    }
    printList(m_readThreadData, "m_readThreadData");
}

void ExtractWeightData::createReadThreads() {
    for (int i = 0; i < m_readThreadData.size(); ++i) {
        QString  indexCode = m_readThreadData[i][0];
        QString  indexDate = m_readThreadData[i][1];
        QThread* currThread = new QThread();
        int dbConnectID = m_dbConnectID + 10 + i;
        ReadDatabaseData* currReadDatabaseData = new ReadDatabaseData(m_dbhost, dbConnectID,
                                                                      indexDate, indexCode);
        currReadDatabaseData->moveToThread(currThread);

        connect(currThread, SIGNAL(finished()),
                currReadDatabaseData, SLOT(deleteLater()));

        connect(this, SIGNAL(startReadWeightData_signal()),
                currReadDatabaseData, SLOT(startReadWeightData_slot()));

        connect(currReadDatabaseData, SIGNAL(readWeightDataComplete_signal(QList<QStringList>)),
                this, SLOT(readWeightDataComplete_slot(QList<QStringList>)));

        connect(currReadDatabaseData, SIGNAL(readOneWeightDataComplete_signal()),
                this, SLOT(readOneWeightDataComplete_slot()));

        m_ReadThreadList.append(currThread);
        m_ReadDataObjList.append(currReadDatabaseData);
    }

    for (QThread* thread:m_ReadThreadList) {
        thread->start();
    }
}

void ExtractWeightData::startReadData() {
    qDebug() << "ExtractWeightData::startReadData";
    m_workProgressDialog->setWorkInfo(QString::fromLocal8Bit("开始读取数据"));
    m_workProgressDialog->setRange(m_indexCodeList.size(), 0);
    m_workProgressDialog->updateWorkState(0);
    m_workProgressDialog->show();
    emit startReadWeightData_signal();
}

void ExtractWeightData::storeData() {
    for (int i = 0; i < m_sumResult.size(); ++i) {
        QString indexCode = m_sumResult[i][0][0];
        QString indexDate = m_sumResult[i][0][1];
        m_sumResult[i].pop_front();
        QString currDesFileName = m_desDirName +  QString("%1_%2.xlsx")
                                .arg(m_indexCodeMap[indexCode]).arg(indexDate);
        m_desFileNameList.append(currDesFileName);
        writeMatrixExcelData(currDesFileName, m_sumResult[i], indexCode, false);
    }
}

int ExtractWeightData::writeMatrixExcelData(QString fileName, QList<QStringList>& oriData,
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

    m_workProgressDialog->setWorkInfo(QString::fromLocal8Bit("开始写入数据文件: %1").arg(fileName));
    m_workProgressDialog->setRange(oriData.length(), 0);
    for (int rowIndex = 0; rowIndex < oriData.length(); ++rowIndex) {
        for (int colIndex = 0; colIndex < oriData[rowIndex].length(); ++colIndex) {
            if (isTranspose) {
              xlsx.write(colIndex+1, rowIndex+1, oriData[rowIndex][colIndex]);
            } else {
              xlsx.write(rowIndex+1, colIndex+1, oriData[rowIndex][colIndex]);
            }
        }
        m_workProgressDialog->updateWorkState(rowIndex+1);
    }
    xlsx.save();
    return 1;
}

void ExtractWeightData::readWeightDataComplete_slot(QList<QStringList> threadResult) {
    QMutexLocker locker(&m_oneThreadCompleteMutex);
    m_sumResult.append(threadResult);
    threadResult.clear();
    if (m_sumResult.size() == m_readThreadData.size()) {
        m_workProgressDialog->disableStopButton();
        qDebug() << "m_sumResult.size: " << m_sumResult.size();
        storeData();

        m_endTime = QDateTime::currentDateTime();
        float costSeconds = (m_endTime.toMSecsSinceEpoch() - m_startTime.toMSecsSinceEpoch())/1000;
        qDebug() << "costSeconds: " << costSeconds;
        m_workProgressDialog->setWorkInfo(QString::fromLocal8Bit("所有数据文件已写入完成, 共耗时: %1 秒")
                                          .arg(costSeconds));
        emit extractWeightDataComplete_signal(m_desFileNameList);
    }
}

void ExtractWeightData::readOneWeightDataComplete_slot() {
    QMutexLocker locker(&m_readOneDataCompleteMutex);
    m_currCompleteCount += 1;
    qDebug() << QString("Complete: %1, %2 left.")
                .arg(m_currCompleteCount).arg(m_indexCodeList.size()-m_currCompleteCount);
    m_workProgressDialog->updateWorkState(m_currCompleteCount);
}

void ExtractWeightData::stopWork_slot() {
    qDebug() << "ExtractWeightData::stopWork_slot ";
    for (int i = 0; i < m_ReadThreadList.size(); ++i) {
        disconnect(m_ReadDataObjList[i], SIGNAL(readWeightDataComplete_signal(QList<QStringList>)),
                this, SLOT(readWeightDataComplete_slot(QList<QStringList>)));

        disconnect(m_ReadDataObjList[i], SIGNAL(readOneWeightDataComplete_signal()),
                this, SLOT(readOneWeightDataComplete_slot()));

        if (NULL != m_ReadThreadList[i]) {
            m_ReadThreadList[i]->terminate();
        }
    }
    m_workProgressDialog->updateWorkState(0);
}
