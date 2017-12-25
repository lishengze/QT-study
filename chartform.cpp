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
    m_macdTime(macdTime), m_readDataThreadCount(0), m_threadNumb(threadNumb),
    m_macdTooltip(NULL), m_strategyTooltip(NULL), m_votrunoverTooltip(NULL),
    m_isRealTime(false), m_isclosed(false),
    m_keyMoveCount(0), m_mouseInitPos(-1, -1),
    ui(new Ui::ChartForm)
{
    registSignalParamsType();
    initData(databaseName, timeType, strategyList);
    startReadData ();
}

ChartForm::ChartForm(QWidget *parent, RealTimeDataRead* readRealTimeData,
                     QTableView* programInfoTableView, int chartViewID,
                     QList<strategy_ceil> strategyList, QString strategyName,
                     QString hedgeIndexCode, int hedgeIndexCount,
                     int updateTime, QList<int> macdTime, bool isTestRealTime):
    QWidget(parent), m_readRealTimeData(readRealTimeData),
    m_programInfoTableView(programInfoTableView), m_chartViewID(chartViewID),
    m_strategy(strategyList), m_strategyName(strategyName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_updateTime(updateTime), m_macdTime(macdTime), m_bTestRealTime(isTestRealTime),
    m_macdTooltip(NULL), m_strategyTooltip(NULL), m_votrunoverTooltip(NULL),
    m_isRealTime(true), m_isclosed(false), m_OldStrategySpread(6),
    m_keyMoveCount(0), m_mouseInitPos(-1, -1), m_oldPointDistance(-1),
    ui(new Ui::ChartForm)
{
    registSignalParamsType();
    initRealTimeData();
    setLayout();
}

void ChartForm::registSignalParamsType () {
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
    qRegisterMetaType<QMap<QString,QStringList>>("QMap<QString,QStringList>");
}

void ChartForm::initData (QString databaseName, QString timeType, QList<strategy_ceil> strategyList) {
//    this->setWindowTitle(m_strategyName);
    m_dbhost = "192.168.211.165";
//    m_dbhost = "localhost";
    m_databaseName = databaseName + "_" + timeType;
    m_keyValueList << "TCLOSE" << "VOTRUNOVER";

    for (int i = 0; i < strategyList.size (); ++i) {
        m_seocdebuyCountMap.insert (strategyList[i].m_secode, strategyList[i].m_buyCount);
    }
    m_seocdebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
    m_secodeNameList = m_seocdebuyCountMap.keys();

    if (m_timeType.contains("m") && m_timeType != "month") {
        m_timeTypeFormat = "yyyy-MM-dd h:mm";
    } else {
        m_timeTypeFormat = "yyyy-MM-dd";
    }
    m_chartXaxisTickCount = 10;
}

void ChartForm::initRealTimeData() {
//    this->setWindowTitle(m_strategyName);

    QList<QString> oriSecodeList;
    for (int i = 0; i < m_strategy.size (); ++i) {
        m_seocdebuyCountMap.insert (getWindSecode(m_strategy[i].m_secode), m_strategy[i].m_buyCount);
        oriSecodeList.append(m_strategy[i].m_secode);
    }
    oriSecodeList.append(m_hedgeIndexCode);
    m_hedgeIndexCode = getWindSecode(m_hedgeIndexCode);
    m_seocdebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
    m_secodeNameList = m_seocdebuyCountMap.keys();

    m_timeTypeFormat = "yyyy-MM-dd hh:mm:ss";
    m_chartXaxisTickCount = 5;

    connect(this, SIGNAL(getPreData(QList<QString>)),
            m_readRealTimeData, SLOT(getPreData(QList<QString>)));

    connect(m_readRealTimeData, SIGNAL(sendPreData(QMap<QString,QStringList>)),
            this, SLOT(receivePreData(QMap<QString,QStringList>)));

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkRealTimeData()));

    m_timer.start(m_updateTime);

    m_indexHedgeMetaInfo.insert("000300.SH", 300);
    m_indexHedgeMetaInfo.insert("000016.SH", 50);
    m_indexHedgeMetaInfo.insert("000852.SH", 1000);
    m_indexHedgeMetaInfo.insert("000904.SH", 200);
    m_indexHedgeMetaInfo.insert("000905.SH", 500);
    m_indexHedgeMetaInfo.insert("000906.SH", 800);
    m_indexHedgeMetaInfo.insert("399903.SZ", 100);

    if (!m_bTestRealTime) {
        emit getPreData(oriSecodeList);
    }
}

