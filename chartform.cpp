#include <QDebug>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QChartView>
#include <QPointF>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QScatterSeries>
#include <QGridLayout>
#include <QBarCategoryAxis>
#include <QCategoryAxis>
#include <QtMath>
#include <QMouseEvent>
#include <QPoint>
#include <QStackedBarSeries>
#include <QBarSet>
#include <QMessageBox>
#include <QTimer>
#include <QMetaType>
#include <QMutexLocker>

#include "chartform.h"
#include "ui_chartform.h"
#include "toolfunc.h"
#include "macd.h"
#include "callout.h"

//#pragma execution_character_set("utf-8")
extern QMap<QString, QList<QStringList>> g_wsqData;
extern QMutex g_wsqMutex;

QMap<QString, double> g_seocdebuyCountMap;
QList<QString> g_secodeNameList;

ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
//    QGridLayout* mainLayout = new QGridLayout(this);
//    mainLayout->addWidget (ui->gridLayoutWidget);
}

ChartForm::ChartForm(QWidget *parent, QTableView* programInfoTableView, int chartViewID,
                     QString startDate, QString endDate, QString timeType,
                     QList<strategy_ceil> strategyList, QString strategyName,
                     QString hedgeIndexCode, int hedgeIndexCount,
                     QList<int> macdTime, int threadNumb,
                     QString databaseName):
    QWidget(parent), m_programInfoTableView(programInfoTableView), m_chartViewID(chartViewID),
    m_startDate(startDate), m_endDate(endDate), m_timeType(timeType),
    m_strategy(strategyList), m_strategyName(strategyName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),    
    m_macdTime(macdTime), m_threadNumb(threadNumb),
    m_isRealTime(false),
    ui(new Ui::ChartForm)
{
    initCommonData();
    initHistoryData(databaseName, timeType, strategyList);
}

