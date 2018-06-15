#include <QDebug>
#include "extractindustrydata.h"
#include "excel_func.h"
#include "readdatabasedata.h"
#include "toolfunc.h"
#include "time_func.h"
#include "xlsxdocument.h"

ExtractIndustryData::ExtractIndustryData(int dbConnectID, QString dbhost,
                                         QString startDate, QString endDate,
                                         QStringList industryList, QString desDirName,
                                         int threadCount, QObject *parent):
    m_dbConnectID(dbConnectID), m_dbhost(dbhost),
    m_startDate(startDate), m_endDate(endDate),
    m_industryList(industryList), m_desDirName(desDirName),
    m_readThreadCount(threadCount), QObject(parent)
{
    initCommonData();

    initDateList();

    initReadThreadCount();

    initWorkProgressDialog();

    registerMetatype();

    allocateThreadData();

    createReadThreads();
}

ExtractIndustryData::~ExtractIndustryData() {
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

void ExtractIndustryData::initCommonData() {
    m_startTime = QDateTime::currentDateTime();
    m_currCompleteCount = 0;
    m_industryList.insert(0, QString::fromLocal8Bit("日期"));
    m_industryList.insert(1, QString::fromLocal8Bit("股票代码"));
    m_industryList.insert(2, QString::fromLocal8Bit("股票名称"));
}

void ExtractIndustryData::initDateList() {
    QDate startDate = getDate(m_startDate);
    QDate endDate = getDate(m_endDate);
    for (QDate currDate = startDate; currDate<= endDate; currDate = currDate.addDays(1)) {
        m_dateList.append(currDate.toString("yyyyMMdd"));
    }
}

void ExtractIndustryData::initWorkProgressDialog() {
    m_workProgressDialog = new WorkProgressDialog();
    m_workProgressDialog->setWindowTitle(QString::fromLocal8Bit("进度条"));

    connect(m_workProgressDialog, SIGNAL(stopWork_signal()),
            this, SLOT(stopWork_slot()) );
}

void ExtractIndustryData::initReadThreadCount() {
    if (m_readThreadCount > m_dateList.size()) {
        m_readThreadCount = m_dateList.size();
    }
}

void ExtractIndustryData::registerMetatype() {
    qRegisterMetaType<QList<QStringList>>("QList<QStringList>");
}

void ExtractIndustryData::allocateThreadData() {
    m_readThreadData = m_dateList;
}

void ExtractIndustryData::createReadThreads() {
    for (int i = 0; i < m_readThreadData.size(); ++i) {
        QString  currThreadData = m_readThreadData[i];
        QThread* currThread = new QThread();
        int dbConnectID = m_dbConnectID + 10 + i;
        ReadDatabaseData* currReadDatabaseData = new ReadDatabaseData(m_dbhost, dbConnectID,
                                                                      currThreadData, m_industryList);
        currReadDatabaseData->moveToThread(currThread);

        connect(currThread, SIGNAL(finished()),
                currReadDatabaseData, SLOT(deleteLater()));

        connect(this, SIGNAL(startReadIndustryData_signal()),
                currReadDatabaseData, SLOT(startReadIndustryData_slot()));

        connect(currReadDatabaseData, SIGNAL(readIndustryDataComplete_signal(QList<QStringList>)),
                this, SLOT(readIndustryDataComplete_slot(QList<QStringList>)));

        connect(currReadDatabaseData, SIGNAL(readOneIndustryDataComplete_signal()),
                this, SLOT(readOneIndustryDataComplete_slot()));

        m_ReadThreadList.append(currThread);
        m_ReadDataObjList.append(currReadDatabaseData);
    }

    for (QThread* thread:m_ReadThreadList) {
        thread->start();
    }
}

void ExtractIndustryData::startReadData() {
    qDebug() << "ExtractIndustryData::startReadData";
    m_workProgressDialog->setWorkInfo(QString::fromLocal8Bit("开始读取数据"));
    m_workProgressDialog->setRange(m_dateList.size(), 0);
    m_workProgressDialog->updateWorkState(0);
    m_workProgressDialog->show();
    emit startReadIndustryData_signal();
}

void ExtractIndustryData::storeData() {
    for (int i = 0; i < m_sumResult.size(); ++i) {
        QString date = m_sumResult[i][0][0];
        m_sumResult[i].pop_front();
        QString currDesFileName = m_desDirName + QString::fromLocal8Bit("行业分类_%1.xlsx").arg(date);
        m_desFileNameList.append(currDesFileName);
        writeMatrixExcelData(currDesFileName, m_sumResult[i], date, false);
    }
}

int ExtractIndustryData::writeMatrixExcelData(QString fileName, QList<QStringList>& oriData,
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

void ExtractIndustryData::readIndustryDataComplete_slot(QList<QStringList> threadResult) {
    QMutexLocker locker(&m_oneThreadCompleteMutex);
    m_sumResult.append(threadResult);
    threadResult.clear();
    if (m_sumResult.size() == m_dateList.size()) {
        m_workProgressDialog->disableStopButton();

        storeData();

        m_endTime = QDateTime::currentDateTime();
        float costSeconds = (m_endTime.toMSecsSinceEpoch() - m_startTime.toMSecsSinceEpoch())/1000;
        qDebug() << "costSeconds: " << costSeconds;
        m_workProgressDialog->setWorkInfo(QString::fromLocal8Bit("所有数据文件已写入完成, 共耗时: %1 秒")
                                          .arg(costSeconds));
        emit extractIndustryDataComplete_signal(m_desFileNameList);
    }
}

void ExtractIndustryData::readOneIndustryDataComplete_slot() {
    QMutexLocker locker(&m_readOneDataCompleteMutex);
    m_currCompleteCount += 1;
    qDebug() << QString("Complete: %1, %2 left.")
                .arg(m_currCompleteCount).arg(m_dateList.size()-m_currCompleteCount);
    m_workProgressDialog->updateWorkState(m_currCompleteCount);
}

void ExtractIndustryData::stopWork_slot() {
    qDebug() << "ExtractWeightData::stopWork_slot ";
    for (int i = 0; i < m_ReadThreadList.size(); ++i) {
        disconnect(m_ReadDataObjList[i], SIGNAL(readIndustryDataComplete_signal(QList<QStringList>)),
                this, SLOT(readIndustryDataComplete_slot(QList<QStringList>)));

        disconnect(m_ReadDataObjList[i], SIGNAL(readOneIndustryDataComplete_signal()),
                this, SLOT(readOneIndustryDataComplete_slot()));

        if (NULL != m_ReadThreadList[i]) {
            m_ReadThreadList[i]->terminate();
        }
    }
    m_workProgressDialog->updateWorkState(0);
}