void ChartForm::receivePreData(QMap<QString, QStringList> preCLose) {
//    m_OldStrategySpread = spread;
    double result = 0;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (secode != m_hedgeIndexCode) {
            result += preCLose[secode][0].toDouble() * m_seocdebuyCountMap[secode];
        }
    }
    result = result / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode]) - preCLose[m_hedgeIndexCode][0].toDouble();
    m_OldStrategySpread = result;
    ui->preCLoseSpreadValue_Label->setText(QString("\346\230\250\346\227\245\347\202\271\345\267\256: %1").arg(m_OldStrategySpread));
    qDebug() << "m_OldStrategySpread: " << m_OldStrategySpread;

    disconnect(m_readRealTimeData, SIGNAL(sendPreData(QMap<QString,QStringList>)),
            this, SLOT(receivePreData(QMap<QString,QStringList>)));
}

void ChartForm::checkRealTimeData() {
    bool dataChange = false;
    QList<QString> global_secodeList = g_wsqData.keys();
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
       QString secode = m_secodeNameList[i];
       if (global_secodeList.indexOf(secode) >= 0) {
           QList<QStringList> currData = g_wsqData[secode];
           QStringList latestData = currData[currData.size()-1];
           if (m_realTimeData[secode].size() == 0
                   || latestData[1].toDouble() > m_realTimeData[secode][1].toDouble()) {
               m_realTimeData[secode] = latestData;
               if (currData.size() == 2) {
                   m_realTimeData[secode][4] = QString("%1").arg(g_wsqData[secode][1][4].toDouble() - g_wsqData[secode][0][4].toDouble());
               }
               dataChange = true;
           }

//           if (m_realTimeData[secode].size() == 0) {
//                m_realTimeData[secode] = latestData;
//                dataChange = true;
//            } else if (latestData[1].toDouble() > m_realTimeData[secode][1].toDouble()) {
//               double amt = latestData[4].toDouble() - m_realTimeData[secode][4].toDouble();
//               m_realTimeData[secode] = latestData;
//               m_realTimeData[secode][4] = amt;
//               dataChange = true;
//           }

       } else {
           dataChange = false;
           break;
       }
    }
    if(dataChange) {
        updateData();
        updateChart();
        dataChange = false;
    }
}

void ChartForm::updateData() {
    double strategyData = 0;
    double votData = 0;
    double timeData = QDateTime::currentDateTime().toMSecsSinceEpoch();
    MACD macdData;
    for (int i = 0; i < m_secodeNameList.size(); ++i) {
        QString secode = m_secodeNameList[i];
        if (secode == m_hedgeIndexCode) {
//            qDebug() << "m_hedgeIndexCode: " << m_hedgeIndexCode;
            continue;
        }
        if (m_realTimeData[secode][2] == "") {
            strategyData += m_realTimeData[secode][3].toDouble() * m_seocdebuyCountMap[secode];
        } else {
            strategyData += m_realTimeData[secode][2].toDouble() * m_seocdebuyCountMap[secode];
        }
        votData += m_realTimeData[secode][4].toDouble();
    }

//    qDebug () << "m_hedgeIndexCount: " << m_hedgeIndexCount
    //<< "  m_indexHedgeMetaInfo[m_hedgeIndexCode]: " << m_indexHedgeMetaInfo[m_hedgeIndexCode];
    strategyData = strategyData / (m_hedgeIndexCount * m_indexHedgeMetaInfo[m_hedgeIndexCode])
                   - m_realTimeData[m_hedgeIndexCode][2].toDouble();

    if (m_macdData.size() > 0) {
        MACD latestData = m_macdData[m_macdData.size()-1];
        macdData = computeMACDData(strategyData, latestData, m_macdTime[0], m_macdTime[1], m_macdTime[2]);
    } else {
        macdData = MACD(strategyData, strategyData, 0, 0, 0);
    }
    m_strategyData.append(strategyData);
    m_votData.append(votData);
    m_timeData.append(timeData);
    m_macdData.append(macdData);

    if (m_votData.size() ==1) {
        m_votData[0] = 0;
    } else {
        m_votData[0] = m_votData[1];
    }
}