ChartForm::ChartForm(QWidget *parent,
                     QTableView* programInfoTableView, int chartViewID,
                     QList<strategy_ceil> strategyList, QString strategyName,
                     QString hedgeIndexCode, int hedgeIndexCount,
                     int updateTime, QList<int> macdTime, bool isTestRealTime,
                     int threadNumb):
    QWidget(parent),
    m_programInfoTableView(programInfoTableView), m_chartViewID(chartViewID),
    m_strategy(strategyList), m_strategyName(strategyName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_updateTime(updateTime), m_macdTime(macdTime),
    m_threadNumb(threadNumb), m_bTestRealTime(isTestRealTime),
    m_isRealTime(true),
    m_setKeyMove(false),
    ui(new Ui::ChartForm)
{
    initCommonData();
    initRealTimeData();
}

ChartForm::ChartForm(QWidget *parent, QTableView* programInfoTableView, int chartViewID,
                     QList<strategy_ceil> strategyList, QString strategyName,
                     QString hedgeIndexCode, int hedgeIndexCount,
                     QList<int> macdTime, bool isTestRealTime,
                     int threadNumb, int updateTime,
                     QString startDate, QString endDate, QString timeType,
                     QString databaseName):
    QWidget(parent),
    m_programInfoTableView(programInfoTableView), m_chartViewID(chartViewID),
    m_strategy(strategyList), m_strategyName(strategyName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_macdTime(macdTime), m_bTestRealTime(isTestRealTime),
    m_threadNumb(threadNumb), m_updateTime(updateTime),
    m_isRealTime(true),
    ui(new Ui::ChartForm)
{
    initCommonData();
    initRealTimeData();
}

ChartForm::ChartForm(QWidget *parent, QTableView* programInfoTableView, int chartViewID,
                     QList<strategy_ceil> buyStrategyList, QList<strategy_ceil> saleStrategyList,
                     QList<int> macdTime, bool isRealTime, int updateTime,
                     QString startDate, QString endDate, QString timeType,
                     QString databaseName):
            QWidget(parent),
            m_programInfoTableView(programInfoTableView), m_chartViewID(chartViewID),
            m_buyStrategy(buyStrategyList), m_saleStrategy(saleStrategyList),
            m_macdTime(macdTime), m_isRealTime(isRealTime), m_updateTime(updateTime),
            m_startDate(startDate), m_endDate(endDate), m_timeType(timeType),
            m_databaseName(databaseName),
            m_hedgeIndexCode(""), m_hedgeIndexCount(-1), m_bTestRealTime(false),
            ui(new Ui::ChartForm)
{
    initCommonData();
    if (m_isRealTime) {

    } else {
        initHistoryData();
    }
}

void ChartForm::initCommonData() {
    m_readDataThreadCount = 0;
    m_startTime = QTime::currentTime();
    m_dbhost = "192.168.211.165";

    m_oldPointDistance = -1;
    m_setMouseTimeIndex = false;
    m_timeAxisUpdatePercent = 0.2;
    m_keyMoveCount = 0;
    m_mouseInitPos = QPoint(-1, -1);
    m_monitorWorker = NULL;
    m_preSpread = 0;
    m_isclosed = false;
    m_isLayoutSetted = false;
    m_macdTooltip = NULL;
    m_strategyTooltip = NULL;
    m_votrunoverTooltip = NULL;

    initSecodeList();
    registSignalParamsType();
}

void ChartForm::registSignalParamsType () {
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
    qRegisterMetaType<QMap<QString,QStringList>>("QMap<QString,QStringList>");
    qRegisterMetaType<ChartData>("ChartData");
}

void ChartForm::initSecodeList() {
    if (m_buyStrategy.size() != 0 && m_saleStrategy.size() != 0) {
        m_strategy = m_buyStrategy + m_saleStrategy;
        if (m_isRealTime) {
            for (int i = 0; i < m_buyStrategy.size (); ++i) {
                m_buyStrategyMap.insert (getWindSecode(m_buyStrategy[i].m_secode), m_buyStrategy[i].m_buyCount);
            }
            for (int i = 0; i < m_saleStrategy.size (); ++i) {
                m_saleStrategyMap.insert (getWindSecode(m_saleStrategy[i].m_secode), m_saleStrategy[i].m_buyCount);
            }
        } else {
            for (int i = 0; i < m_buyStrategy.size (); ++i) {
                m_buyStrategyMap.insert (m_buyStrategy[i].m_secode, m_buyStrategy[i].m_buyCount);
            }
            for (int i = 0; i < m_saleStrategy.size (); ++i) {
                m_saleStrategyMap.insert (m_saleStrategy[i].m_secode, m_saleStrategy[i].m_buyCount);
            }
        }
    }
    if (m_isRealTime) {
        for (int i = 0; i < m_strategy.size (); ++i) {
            if (m_seocdebuyCountMap.keys().indexOf(getWindSecode(m_strategy[i].m_secode)) < 0) {
                m_seocdebuyCountMap.insert (getWindSecode(m_strategy[i].m_secode), m_strategy[i].m_buyCount);
            }
        }
        if (m_hedgeIndexCount != -1) {
            m_hedgeIndexCode = getWindSecode(m_hedgeIndexCode);
            m_seocdebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
        }
        m_secodeNameList = m_seocdebuyCountMap.keys();
    } else {
        for (int i = 0; i < m_strategy.size (); ++i) {
            if (m_seocdebuyCountMap.keys().indexOf(m_strategy[i].m_secode) < 0) {
                m_seocdebuyCountMap.insert (m_strategy[i].m_secode, m_strategy[i].m_buyCount);
            }
        }
        if (m_hedgeIndexCount != -1) {
            m_seocdebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
        }
        m_secodeNameList = m_seocdebuyCountMap.keys();
    }

//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        QString secode = m_secodeNameList[i];
//        if (m_buyStrategyMap.keys().indexOf(secode) < 0 &&  m_saleStrategyMap.keys().indexOf(secode) < 0) {
//            qDebug() << "secode: " << secode;
//        }
//    }
//    qDebug() << "m_secodeNameList.size: " << m_secodeNameList.size() << ", "
//             << "m_buyStrategyMap.size: " << m_buyStrategyMap.size()<< ", "
//             << "m_saleStrategyMap.size: " << m_saleStrategyMap.size();
}

void ChartForm::initHistoryData (QString databaseName, QString timeType, QList<strategy_ceil> strategyList) {
    m_databaseName = databaseName + "_" + timeType;
    m_keyValueList << "TCLOSE" << "VOTRUNOVER";

//    for (int i = 0; i < strategyList.size (); ++i) {
//        m_seocdebuyCountMap.insert (strategyList[i].m_secode, strategyList[i].m_buyCount);
//    }
//    m_seocdebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
//    m_secodeNameList = m_seocdebuyCountMap.keys();

    m_threadNumb = m_secodeNameList.size();
    if (m_timeType.contains("m") && m_timeType != "month") {
        m_timeTypeFormat = "yyyy-MM-dd h:mm";
    } else {
        m_timeTypeFormat = "yyyy-MM-dd";
    }
    m_chartXaxisTickCount = 10;
    startReadData ();
}

void ChartForm::initHistoryData () {
    m_databaseName += "_" + m_timeType;
    m_keyValueList << "TCLOSE" << "VOTRUNOVER";

    m_threadNumb = m_secodeNameList.size();
    if (m_timeType.contains("m") && m_timeType != "month") {
        m_timeTypeFormat = "yyyy-MM-dd h:mm";
    } else {
        m_timeTypeFormat = "yyyy-MM-dd";
    }
    m_chartXaxisTickCount = 10;
    startReadData ();
}

void ChartForm::initRealTimeData() {
    m_databaseName = "MarketData_RealTime";

//    QList<QString> oriSecodeList;
//    for (int i = 0; i < m_strategy.size (); ++i) {
//        m_seocdebuyCountMap.insert (getWindSecode(m_strategy[i].m_secode), m_strategy[i].m_buyCount);
//        oriSecodeList.append(m_strategy[i].m_secode);
//    }
//    oriSecodeList.append(m_hedgeIndexCode);
//    m_hedgeIndexCode = getWindSecode(m_hedgeIndexCode);
//    m_seocdebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
//    m_secodeNameList = m_seocdebuyCountMap.keys();
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        g_seocdebuyCountMap.insert(m_secodeNameList[i], m_seocdebuyCountMap[m_secodeNameList[i]]);
//    }
//    g_secodeNameList = m_secodeNameList;

    m_threadNumb = m_secodeNameList.size();
    m_timeTypeFormat = "hh:mm:ss";
    m_chartXaxisTickCount = 5;

    initMonitorThread();

    if (isTradingStart()) {
        startReadData();
    } else {
        setLayout();
        m_monitorWorker->startTimer();
    }
}

void ChartForm::initMonitorThread() {
    m_monitorWorker = new MonitorRealTimeData(m_updateTime, m_macdTime,
                                              m_seocdebuyCountMap, m_secodeNameList,
                                              m_hedgeIndexCode, m_hedgeIndexCount, m_bTestRealTime,
                                              QString("%1").arg(m_chartViewID), m_dbhost);

    connect(m_monitorWorker, SIGNAL(sendRealTimeData(ChartData)),
            this, SLOT(receiveRealTimeData(ChartData)));

    connect(this, SIGNAL(getPreData()),
            m_monitorWorker, SLOT(getPreData()));

    connect(m_monitorWorker, SIGNAL(sendPreCloseData(double)),
            this, SLOT(receivePreCloseData(double)));

    connect(m_monitorWorker, SIGNAL(sendTradeOver()),
            this, SLOT(receiveTradeOver()));

    m_monitorWorker->moveToThread(&m_MonitorThread);

    connect(&m_MonitorThread, SIGNAL(finished()),
            m_monitorWorker, SLOT(deleteLater()));

    m_MonitorThread.start();

    if (!m_bTestRealTime) {
        emit getPreData();
    }
}

void ChartForm::receivePreCloseData(double preSpread) {
    m_preSpread = preSpread;
    if (m_isLayoutSetted) {
        ui->preCLoseSpreadValue_Label->setText(QString("\346\230\250\346\227\245\347\202\271\345\267\256: %1").arg(m_preSpread));
    }
    qDebug() << "m_PreSpread: " << m_preSpread;
    disconnect(m_monitorWorker, SIGNAL(sendPreCloseData(double)),
               this, SLOT(receivePreCloseData(double)));
}

void ChartForm::receiveRealTimeData(ChartData curChartData) {
//    qDebug() << "ChartForm::receiveRealTimeData(): " << QThread::currentThread();
    m_strategyData.append(curChartData.strategyData);
    m_votData.append(curChartData.votData);
    m_timeData.append(curChartData.timeData);
    m_macdData.append(curChartData.macdData);
    updateChart();

    if (isTradingOver(QTime::currentTime())) {
        m_monitorWorker->stopTimer();
    }
}

void ChartForm::receiveTradeOver() {
    qDebug() << "Trade is over: " << QTime::currentTime().toString("HHmmss");
    m_monitorWorker->stopTimer();
}

void ChartForm::updateChart() {
    updateAxis();
    updateSeries();
}

QCategoryAxis* ChartForm::getTimeAxis() {
    QCategoryAxis* axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(m_timeAxisUpdateData.size (), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_timeAxisUpdateData[xpos]);
        axisX->append (tmpDatetime.toString (m_timeTypeFormat), xpos);
    }
    axisX->setMax(m_timeAxisUpdateData.size()-1);
    return axisX;
}

void ChartForm::setTimeAxisUpdateData() {
    int addedNumb = m_timeData.size() * m_timeAxisUpdatePercent;
//    qDebug() << "addedNumb: " << addedNumb;
    QDateTime curDateTime = QDateTime::fromMSecsSinceEpoch (m_timeData[m_timeData.size()-1]);
    QTime amStopTime = QTime(11, 30, 20);
    QTime pmStopTime = QTime(15, 0, 20);
    QTime pmStartTime = QTime(13, 0, 0);
    if (m_bTestRealTime) {
        for (int i = 0; i < addedNumb; ++i) {
            QDateTime tmpTime = curDateTime.addMSecs(m_updateTime * (i+1));
            m_timeAxisUpdateData.append(tmpTime.toMSecsSinceEpoch());
        }
    } else {
        for (int i = 0; i < addedNumb; ++i) {
            QDateTime tmpTime = curDateTime.addMSecs(m_updateTime * (i+1));
            if ((tmpTime.time() >= amStopTime && tmpTime.time() < pmStartTime) ||
                 tmpTime.time() >= pmStopTime ) {
                break;
            } else {
                m_timeAxisUpdateData.append(tmpTime.toMSecsSinceEpoch());
            }
        }
    }
}

void ChartForm::updateAxis() {
    int curPointNumb = m_timeData.size();
    QValueAxis *strategyAxisY = dynamic_cast<QValueAxis *>(m_strategyChart->axisY());
    QList<double> allStrategyData = m_strategyData;
    if (m_bTestRealTime) {
        m_preSpread = getAveValue(m_strategyData);
    }

    if (m_preSpread == 0) {
        emit(getPreData());
        if (m_strategyData.last() > strategyAxisY->max() || m_strategyData.last() < strategyAxisY->min()) {
            QList<double>  strategyAxisYRange = getChartYvalueRange(allStrategyData);
            strategyAxisY -> setRange (strategyAxisYRange[0], strategyAxisYRange[1]);
        }
    } else {
        allStrategyData.append(m_preSpread);
        if (m_preSpread > strategyAxisY->max() || m_preSpread < strategyAxisY->min() ||
            m_strategyData.last() > strategyAxisY->max() || m_strategyData.last() < strategyAxisY->min()) {
            QList<double>  strategyAxisYRange = getChartYvalueRange(allStrategyData);
            strategyAxisY -> setRange (strategyAxisYRange[0], strategyAxisYRange[1]);
        }
    }

    QValueAxis *votAxisY = dynamic_cast<QValueAxis *>(m_votrunoverChart->axisY());
    if (m_votData.last() > votAxisY->max() || m_votData.last() < votAxisY->min()) {
        QList<double>  votAxisYRange = getChartYvalueRange(m_votData);
        votAxisY -> setRange (votAxisYRange[0], votAxisYRange[1]);
    }

    QValueAxis *macdAxisY = dynamic_cast<QValueAxis *>(m_macdChart->axisY());
    if (m_macdData.last().m_dea > macdAxisY->max() || m_macdData.last().m_dea < macdAxisY->min() ||
        m_macdData.last().m_diff > macdAxisY->max() || m_macdData.last().m_diff < macdAxisY->min() ||
        m_macdData.last().m_macd > macdAxisY->max() || m_macdData.last().m_macd < macdAxisY->min()) {
        QList<double>  macdAxisYRange = getMACDRange(m_macdData);
        macdAxisY -> setRange (macdAxisYRange[0], macdAxisYRange[1]);
    }

    QCategoryAxis* strategyAxisX = dynamic_cast<QCategoryAxis*> (m_strategyChart->axisX());
    QCategoryAxis* votAxisX = dynamic_cast<QCategoryAxis*> (m_votrunoverChart->axisX());
    QCategoryAxis* macdAxisX = dynamic_cast<QCategoryAxis*> (m_macdChart->axisX());

    if (curPointNumb < m_chartXaxisTickCount) {
        strategyAxisX->append (QDateTime::fromMSecsSinceEpoch(m_timeData[curPointNumb-1]).toString (m_timeTypeFormat),
                                curPointNumb-1);
        strategyAxisX->setMax(curPointNumb-1);
        votAxisX->append (QDateTime::fromMSecsSinceEpoch(m_timeData[curPointNumb-1]).toString (m_timeTypeFormat),
                            curPointNumb-1);
        votAxisX->setMax(curPointNumb-1);
        macdAxisX->append (QDateTime::fromMSecsSinceEpoch(m_timeData[curPointNumb-1]).toString (m_timeTypeFormat),
                            curPointNumb-1);
        macdAxisX->setMax(curPointNumb-1);

        m_timeAxisUpdateData = m_timeData;
        updateMousePos();

    } else if (m_timeAxisUpdateData.size() <= m_timeData.size()){
        setTimeAxisUpdateData();
        m_strategyChart->removeAxis(strategyAxisX);
        QCategoryAxis* newStrategyAxisX = getTimeAxis();
        m_strategyChart->addAxis (newStrategyAxisX, Qt::AlignBottom);
        m_strategySeries->attachAxis (newStrategyAxisX);
        m_preSpreadSeries->attachAxis(newStrategyAxisX);

        m_votrunoverChart->removeAxis(votAxisX);
        QCategoryAxis* newVotAxisX = getTimeAxis();
        m_votrunoverChart->addAxis (newVotAxisX, Qt::AlignBottom);
        m_votBarSeries->attachAxis (newVotAxisX);

        m_macdChart->removeAxis(macdAxisX);
        QCategoryAxis* newMacdAxisX = getTimeAxis();
        m_macdChart->addAxis (newMacdAxisX, Qt::AlignBottom);
        m_diffSeries->attachAxis (newMacdAxisX);
        m_deaSeries->attachAxis (newMacdAxisX);
        m_macdSeries->attachAxis (newMacdAxisX);

        updateMousePos();
    } else {
//        qDebug() << "m_timeAxisUpdateData.size(): " << m_timeAxisUpdateData.size();
//        qDebug() << "m_timeData.size(): " << m_timeData.size();
    }

}

void ChartForm::updateSeries() {
    int curPointNumb = m_timeData.size();
    m_strategySeries->append(curPointNumb-1, m_strategyData[curPointNumb-1]);

    if (m_preSpreadSeries->points().size() != m_strategySeries->points().size()-1 && m_preSpread!=0) {
        qDebug() << "m_perSpread: " << m_preSpread << ", m_preSpreadSeries->points().size(): " << m_preSpreadSeries->points().size()
                 << ", m_strategySeries->points().size(): " << m_strategySeries->points().size();
        m_preSpreadSeries->clear();
        for (int i = 0; i < m_strategySeries->points().size()-1; ++i) {
            m_preSpreadSeries->append(i, m_preSpread);
        }
    }
    if (m_preSpread != 0) {
        m_preSpreadSeries->append(curPointNumb-1, m_preSpread);
    }

    QList<QBarSet *> votSetList = m_votBarSeries->barSets();
    votSetList[0]->append(m_votData[curPointNumb-1]);

    m_diffSeries->append(curPointNumb-1, m_macdData[curPointNumb-1].m_diff);
    m_deaSeries->append(curPointNumb-1, m_macdData[curPointNumb-1].m_dea);
    QList<QBarSet *> macdSetList = m_macdSeries->barSets();
    macdSetList[0]->append(m_macdData[curPointNumb-1].m_macd);

    int strategyHeight = m_strategyChartView->size().height();
    int votHeight = m_votrunoverChartView->size().height();
    int macdHeight = m_macdChartView->size().height();
    int sumHeight = strategyHeight + votHeight + macdHeight;
    strategyHeight = sumHeight / 2;
    votHeight = sumHeight / 4;
    macdHeight = sumHeight / 4;

    m_votrunoverChartView->setMaximumHeight(votHeight);
    m_macdChartView->setMaximumHeight(macdHeight);

    if (!m_setKeyMove) {
       setMouseMoveValue(curPointNumb-1);
    }
}

void ChartForm::updateMousePos() {
    if (m_mouseInitPos.x() != -1) {
        double newPointDistance = getPointXDistance();
        double rollbackDistance = m_oldPointDistance - newPointDistance;
        m_mouseInitPos.setX(m_mouseInitPos.x() - rollbackDistance);
        m_oldPointDistance = newPointDistance;
        QCursor::setPos(QCursor::pos().x() - rollbackDistance, QCursor::pos().y());
    }
}

QList<QStringList> ChartForm::allocateThreadData() {
    QList<QStringList> result;
    for (int i = 0; i < m_threadNumb; ++i) {
        QStringList emptyList;
        result.append (emptyList);
    }

    for (int i = 0; i < m_secodeNameList.size(); i+=m_threadNumb) {
        for (int j = 0; j < m_threadNumb && i + j < m_secodeNameList.size(); ++j) {
            result[j].append (m_secodeNameList[i+j]);
        }
    }
    return result;
}

void ChartForm::startReadData (){
    QList<QStringList> allocatedTableList = allocateThreadData ();
//    qDebug() << "allocatedTableList: " << allocatedTableList;
    for (int i = 0; i < allocatedTableList.size (); ++i) {
        DataRead* curDataReader;
        if (allocatedTableList[i].size() > 0) {
            QString databaseId = QString("%1").arg(i + m_chartViewID * m_threadNumb + 1);
            if (m_isRealTime) {
                curDataReader = new DataRead(databaseId, m_dbhost, m_databaseName, allocatedTableList[i]);
            } else {
                curDataReader = new DataRead(databaseId, m_dbhost, m_databaseName, allocatedTableList[i],
                                             m_startDate, m_endDate, m_keyValueList);
            }
        }
        QThread* curThread = new QThread();
        curDataReader->moveToThread(curThread);

        connect (curThread, SIGNAL(finished()),
                 curDataReader, SLOT(deleteLater()));

        connect (this, SIGNAL(sendStartReadDataSignal(QString)),
                 curDataReader, SLOT(receiveStartReadData(QString)));

        connect (curDataReader, SIGNAL(sendHistoryData(QMap<QString, QList<QStringList>>)),
                 this, SLOT(receiveOriginalData(QMap<QString, QList<QStringList>>)), Qt::QueuedConnection);

        curThread->start();
        m_dataReaderList.append(curDataReader);
        m_dataReaderThreadList.append(curThread);
    }

    qDebug() << "ChartForm::sendStartReadDataSignal: " << QThread::currentThreadId();

    if (m_isRealTime) {
        emit sendStartReadDataSignal("RealTimeData");
    } else {
        emit sendStartReadDataSignal("HistoryData");
    }
}

void ChartForm::receiveOriginalData (QMap<QString, QList<QStringList>> subThreadData) {
    QMutexLocker locker(&m_mutex);
    m_completeTableData.unite (subThreadData);
    ++m_readDataThreadCount;
//    qDebug() << "ChartForm::receiveOriginalData: " << QThread::currentThreadId()
//             << ", dataNumb: " << subThreadData.size ()
//             << ", m_readDataThreadCount: " << m_readDataThreadCount << "\n";

    updateProgramInfo (m_programInfoTableView, QString("完成股票指数读取数目: %1").arg(m_completeTableData.size ()));
    if (m_readDataThreadCount == m_threadNumb) {
        releaseDataReaderSrc ();
        m_readDataThreadCount = 0;
        startProcessHistoryData();
        m_completeTableData.clear ();
    }
//    qDebug() << "receiveOriginalData end!";
}

void ChartForm::startProcessHistoryData () {
    int oridataCount = 0;
    for (int i = 0; i < m_secodeNameList.size (); ++i) {
        oridataCount += m_completeTableData[m_secodeNameList[i]].size ();
    }
    updateProgramInfo (m_programInfoTableView, QString("原始数据获取完成, 原始数据总数: %1").arg(oridataCount));
    updateProgramInfo (m_programInfoTableView, QString("计算数据"));
//    DataProcess* curDataProcess = new DataProcess(m_completeTableData, m_seocdebuyCountMap, m_macdTime);
    DataProcess* curDataProcess;
    if (m_buyStrategy.size() != 0 && m_saleStrategy.size() != 0) {
        curDataProcess = new DataProcess(m_isRealTime, m_completeTableData,
                                         m_buyStrategyMap, m_saleStrategyMap, m_macdTime);
    } else {
        curDataProcess = new DataProcess(m_isRealTime, m_completeTableData,
                                         m_seocdebuyCountMap, m_hedgeIndexCode, m_macdTime);
    }

    QThread* curDataProcessThread = new QThread();
    curDataProcess->moveToThread (curDataProcessThread);

    connect (curDataProcessThread, SIGNAL(finished()),
             curDataProcess, SLOT(deleteLater()));

    connect (this, SIGNAL(sendstartProcessHistoryDataSignal(QString)),
             curDataProcess, SLOT(receiveOrigianlHistoryData(QString)));

    connect(curDataProcess, SIGNAL(sendAllData(QList<QList<double>>)),
            this, SLOT(receiveProcessedHistoryData (QList<QList<double>>)));

    curDataProcessThread->start ();
    m_dataProcessList.append (curDataProcess);
    m_dataProcessThreadList.append (curDataProcessThread);   

    emit sendstartProcessHistoryDataSignal("all");

    qDebug() << "startProcessHistoryData end!";
}

void ChartForm::receiveProcessedHistoryData (QList<QList<double>> allData) {
    qDebug() << "ChartForm::receiveProcessedHistoryData: " << QThread::currentThreadId();

    m_timeData = allData[0];
    m_strategyData = allData[1];
    m_votData = allData[2];
    QList<double> macdData = allData[3];
    for (int i = 0; i < macdData.size (); i += 5){
        m_macdData.append (MACD(macdData[i], macdData[i+1],
                                macdData[i+2], macdData[i+3], macdData[i+4]));
    }
    updateProgramInfo (m_programInfoTableView, QString("完成数据计算, 计算后数据总数: %1").arg(m_timeData.size () * 6));
    updateProgramInfo (m_programInfoTableView, QString("显示图像"));
    qDebug() << "timeData.size:     " << m_timeData.size ();
    qDebug() << "strategyData.size: " << m_strategyData.size ();
    qDebug() << "votData.size:      " << m_votData.size ();
    qDebug() << "macdData.size:     " << m_macdData.size ();
    releaseDataProcessSrc ();
    setLayout ();

    if (m_isRealTime) {
        if (m_macdData.size() > 0) {
            m_monitorWorker->setInitMacd(m_macdData[m_macdData.size()-1]);
        }
        m_monitorWorker->startTimer();
    }
}

void ChartForm::releaseDataReaderSrc () {
    qDebug() << "m_dataReaderThreadList.size(): " <<  m_dataReaderThreadList.size();
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         m_dataReaderThreadList[i]->quit ();
    }
}

void ChartForm::releaseDataProcessSrc () {
    qDebug() << "m_dataProcessThreadList.size(): " <<  m_dataProcessThreadList.size();
    for (int i = 0; i < m_dataProcessThreadList.size (); ++i) {
        m_dataProcessThreadList[i]->quit ();
    }
}

void ChartForm::setLayout () {
    ui->setupUi(this);
    this->setWindowTitle(m_strategyName);
    m_title = QString("策略: %1 , MACD: %2, %3, %4 ").arg(m_strategyName).arg(m_macdTime[0]).arg(m_macdTime[1]).arg (m_macdTime[2]);

    ui->Title_Label->setText(m_title);

    setStrategyChartView();
    setVotRunoverChartView ();
    setMACDChartView();
    setTheme();

    if (NULL != m_strategyChartView) {
        ui->gridLayout->addWidget (m_strategyChartView, 1, 0);
    }
    if (NULL != m_votrunoverChartView) {
        ui->gridLayout->addWidget (m_votrunoverChartView, 2, 0);
    }
    if (NULL != m_macdChartView) {
        ui->gridLayout->addWidget (m_macdChartView, 3, 0);
    }
    this->setMouseTracking(true);
    m_timeAxisUpdateData = m_timeData;
    m_isLayoutSetted = true;

    m_endTime = QTime::currentTime();
    int costSecs = m_endTime.msecsSinceStartOfDay() - m_startTime.msecsSinceStartOfDay();
    updateProgramInfo (m_programInfoTableView, QString("读取计算数据,绘制图像耗费: %1s, 线程: %2").arg(costSecs/1000).arg(m_threadNumb));
}