void ChartForm::updateChart() {
    updateAxis();
    updateSeries();
    updateMousePos();
}

void ChartForm::updateAxis() {
    int curPointNumb = m_timeData.size();
//    qDebug() << "curPointNumb: " << curPointNumb;
    QValueAxis *strategyAxisY = dynamic_cast<QValueAxis *>(m_strategyChart->axisY());
    QList<double> allStrategyData = m_strategyData;
    if (m_bTestRealTime) {
        m_OldStrategySpread = getAveValue(m_strategyData);
    }
    allStrategyData.append(m_OldStrategySpread);
    QList<double>  strategyAxisYRange = getChartYvalueRange(allStrategyData);
    strategyAxisY -> setRange (strategyAxisYRange[0], strategyAxisYRange[1]);

    QValueAxis *votAxisY = dynamic_cast<QValueAxis *>(m_votrunoverChart->axisY());
    QList<double>  votAxisYRange = getChartYvalueRange(m_votData);
    votAxisY -> setRange (votAxisYRange[0], votAxisYRange[1]);

    QValueAxis *macdAxisY = dynamic_cast<QValueAxis *>(m_macdChart->axisY());
    QList<double>  macdAxisYRange = getMACDRange(m_macdData);
    macdAxisY -> setRange (macdAxisYRange[0], macdAxisYRange[1]);

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
    } else {
        m_strategyChart->removeAxis(strategyAxisX);
        QCategoryAxis* newStrategyAxisX = getAxisX();
        newStrategyAxisX->setMax(curPointNumb-1);
        m_strategyChart->addAxis (newStrategyAxisX, Qt::AlignBottom);
        m_strategySeries->attachAxis (newStrategyAxisX);
        m_oldStrategySpreadSeries->attachAxis(newStrategyAxisX);

        m_votrunoverChart->removeAxis(votAxisX);
        QCategoryAxis* newVotAxisX = getAxisX();
        newVotAxisX->setMax(curPointNumb-1);
        m_votrunoverChart->addAxis (newVotAxisX, Qt::AlignBottom);
        m_votBarSeries->attachAxis (newVotAxisX);

        m_macdChart->removeAxis(macdAxisX);
        QCategoryAxis* newMacdAxisX = getAxisX();
        newMacdAxisX->setMax(curPointNumb-1);
        m_macdChart->addAxis (newMacdAxisX, Qt::AlignBottom);
        m_diffSeries->attachAxis (newMacdAxisX);
        m_deaSeries->attachAxis (newMacdAxisX);
        m_macdSeries->attachAxis (newMacdAxisX);
    }
}