QCategoryAxis* ChartForm::getAxisX () {
    QCategoryAxis* axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(m_timeData.size (), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_timeData[xpos]);
        axisX->append (tmpDatetime.toString (m_timeTypeFormat), xpos);
    }
    return axisX;
}

void ChartForm::setStrategyChartView () {
    QCategoryAxis* axisX = getAxisX();
    QValueAxis *axisY = new QValueAxis;
    m_strategySeries = new QLineSeries;
    m_preSpreadSeries = new QLineSeries;

    for (int i = 0; i < m_strategyData.size (); ++i) {
        m_strategySeries->append (i, m_strategyData.at (i));
    }
    if (m_isRealTime) {
        ui->preCLoseSpreadValue_Label->setText(QString("\346\230\250\346\227\245\347\202\271\345\267\256: %1").arg(m_preSpread));
        for (int i = 0; i < m_strategyData.size (); ++i) {
            m_preSpreadSeries->append(i, m_preSpread);
        }
    }

    QPen red(Qt::red);
    red.setWidth(3);
    m_strategySeries->setPen(red);
    m_strategySeries->setName("点差");
    m_strategySeries->setUseOpenGL (true);
    m_preSpreadSeries->setName("昨日点差");
    m_preSpreadSeries->setUseOpenGL(true);

    QList<double>  axisYRange;
    if (m_isRealTime) {
        QList<double> allYdata = m_strategyData;
        allYdata.append(m_preSpread);
        axisYRange = getChartYvalueRange(allYdata);
    } else {
        axisYRange = getChartYvalueRange(m_strategyData);
    }

    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_strategyChart = new QChart();
    m_strategyChart->addSeries (m_strategySeries);
    m_strategyChart->addSeries(m_preSpreadSeries);
//    m_strategyChart->legend()->setAlignment(Qt::AlignTop);
    m_strategyChart->legend()->hide();
    m_strategyChart->setAnimationOptions(QChart::NoAnimation);

    m_strategyChartView = new QMyChartView(m_strategyChart);
    m_strategyChartView->setRenderHint(QPainter::Antialiasing);
    m_strategyChartView->installEventFilter(this);
    m_strategyChartView->setMouseTracking(true);

    m_strategyChart->addAxis (axisX, Qt::AlignBottom);
    m_strategySeries->attachAxis (axisX);
    m_preSpreadSeries->attachAxis(axisX);
    m_strategyChart->addAxis (axisY, Qt::AlignLeft);
    m_strategySeries->attachAxis (axisY);
    m_preSpreadSeries->attachAxis(axisY);
}