void ChartForm::updateSeries() {
    int curPointNumb = m_timeData.size();
    m_strategySeries->append(curPointNumb-1, m_strategyData[curPointNumb-1]);
    m_oldStrategySpreadSeries->append(curPointNumb-1, m_OldStrategySpread);

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

    m_strategyChartView->setMinimumHeight(strategyHeight);
    m_votrunoverChartView->setMaximumHeight(votHeight);
    m_macdChartView->setMaximumHeight(macdHeight);
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
    QList<QStringList> oriThreadData = allocateThreadData ();
//    qDebug() << "oriThreadData: " << oriThreadData;
    for (int i = 0; i < oriThreadData.size (); ++i) {
        DataRead* curDataReader = new DataRead(QString("%1").arg(i + m_chartViewID * m_threadNumb), m_dbhost, m_databaseName,
                                               oriThreadData[i], m_startDate, m_endDate, m_keyValueList);
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
    emit sendStartReadDataSignal("HistoryData");
}

void testCLosePrice(QMap<QString, QList<QStringList>> oriData) {
    QList<QString> secodeList = oriData.keys();
    for (int i = 0; i < secodeList.size(); ++i) {
        QString secode = secodeList[i];
        QList<QStringList> currData = oriData[secode];
        if (currData.size() == 0) {
            qDebug() << "secode : " << secode << " has no data";
        } else {
            qDebug() << secode << ", TCLOSE: " << currData[currData.size()-1];
        }
    }
}

void ChartForm::receiveOriginalData (QMap<QString, QList<QStringList>> subThreadData) {
    QMutexLocker locker(&m_mutex);
    m_completeTableData.unite (subThreadData);
    ++m_readDataThreadCount;
    qDebug() << "ChartForm::receiveOriginalData: " << QThread::currentThreadId()
             << " dataNumb: " << subThreadData.size ();
    qDebug() << "m_readDataThreadCount: " << m_readDataThreadCount << "\n";

    updateProgramInfo (m_programInfoTableView, QString("完成股票指数读取数目: %1").arg(m_completeTableData.size ()));
    if (m_readDataThreadCount == m_threadNumb) {
//        testCLosePrice(m_completeTableData);
        releaseDataReaderSrc ();
        m_readDataThreadCount = 0;
        startProcessData();
        m_completeTableData.clear ();
    }
}

void ChartForm::startProcessData () {
    int oridataCount = 0;
    for (int i = 0; i < m_secodeNameList.size (); ++i) {
        oridataCount += m_completeTableData[m_secodeNameList[i]].size ();
    }
    updateProgramInfo (m_programInfoTableView, QString("原始数据获取完成, 原始数据总数: %1").arg(oridataCount));
    updateProgramInfo (m_programInfoTableView, QString("计算数据"));
    DataProcess* curDataProcess = new DataProcess(m_completeTableData, m_seocdebuyCountMap, m_macdTime);
    QThread* curDataProcessThread = new QThread();
    curDataProcess->moveToThread (curDataProcessThread);

    connect (curDataProcessThread, SIGNAL(finished()),
             curDataProcess, SLOT(deleteLater()));

    connect (this, SIGNAL(sendStartProcessDataSignal(QString)),
             curDataProcess, SLOT(receiveStartProcessData(QString)));

    connect(curDataProcess, SIGNAL(sendAllData(QList<QList<double>>)),
            this, SLOT(receiveAllProcessedData (QList<QList<double>>)));

    curDataProcessThread->start ();
    m_dataProcessList.append (curDataProcess);
    m_dataProcessThreadList.append (curDataProcessThread);   
    emit sendStartProcessDataSignal("all");
}

void ChartForm::receiveAllProcessedData (QList<QList<double>> allData) {
    qDebug() << "ChartForm::receiveAllProcessedData: " << QThread::currentThreadId();
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
}

void ChartForm::releaseDataReaderSrc () {
    qDebug() << " m_dataReaderThreadList.size(): " <<  m_dataReaderThreadList.size();
    for (int i = 0; i < m_dataReaderThreadList.size(); ++i) {
         m_dataReaderThreadList[i]->quit ();
    }
}

void ChartForm::releaseDataProcessSrc () {
    qDebug() << " m_dataProcessThreadList.size(): " <<  m_dataProcessThreadList.size();
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
    m_oldStrategySpreadSeries = new QLineSeries;
    for (int i = 0; i < m_strategyData.size (); ++i) {
        m_strategySeries->append (i, m_strategyData.at (i));
    }
    QPen red(Qt::red);
    red.setWidth(3);
    m_strategySeries->setPen(red);
    m_strategySeries->setName("点差");
    m_strategySeries->setUseOpenGL (true);
    m_oldStrategySpreadSeries->setName("昨日点差");
    m_oldStrategySpreadSeries->setUseOpenGL(true);

    QList<double>  axisYRange = getChartYvalueRange(m_strategyData);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_strategyChart = new QChart();
    m_strategyChart->addSeries (m_strategySeries);
    m_strategyChart->addSeries(m_oldStrategySpreadSeries);
    m_strategyChart->legend()->setAlignment(Qt::AlignTop);

    m_strategyChartView = new QMyChartView(m_strategyChart);
    m_strategyChartView->setRenderHint(QPainter::Antialiasing);
    m_strategyChartView->installEventFilter(this);
    m_strategyChartView->setMouseTracking(true);

    m_strategyChart->addAxis (axisX, Qt::AlignBottom);
    m_strategySeries->attachAxis (axisX);
    m_oldStrategySpreadSeries->attachAxis(axisX);
    m_strategyChart->addAxis (axisY, Qt::AlignLeft);
    m_strategySeries->attachAxis (axisY);
    m_oldStrategySpreadSeries->attachAxis(axisY);

//    qDebug() << "strategy.size: "<< m_strategyChartView->size();
}

void ChartForm::setVotRunoverChartView () {
//    if (m_votData.size () == 0) {
//        QMessageBox::warning (this, "Waring", "成交额曲线在所选时间范围内没有数据");
//        return;
//    }

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

    m_votrunoverChart->legend()->setAlignment(Qt::AlignTop);

    m_votrunoverChartView = new QMyChartView(m_votrunoverChart, this);
    m_votrunoverChartView->setRenderHint(QPainter::Antialiasing);
    m_votrunoverChartView->installEventFilter(this);
    m_votrunoverChartView->setMouseTracking(true);

    m_votrunoverChart->addAxis (axisX, Qt::AlignBottom);
    m_votBarSeries->attachAxis(axisX);

    m_votrunoverChart->addAxis (axisY, Qt::AlignLeft);
    m_votBarSeries->attachAxis (axisY);

//    qDebug() << "votrunoverChartView.size: "<< m_votrunoverChartView->size();
}

void ChartForm::setMACDChartView () {
//    if (m_macdData.size () == 0) {
//         QMessageBox::warning (this, "Waring", "MACD曲线在所选时间范围内没有数据");
//        return;
//    }

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
    m_macdChart->legend()->setAlignment(Qt::AlignTop);

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

void ChartForm::setTestView () {
    int dataCount = 10;
    QCategoryAxis* axisX = new QCategoryAxis;
    QLineSeries* series = new QLineSeries;
    for (int i = 0; i < dataCount; ++i) {
        qreal xPos = i;
        series->append (xPos, i);
        QDateTime tmpDatetime = QDateTime(QDate(2017,11,i), QTime(12, 12, 12));
        axisX->append (tmpDatetime.toString ("yyyy-MM-dd h:mm"), xPos);
    }

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getChartYvalueRange(series->points());
    axisY -> setRange (axisYRange[0], axisYRange[1]);

    m_testChart = new QChart();
    m_testChart->addSeries (series);
    m_testChart->setTitle("Test Category!");

    m_testChartView = new QChartView(m_testChart);
    m_testChartView->setRenderHint(QPainter::Antialiasing);
    m_testChartView->installEventFilter(this);
    m_testChartView->setMouseTracking(true);

    m_testChart->addAxis (axisX, Qt::AlignBottom);
    series->attachAxis (axisX);
    m_testChart->addAxis (axisY, Qt::AlignLeft);
    series->attachAxis (axisY);

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
        KeyReleaseFunc(watched, event);
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        mouseButtonReleaseFunc(watched, event);
    }
    return QWidget::eventFilter (watched, event);
}

void ChartForm::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
//        qDebug() << "movePoint: " << curPoint;
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
//        qDebug() << "currIndex: " << currIndex;
    setMouseMoveValue(currIndex+1);
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

void ChartForm::KeyReleaseFunc(QObject *watched, QEvent *event) {
    QKeyEvent* keyEvent = (QKeyEvent*)event;
    int step = 0;

    if (keyEvent->key() == Qt::Key_Left) {
        step = -1;
        qDebug() << "key_left";
    }
    if (keyEvent->key() == Qt::Key_Right) {
        step = 1;
        qDebug() << "Key_Right";
    }
    if (step != 0) {
        double pointXDistance = getPointXDistance();
//        QPoint curPoint = QCursor::pos();
        m_currTimeIndex += step;
        qDebug() << "m_timeData.Size: " << m_timeData.size()
                 << "    m_currTimeIndex: " << m_currTimeIndex;
        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeData.size()) {
            setMouseMoveValue(m_currTimeIndex);
            m_keyMoveCount+=step;
            qDebug() <<"pointXDistance: " << pointXDistance
                    << "    m_currTimeIndex: " << m_currTimeIndex
                     <<"    m_keyMoveCount: " << m_keyMoveCount;
            if (abs(pointXDistance*m_keyMoveCount) > 1 || m_keyMoveCount == 0) {
                QCursor::setPos(m_mouseInitPos.x() + m_keyMoveCount*pointXDistance, m_mouseInitPos.y());
            }
//            setMouseMoveValue(m_currTimeIndex);
        }else {
            m_currTimeIndex -= step;
        }
    }
}

void ChartForm::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    m_keyMoveCount = 0;
    int currIndex = -1;
    m_mouseInitPos = QCursor::pos();
    double deltaInGlobalPointAndChartPoint = 0;
    if (watched == m_strategyChartView) {
        QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue (curPoint);
        currIndex = qFloor(curStrategyChartChartPoint.x());
        QPointF transPoint = m_strategyChart->mapToPosition( (QPointF(currIndex, m_strategyData[currIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
        qDebug() << "global point:  " << QCursor::pos();
        qDebug() << "this pos:      " << this->pos();
        qDebug() << "chartView pos: " << m_strategyChartView->pos();
        qDebug() << "event point:   " << curPoint;
        qDebug() << "transPoint:    " << transPoint;
    }
    if (watched == m_votrunoverChartView) {
        QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue (curPoint);
        currIndex = qFloor(curVotrunoverChartChartPoint.x());
        QPointF transPoint = m_votrunoverChart->mapToPosition( (QPointF(currIndex, m_votData[currIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (watched == m_macdChartView) {
        QPointF curMacdChartChartPoint = m_macdChart->mapToValue (curPoint);
        currIndex = qFloor(curMacdChartChartPoint.x());
        QPointF transPoint = m_macdChart->mapToPosition( (QPointF(currIndex, m_macdData[currIndex].m_macd)));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (currIndex >= 0 && currIndex < m_timeData.size()) {
        m_currTimeIndex = currIndex;
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
        qDebug() <<"m_mouseInitPos:  " << m_mouseInitPos;
    } else {
        m_currTimeIndex = -1;
    }
    qDebug() << "currIndex: " << currIndex;
}

void ChartForm::closeEvent(QCloseEvent *event) {
    if (!m_isclosed) {
        qDebug() << "--- Close -- ";
        if (m_isRealTime) {
            m_timer.stop();
            emit sendCloseSignal(m_chartViewID);
        }
        m_isclosed = true;
    }
}

ChartForm::~ChartForm()
{
    delete ui;
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
}