void ChartForm::setVotRunoverChartView () {
    QCategoryAxis* axisX = getAxisX();
    m_votBarSeries = new QStackedBarSeries();
    QBarSet *set = new QBarSet("\346\210\220\344\272\244\351\242\235:");
    for (int i = 0; i < m_votData.size (); ++i) {
        set->append(m_votData.at (i));
    }
    m_votBarSeries->append(set);
    m_votBarSeries->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getChartYvalueRange(m_votData);
    axisY -> setRange (0, axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_votrunoverChart = new QChart();
    m_votrunoverChart->addSeries(m_votBarSeries);
    m_votrunoverChart->setAnimationOptions(QChart::NoAnimation);

//    m_votrunoverChart->legend()->setAlignment(Qt::AlignTop);
    m_votrunoverChart->legend()->hide();

    m_votrunoverChartView = new QMyChartView(m_votrunoverChart, this);
    m_votrunoverChartView->setRenderHint(QPainter::Antialiasing);
    m_votrunoverChartView->installEventFilter(this);
    m_votrunoverChartView->setMouseTracking(true);

    m_votrunoverChart->addAxis (axisX, Qt::AlignBottom);
    m_votBarSeries->attachAxis(axisX);

    m_votrunoverChart->addAxis (axisY, Qt::AlignLeft);
    m_votBarSeries->attachAxis (axisY);
}

void ChartForm::setMACDChartView () {
    m_diffSeries = new QLineSeries();
    m_diffSeries->setName("DIFF");
    m_deaSeries = new QLineSeries();
    m_deaSeries->setName("DEA");
    m_macdSeries = new QStackedBarSeries();
    QBarSet *macdSet = new QBarSet("MACD");

    QCategoryAxis* axisX = getAxisX();
    for (int i = 0; i < m_macdData.size (); ++i) {
        m_diffSeries->append (i, m_macdData.at(i).m_diff);
        m_deaSeries->append (i, m_macdData.at(i).m_dea);
        macdSet->append (m_macdData.at(i).m_macd);
    }
    m_macdSeries->append(macdSet);
    m_diffSeries->setUseOpenGL (true);
    m_deaSeries->setUseOpenGL (true);
    m_macdSeries->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getMACDRange(m_macdData);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_macdChart = new QChart();
    m_macdChart->addSeries (m_diffSeries);
    m_macdChart->addSeries (m_deaSeries);
    m_macdChart->addSeries (m_macdSeries);
//    m_macdChart->legend()->setAlignment(Qt::AlignTop);
    m_macdChart->legend()->hide();
    m_macdChart->setAnimationOptions(QChart::NoAnimation);

    m_macdChartView = new QMyChartView(m_macdChart, this);
    m_macdChartView->setRenderHint(QPainter::Antialiasing);
    m_macdChartView->installEventFilter(this);
    m_macdChartView->setMouseTracking(true);

    m_macdChart->addAxis(axisX, Qt::AlignBottom);
    m_diffSeries->attachAxis(axisX);
    m_deaSeries->attachAxis (axisX);
    m_macdSeries->attachAxis (axisX);

    m_macdChart->addAxis(axisY, Qt::AlignLeft);
    m_diffSeries->attachAxis(axisY);
    m_deaSeries->attachAxis (axisY);
    m_macdSeries->attachAxis (axisY);

//    qDebug() << "m_macdChartView.size: "<< m_macdChartView->size();
}

void ChartForm::setTheme() {
    m_strategyChart->setTheme(QChart::ChartThemeDark);
    m_votrunoverChart->setTheme(QChart::ChartThemeDark);
    m_macdChart->setTheme(QChart::ChartThemeDark);
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0x121218));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    window()->setPalette(pal);
    ui->groupBox->setPalette(pal);
    ui->Title_Label->setStyleSheet(QStringLiteral("color: rgb(250, 250, 250);font: 75 14pt \"微软雅黑\";"));
}

void ChartForm::setMouseMoveValue(int currIndex) {
    if (currIndex >= 0 && currIndex < m_strategyData.size()) {
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(qint64(m_timeData.at(currIndex)));
        QString dateTimeString = curDatetime.toString (m_timeTypeFormat);
        qreal strategyValue = m_strategyData.at(currIndex);
        qreal votrunoverValue = m_votData.at(currIndex);
        qreal DIFF = m_macdData.at(currIndex).m_diff;
        qreal DEA = m_macdData.at(currIndex).m_dea;
        qreal Macd = m_macdData.at(currIndex).m_macd;

        QString strategyChineseString = "点差:";

        ui->TimeLabel->setText(QString("%1 %2").arg("时间: ").arg(dateTimeString));
        ui->StrategyValue_Label->setText(QString("%1 %2").arg(strategyChineseString).arg(strategyValue));
        ui->VotRunover_Label->setText(QString("%1 %2").arg("\346\210\220\344\272\244\351\242\235: ").arg(votrunoverValue));

        ui->DIFF_Label->setText(QString("DIFF: %1").arg(DIFF));
        ui->DEA_Label->setText(QString("DEA: %1").arg(DEA));
        ui->MACD_Label->setText(QString("MACD: %1").arg(Macd));
    }
}

bool ChartForm::eventFilter (QObject *watched, QEvent *event) {
    if (event->type () == QEvent::MouseMove) {
        mouseMoveEvenFunc(watched, event);
    }
    if (event->type() == QEvent::KeyRelease) {
        KeyReleaseFunc(event);
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        mouseButtonReleaseFunc(watched, event);
    }
    return QWidget::eventFilter (watched, event);
}

void ChartForm::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    int currIndex = -1;
    if (watched == m_strategyChartView) {
        QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue (curPoint);
//            qDebug() << "curStrategyChartChartPoint: " << curStrategyChartChartPoint;
        currIndex = qFloor(curStrategyChartChartPoint.x());
    }
    if (watched == m_votrunoverChartView) {
        QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue (curPoint);
//            qDebug() << "curVotrunoverChartChartPoint: " << curVotrunoverChartChartPoint;
        currIndex = qFloor(curVotrunoverChartChartPoint.x());
    }
    if (watched == m_macdChartView) {
        QPointF curMacdChartChartPoint = m_macdChart->mapToValue (curPoint);
//            qDebug() << "curMacdChartChartPoint: " << curMacdChartChartPoint;
        currIndex = qFloor(curMacdChartChartPoint.x());
    }

    if (m_setMouseTimeIndex) {
        m_currTimeIndex = currIndex;
        m_setMouseTimeIndex = false;
    }
//    qDebug() << "mouseMoveEvenFunc, currIndex: " << currIndex << ", currPoint: " << curPoint
//             << ", globalX: " << QCursor::pos().x();

    QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(qint64(m_timeData.at(currIndex)));
    QString dateTimeString = curDatetime.toString (m_timeTypeFormat);
//    qDebug() << "mouseMoveEvenFunc, currIndex: " << currIndex << ", curTime: " << dateTimeString;

    if (m_setKeyMove) {
        m_setKeyMove = false;
    }else {
        setMouseMoveValue(currIndex);
    }
}

double ChartForm::getPointXDistance() {
    int testIndex = 0;
    QPointF pointa = m_strategyChart->mapToPosition(QPointF(testIndex, m_strategyData[testIndex]));
    QPointF pointb = m_strategyChart->mapToPosition(QPointF(testIndex+1, m_strategyData[testIndex+1]));
    double distance = pointb.x() - pointa.x();
    if (m_oldPointDistance == -1) {
        m_oldPointDistance = distance;
    }
    return distance;
}

void ChartForm::KeyReleaseFunc(QEvent *event) {
    QKeyEvent* keyEvent = (QKeyEvent*)event;
    int step = 0;

    if (keyEvent->key() == Qt::Key_Left) {
        step = -1;
//        qDebug() << "key_left";
    }
    if (keyEvent->key() == Qt::Key_Right) {
        step = 1;
//        qDebug() << "Key_Right";
    }
    if (step != 0) {
        double pointXDistance = getPointXDistance();
        m_currTimeIndex += step;
//        qDebug() << "m_timeData.Size: " << m_timeData.size() << ", "
//                 << "m_currTimeIndex: " << m_currTimeIndex;
        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeData.size()) {
            m_keyMoveCount+=step;
            float move_distance = pointXDistance * m_keyMoveCount;

            QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(qint64(m_timeData.at(m_currTimeIndex)));
            QString dateTimeString = curDatetime.toString (m_timeTypeFormat);
            qDebug() << "pointXDistance: " << pointXDistance << ", "
                     << "m_currTimeIndex: " << m_currTimeIndex << ", "
                     << "curr_time: " << dateTimeString << ", "
                     << "m_timeData.Size: " << m_timeData.size() << ", "
                     << "m_keyMoveCount: " << m_keyMoveCount << ", "
                     << "move_distance: " << move_distance << ", "
                     << "mouse_xpos: " << m_mouseInitPos.x() + move_distance;

            if (move_distance >= 1 || move_distance <=-1 || move_distance == 0) {
                QCursor::setPos(m_mouseInitPos.x() + move_distance, m_mouseInitPos.y());
            }
            m_setKeyMove = true;
            setMouseMoveValue(m_currTimeIndex);
        }else {
            m_currTimeIndex -= step;
        }
    }
}

void ChartForm::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    QPointF transPoint;
    int currIndex = -1;
    m_mouseInitPos = QCursor::pos();
    m_keyMoveCount = 0;
    double deltaInGlobalPointAndChartPoint = 0;
    if (watched == m_strategyChartView) {
        QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue (curPoint);
        currIndex = qFloor(curStrategyChartChartPoint.x());
        transPoint = m_strategyChart->mapToPosition( (QPointF(currIndex, m_strategyData[currIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();

//        int transIndex =  qFloor((m_strategyChart->mapToValue (transPoint)).x());
//        qDebug() << "mouseButtonReleaseFunc: currIndex " << currIndex << ", currPoint: " << curPoint
//                 << ", transIndex" << transIndex << ", transPoint: " << transPoint
//                 << ", globalX: " << m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint;
//        qDebug() << "global point:  " << QCursor::pos();
//        qDebug() << "event point:   " << curPoint;
//        qDebug() << "transPoint:    " << transPoint;
    }
    if (watched == m_votrunoverChartView) {
        QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue (curPoint);
        currIndex = qFloor(curVotrunoverChartChartPoint.x());
        transPoint = m_votrunoverChart->mapToPosition( (QPointF(currIndex, m_votData[currIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (watched == m_macdChartView) {
        QPointF curMacdChartChartPoint = m_macdChart->mapToValue (curPoint);
        currIndex = qFloor(curMacdChartChartPoint.x());
        transPoint = m_macdChart->mapToPosition( (QPointF(currIndex, m_macdData[currIndex].m_macd)));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (currIndex >= 0 && currIndex < m_timeData.size()) {
        m_setMouseTimeIndex = true;
//        m_currTimeIndex = currIndex;
//        qDebug() << "transPoint: " << transPoint << ", curPoint: " << curPoint;
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
//        qDebug() <<"m_mouseInitPos:  " << m_mouseInitPos;
    } else {
        m_currTimeIndex = -1;
    }
//    qDebug() << "mouseButtonReleaseFunc currIndex: " << currIndex;
}

void ChartForm::closeEvent(QCloseEvent *event) {
    event;
    if (!m_isclosed) {
        qDebug() << "--- Close -- ";
        if (m_isRealTime) {
            m_timer.stop();
            if (NULL != m_monitorWorker) {
                qDebug() << "m_monitorWorker->stopTimer() ";
                m_monitorWorker->stopTimer();
            }            
        }
//        emit sendCloseSignal(m_chartViewID);
        m_isclosed = true;
    }
}

ChartForm::~ChartForm()
{
//    qDebug() << "~ChartForm " << 0;
    delete ui;
    m_MonitorThread.quit();
    m_MonitorThread.wait();
    if (NULL != m_macdTooltip) {
        delete m_macdTooltip;
        m_macdTooltip = NULL;
    }
    if (NULL != m_strategyTooltip) {
        delete m_strategyTooltip;
        m_strategyTooltip = NULL;
    }
    if (NULL != m_votrunoverTooltip) {
        delete m_votrunoverTooltip;
        m_votrunoverTooltip = NULL;
    }
    for (int i = 0; i < m_dataProcessThreadList.size(); ++i) {
         delete m_dataProcessThreadList[i];
         m_dataProcessThreadList[i] = NULL;
    }
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         delete m_dataReaderThreadList[i];
         m_dataReaderThreadList[i] = NULL;
    }
    if (NULL != m_monitorWorker) {
        m_monitorWorker = NULL;
    }
//    qDebug() << "~ChartForm " << 1;
}

//void ChartForm::initMonitorTimer() {
//    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkRealTimeData()));
//    m_timer.start(m_updateTime);
//}

//void ChartForm::initIndexHedgeMetaInfo() {
//    m_indexHedgeMetaInfo.insert("000300.SH", 300);
//    m_indexHedgeMetaInfo.insert("000016.SH", 50);
//    m_indexHedgeMetaInfo.insert("000852.SH", 1000);
//    m_indexHedgeMetaInfo.insert("000904.SH", 200);
//    m_indexHedgeMetaInfo.insert("000905.SH", 500);
//    m_indexHedgeMetaInfo.insert("000906.SH", 800);
//    m_indexHedgeMetaInfo.insert("399903.SZ", 100);
//}

//void ChartForm::checkRealTimeData() {
//    bool dataChange = false;
//    QList<QString> global_secodeList = g_wsqData.keys();
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//       QString secode = m_secodeNameList[i];
//       if (global_secodeList.indexOf(secode) >= 0) {
//           QList<QStringList> currData = g_wsqData[secode];
//           QStringList latestData = currData[currData.size()-1];
//           if (m_realTimeData[secode].size() == 0
//                   || latestData[1].toDouble() > m_realTimeData[secode][1].toDouble()) {
//               m_realTimeData[secode] = latestData;
//               m_realTimeData[secode][4] = QString("%1").arg(0);
//               if (currData.size() == 2) {
//                   m_realTimeData[secode][4] = QString("%1").arg(g_wsqData[secode][1][4].toDouble()
//                           - g_wsqData[secode][0][4].toDouble());
//               }
//               dataChange = true;
//           }
//       } else {
//           dataChange = false;
//           break;
//       }
//    }
//    if(dataChange) {
//        updateData();
//        updateChart();
//        dataChange = false;
//    }
//}

//void ChartForm::updateData() {
//    double strategyData = 0;
//    double votData = 0;
//    double timeData = QDateTime::currentDateTime().toMSecsSinceEpoch();
//    MACD macdData;
//    for (int i = 0; i < m_secodeNameList.size(); ++i) {
//        QString secode = m_secodeNameList[i];
//        if (secode == m_hedgeIndexCode) {
//            continue;
//        }
//        if (m_realTimeData[secode][2] == "0.0000") {
//            strategyData += m_realTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
//        } else {
//            strategyData += m_realTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
//        }
//        votData += m_realTimeData[secode][4].toDouble();
//    }

//    strategyData = strategyData / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
//                   - m_realTimeData[m_hedgeIndexCode][2].toDouble();

//    if (m_macdData.size() > 0) {
//        MACD latestData = m_macdData[m_macdData.size()-1];
//        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
//    } else {
//        macdData = MACD(strategyData, strategyData, 0, 0, 0);
//    }
//    m_strategyData.append(strategyData);
//    m_votData.append(votData);
//    m_timeData.append(timeData);
//    m_macdData.append(macdData);
//}

//void ChartForm::setTestView () {
//    int dataCount = 10;
//    QCategoryAxis* axisX = new QCategoryAxis;
//    QLineSeries* series = new QLineSeries;
//    for (int i = 0; i < dataCount; ++i) {
//        qreal xPos = i;
//        series->append (xPos, i);
//        QDateTime tmpDatetime = QDateTime(QDate(2017,11,i), QTime(12, 12, 12));
//        axisX->append (tmpDatetime.toString ("yyyy-MM-dd h:mm"), xPos);
//    }

//    QValueAxis *axisY = new QValueAxis;
//    QList<double>  axisYRange = getChartYvalueRange(series->points());
//    axisY -> setRange (axisYRange[0], axisYRange[1]);

//    m_testChart = new QChart();
//    m_testChart->addSeries (series);
//    m_testChart->setTitle("Test Category!");

//    m_testChartView = new QChartView(m_testChart);
//    m_testChartView->setRenderHint(QPainter::Antialiasing);
//    m_testChartView->installEventFilter(this);
//    m_testChartView->setMouseTracking(true);

//    m_testChart->addAxis (axisX, Qt::AlignBottom);
//    series->attachAxis (axisX);
//    m_testChart->addAxis (axisY, Qt::AlignLeft);
//    series->attachAxis (axisY);

//}
