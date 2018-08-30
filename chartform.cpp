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
#include <QMenu>
#include <QColor>

#include "chartform.h"
#include "ui_chartform.h"
#include "toolfunc.h"
#include "macd.h"
#include "callout.h"
#include "time_func.h"
#include "secode_func.h"
#include "widget_func.h"
#include "io_func.h"
#include "excel_func.h"
#include "process_data_func.h"
#pragma execution_character_set("utf-8")

ChartForm::ChartForm(QWidget *parent, QTableView* programInfoTableView,
                     int chartViewID, QString dbhost, bool isBuySalePortfolio,
                     QString hedgeIndexCode, int hedgeIndexCount, QList<int> macdTime,
                     QMap<QString, int> strategyMap, QString strategyName,
                     QMap<QString, int> buyStrategyMap, QMap<QString, int> saleStrategyMap,
                     bool isRealTime, QString startDate, QString endDate, QString timeType):
    BaseChart(chartViewID, parent), m_programInfoTableView(programInfoTableView),
    m_chartViewID(chartViewID), m_dbhost(dbhost), m_isBuySalePortfolio(isBuySalePortfolio),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount), m_macdTime(macdTime),
    m_secodebuyCountMap(strategyMap), m_strategyName(strategyName),
    m_buyStrategyMap(buyStrategyMap), m_saleStrategyMap(saleStrategyMap),
    m_isRealTime(isRealTime), m_startDate(startDate), m_endDate(endDate), m_timeType(timeType),
    ui(new Ui::ChartForm)
{
    initCommonData();
    if (m_isRealTime) {
        initRealTimeData();
    } else {
        initHistoryData();
    }
}

ChartForm::~ChartForm()
{
    delete ui;
    m_MonitorThread.quit();
    m_MonitorThread.wait();

    m_histdataThread.quit();
    m_histdataThread.wait();

    if (NULL != m_monitorWorker) {
        m_monitorWorker = NULL;
    }

    if (NULL != m_histdataWorker) {
        m_histdataWorker = NULL;
    }

//    for (int i = 0; i < m_extractWindowList.size(); ++i) {
//        if (m_extractWindowList[i] != NULL) {
//            delete m_m_extractWindowList[i];
//            m_extractWindowList[i] = NULL;
//        }
//    }
}

void ChartForm::initCommonData() {
    m_startTime = QTime::currentTime();   
    updateProgramInfo(m_programInfoTableView, QString("链接的数据库: %1").arg(m_dbhost));

    m_keyMoveCount = 0;
    m_currTimeIndex = 0;
    m_isKeyMove = false;
    m_mouseInitPos = QPoint(-1, -1);
    m_oldPointDistance = -1;
    m_setMouseTimeIndex = false;
    m_timeAxisUpdatePercent = 0.2;

    m_monitorWorker = NULL;
    m_histdataWorker = NULL;
    m_preSpread = 0;
    m_isclosed = false;
    m_isLayoutSetted = false;

    m_oldLabelIndex = 0;

    initSecodeStyle();
    initSecodeList();
    registSignalParamsType();
    initExtractKeyValueList();
}

void ChartForm::initSecodeList() {
    if (m_isBuySalePortfolio) {
        m_secodebuyCountMap.unite(m_buyStrategyMap);
        m_secodebuyCountMap.unite(m_saleStrategyMap);
        m_buyStrategyMap = transStrategyMap(m_buyStrategyMap, m_secodeStyle);
        m_saleStrategyMap = transStrategyMap(m_saleStrategyMap, m_secodeStyle);
        qDebug() << "m_buyStrategyMap.size: " << m_buyStrategyMap.size() << ", "
                 << "m_saleStrategyMap.size: " << m_saleStrategyMap.size() << ", "
                 << "m_secodebuyCountMap.size: " << m_secodebuyCountMap.size();
    }
    m_secodebuyCountMap = transStrategyMap(m_secodebuyCountMap, m_secodeStyle);

    if (m_hedgeIndexCount != -1) {
        m_hedgeIndexCode = getCompleteIndexCode(m_hedgeIndexCode, m_secodeStyle);
        m_secodebuyCountMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
    }

    m_secodeNameList = m_secodebuyCountMap.keys();
//    qDebug() << "m_secodeNameList: " << m_secodeNameList;
}

void ChartForm::initSecodeStyle() {
    if (m_isRealTime) {
        m_secodeStyle = "wind";
    } else {
        m_secodeStyle = "tinysoft";
    }
}

void ChartForm::registSignalParamsType () {
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
    qRegisterMetaType<QMap<QString,QStringList>>("QMap<QString,QStringList>");
    qRegisterMetaType<ChartData>("ChartData");

    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QList<QString>>("QMap<QString, double>");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
}

void ChartForm::initExtractKeyValueList() {
    m_extractKeyValueList.append(QString("对冲点差值"));
    m_extractKeyValueList.append(QString("DIFF"));
    m_extractKeyValueList.append(QString("DEA"));
    m_extractKeyValueList.append(QString("MACD"));
    if (m_isBuySalePortfolio==false) {
        m_extractKeyValueList.append(QString("沪深300指数"));
        m_extractKeyValueList.append(QString("成交额"));
    }
}

QList<QMyChartView*> ChartForm::getChartViewList() {
    QList<QMyChartView*>  result;
    result.append(m_strategyChartView);
    result.append(m_votrunoverChartView);
    result.append(m_macdChartView);
    return result;
}

void ChartForm::initHistoryData () {
    m_databaseName = "MarketData_" + m_timeType;
    m_keyValueList << "TCLOSE" << "VOTRUNOVER";

    m_threadNumb = m_secodeNameList.size();
    if (m_timeType.contains("m") && m_timeType != "month") {
        m_timeTypeFormat = "yyyy-MM-dd h:mm";
    } else {
        m_timeTypeFormat = "yyyy-MM-dd";
    }
    m_chartXaxisTickCount = 10;

    initHistdataThread();

    emit getHistDataSignal();
    qDebug() << "ChartForm::initHistoryData: " << QThread::currentThreadId();
}

void ChartForm::initRealTimeData() {
    m_databaseName = "MarketData_RealTime";
    m_updateTime = 3000;
    m_threadNumb = m_secodeNameList.size();
    m_timeTypeFormat = "hh:mm:ss";
    m_chartXaxisTickCount = 5;

    initMonitorThread();
    emit getPreCloseSpread();

    if (isTradingStart()) {
        initHistdataThread();
//        emit getHistDataSignal();
    } else {
        initLayout();
        m_monitorWorker->startTimer();
    }
}

void ChartForm::initHistdataThread() {
    m_histdataWorker = new HistoryData(m_chartViewID, m_isBuySalePortfolio,
                                       m_dbhost, m_databaseName,
                                       m_isRealTime, m_threadNumb, m_secodeNameList,
                                       m_macdTime, m_hedgeIndexCode,
                                       m_startDate, m_endDate, m_keyValueList,
                                       m_secodebuyCountMap, m_buyStrategyMap, m_saleStrategyMap);

    connect(&m_histdataThread, SIGNAL(finished()),
             m_histdataWorker, SLOT(deleteLater()));

    connect(this, SIGNAL(getHistDataSignal()),
            m_histdataWorker, SLOT(getHistData()));

    connect(m_histdataWorker, SIGNAL(receiveMarketHistData_Signal(QList<QList<double>>)),
            this, SLOT(receiveMarketHistData_slot(QList<QList<double>>)));

    connect(m_histdataWorker, SIGNAL(tableViewInfoSignal(QString)),
            this, SLOT(receiveTableViewInfo(QString)));

    m_histdataWorker->moveToThread(&m_histdataThread);

    m_histdataThread.start();
}

void ChartForm::initMonitorThread() {
    m_monitorWorker = new MonitorRealTimeData(QString("%1").arg(m_chartViewID+1),
                                              m_isBuySalePortfolio,
                                              m_dbhost, m_updateTime, m_macdTime,
                                              m_hedgeIndexCode, m_hedgeIndexCount,
                                              m_secodebuyCountMap, m_secodeNameList,
                                              m_buyStrategyMap, m_saleStrategyMap);

    connect(m_monitorWorker, SIGNAL(sendRealTimeData(ChartData)),
            this, SLOT(receiveRealTimeData(ChartData)));

    connect(this, SIGNAL(getPreCloseSpread()),
            m_monitorWorker, SLOT(getPreCloseSpread()));

    connect(m_monitorWorker, SIGNAL(sendPreCloseData(double)),
            this, SLOT(receivePreCloseData(double)));

    connect(m_monitorWorker, SIGNAL(sendTradeOver()),
            this, SLOT(receiveTradeOver()));

    m_monitorWorker->moveToThread(&m_MonitorThread);

    connect(&m_MonitorThread, SIGNAL(finished()),
            m_monitorWorker, SLOT(deleteLater()));

    m_MonitorThread.start();
}

void ChartForm::writeExcelData() {
    QStringList timeStringList;
    QStringList strategyValueList;
    QList<QStringList> excelData;
    for (int i = 0; i < m_timeData.size(); ++i) {
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_timeData[i]);
        timeStringList.append(tmpDatetime.toString (m_timeTypeFormat));
        strategyValueList.append(QString("%1").arg(m_strategyData[i]));
    }
//    printList(timeStringList, "timeStringList");
//    printList(strategyValueList, "strategyValueList");
    QString fileName = "D:/excel/cplus/hedged.xlsx";
    excelData.append(timeStringList);
    excelData.append(strategyValueList);
    int result = writeMatrixData(fileName, excelData, "Sheet1", true);
    qDebug() << "writeExcelData: " << result;
}

QList<QStringList> ChartForm::getExcelData(QStringList choosenKeyValueList) {
    QList<QStringList> excelData;
    QStringList timeData;
    timeData.append("时间");
    for (int i = 0; i < m_timeData.size(); ++i) {
        timeData.append(QDateTime::fromMSecsSinceEpoch (m_timeData[i]).toString(m_timeTypeFormat));
    }
    excelData.append(timeData);

    if (choosenKeyValueList.indexOf("对冲点差值") >=0 ) {
        excelData.append(transNumbData("对冲点差值", m_strategyData));
    }

    if (choosenKeyValueList.indexOf("沪深300指数") >=0 ) {
        excelData.append(transNumbData("沪深300指数", m_indexCodeData));
    }

    if (choosenKeyValueList.indexOf("成交额") >=0 ) {
        excelData.append(transNumbData("成交额", m_votData));
    }

    if (choosenKeyValueList.indexOf("DIFF") >=0 ) {
        QStringList tmpData;
        tmpData.append("DIFF");
        for (int i = 0; i < m_macdData.size(); ++i) {
            tmpData.append(QString("%1").arg(m_macdData[i].m_diff));
        }
        excelData.append(tmpData);
    }

    if (choosenKeyValueList.indexOf("DEA") >=0 ) {
        QStringList tmpData;
        tmpData.append("DEA");
        for (int i = 0; i < m_macdData.size(); ++i) {
            tmpData.append(QString("%1").arg(m_macdData[i].m_dea));
        }
        excelData.append(tmpData);
    }

    if (choosenKeyValueList.indexOf("MACD") >=0 ) {
        QStringList tmpData;
        tmpData.append("MACD");
        for (int i = 0; i < m_macdData.size(); ++i) {
            tmpData.append(QString("%1").arg(m_macdData[i].m_macd));
        }
        excelData.append(tmpData);
    }
    return excelData;
}

QString ChartForm::getExcelFileName(QStringList keyValueList, QString fileDir) {
    QString fileName = fileDir;
    fileName += QString("%1_%2").arg(m_strategyName).arg(m_timeType);
    if (m_isRealTime) {
        fileName += QString("_%1_实时数据").arg(QDate::currentDate().toString("yyyyMMdd"));
    } else {
        fileName += QString("_%1_%2").arg(m_startDate).arg(m_endDate);
    }
    fileName += QString("_%1.xlsx").arg(keyValueList.join("_"));
    return fileName;
}

void ChartForm::receivePreCloseData(double preSpread) {
    m_preSpread = preSpread;
    if (m_isLayoutSetted) {
        ui->preCLoseSpreadValue_Label->setText(QString("昨日点差: %1").arg(m_preSpread));
    }
    qDebug() << "m_PreSpread: " << m_preSpread;
    disconnect(m_monitorWorker, SIGNAL(sendPreCloseData(double)),
               this, SLOT(receivePreCloseData(double)));

    if (isTradingStart()) {
        emit getHistDataSignal();
    }
}

void ChartForm::receiveMarketHistData_slot(QList<QList<double> > allData) {
    qDebug() << "ChartForm::receiveProcessedHistoryData: " << QThread::currentThreadId();

    m_timeData = allData[0];
    m_strategyData = allData[1];
    m_votData = allData[2];
    QList<double> macdData = allData[3];
    if (false == m_isBuySalePortfolio) {
        m_indexCodeData = allData[4];
    }
    for (int i = 0; i < macdData.size (); i += 5){
        m_macdData.append (MACD(macdData[i], macdData[i+1],
                                macdData[i+2], macdData[i+3], macdData[i+4]));
    }
    updateProgramInfo (m_programInfoTableView, QString(QString("完成数据计算, 计算后数据总数: %1"))
                                               .arg(m_timeData.size () * 6));
    updateProgramInfo (m_programInfoTableView, QString("显示图像"));
    qDebug() << "timeData.size:     " << m_timeData.size ();
    qDebug() << "strategyData.size: " << m_strategyData.size ();
    qDebug() << "votData.size:      " << m_votData.size ();
    qDebug() << "macdData.size:     " << m_macdData.size ();
    qDebug() << "indexCodeData.size:      " << m_indexCodeData.size ();

    initLayout ();

    if (m_isRealTime) {
        if (m_macdData.size() > 0) {
            m_monitorWorker->setInitMacd(m_macdData[m_macdData.size()-1]);
        }
        m_monitorWorker->startTimer();
    }
}

void ChartForm::receiveRealTimeData(ChartData curChartData) {
    qDebug() << "ChartForm::receiveRealTimeData(): " << QThread::currentThread();
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

void ChartForm::receiveTableViewInfo(QString msg) {
    updateProgramInfo (m_programInfoTableView, msg);
    if (msg.indexOf("Error") >= 0) {
        QMessageBox::critical(NULL, "ERROR", msg);
    }
}

void ChartForm::updateChart() {
    updateAxis();
    updateSeries();
    updateMousePos();
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

void ChartForm::updateAxis() {
    int curPointNumb = m_timeData.size();
    QValueAxis *strategyAxisY = dynamic_cast<QValueAxis *>(m_strategyChart->axisY());
    QList<double> allStrategyData = m_strategyData;

    if (m_preSpread == 0) {
        emit(getPreCloseSpread());
        if (m_strategyData.last() > strategyAxisY->max() || m_strategyData.last() < strategyAxisY->min()) {
            QList<double>  strategyAxisYRange = getChartYvalueRange(allStrategyData);
            strategyAxisY -> setRange (strategyAxisYRange[0], strategyAxisYRange[1]);
        }
    } else {
        allStrategyData.append(m_preSpread);
        if (m_preSpread > strategyAxisY->max() || m_preSpread < strategyAxisY->min() ||
            m_strategyData.last() > strategyAxisY->max() || m_strategyData.last() < strategyAxisY->min()) {
            QList<double>  strategyAxisYRange = getChartYvalueRange(allStrategyData);
            m_strategyChartRange = strategyAxisYRange;
            strategyAxisY -> setRange (strategyAxisYRange[0], strategyAxisYRange[1]);
        }
    }

    QValueAxis *votAxisY = dynamic_cast<QValueAxis *>(m_votrunoverChart->axisY());
    if (m_votData.last() > votAxisY->max() || m_votData.last() < votAxisY->min()) {
        QList<double>  votAxisYRange = getChartYvalueRange(m_votData);
        m_votChartRange = votAxisYRange;
        votAxisY -> setRange (votAxisYRange[0], votAxisYRange[1]);
    }

    QValueAxis *macdAxisY = dynamic_cast<QValueAxis *>(m_macdChart->axisY());
    if (m_macdData.last().m_dea > macdAxisY->max() || m_macdData.last().m_dea < macdAxisY->min() ||
        m_macdData.last().m_diff > macdAxisY->max() || m_macdData.last().m_diff < macdAxisY->min() ||
        m_macdData.last().m_macd > macdAxisY->max() || m_macdData.last().m_macd < macdAxisY->min()) {
        QList<double>  macdAxisYRange = getMACDRange(m_macdData, 10);
        m_macdChartRange = macdAxisYRange;
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
    } else if (m_timeAxisUpdateData.size() <= m_timeData.size()){
        setTimeAxisUpdateData();
        m_strategyChart->removeAxis(strategyAxisX);
        QCategoryAxis* newStrategyAxisX = getTimeAxis();
        m_strategyChart->addAxis (newStrategyAxisX, Qt::AlignBottom);
        m_strategySeries->attachAxis (newStrategyAxisX);
        m_preSpreadSeries->attachAxis(newStrategyAxisX);
        m_strategyLabelSeries->attachAxis(newStrategyAxisX);

        m_votrunoverChart->removeAxis(votAxisX);
        QCategoryAxis* newVotAxisX = getTimeAxis();
        m_votrunoverChart->addAxis (newVotAxisX, Qt::AlignBottom);
        m_votBarSeries->attachAxis (newVotAxisX);
        m_votLabelSeries->attachAxis(newVotAxisX);

        m_macdChart->removeAxis(macdAxisX);
        QCategoryAxis* newMacdAxisX = getTimeAxis();
        m_macdChart->addAxis (newMacdAxisX, Qt::AlignBottom);
        m_diffSeries->attachAxis (newMacdAxisX);
        m_deaSeries->attachAxis (newMacdAxisX);
        m_macdSeries->attachAxis (newMacdAxisX);
        m_macdLabelSeries->attachAxis(newMacdAxisX);

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

    if (m_macdData[curPointNumb-1].m_macd > 0) {
        macdSetList[0]->append(m_macdData[curPointNumb-1].m_macd);
        macdSetList[1]->append(0);
    } else {
        macdSetList[0]->append(0);
        macdSetList[1]->append(m_macdData[curPointNumb-1].m_macd);
    }

//    int strategyHeight = m_strategyChartView->size().height();
//    int votHeight = m_votrunoverChartView->size().height();
//    int macdHeight = m_macdChartView->size().height();
//    int sumHeight = strategyHeight + votHeight + macdHeight;
//    strategyHeight = sumHeight / 2;
//    votHeight = sumHeight / 4;
//    macdHeight = sumHeight / 4;
//    m_votrunoverChartView->setMaximumHeight(votHeight);
//    m_macdChartView->setMaximumHeight(macdHeight);

    if (!m_isKeyMove) {
       setPropertyValue(m_timeData.size()-1);
    }
}

void ChartForm::updateMousePos() {
    moveMouse(0);
}

void ChartForm::initLayout () {
    ui->setupUi(this);
    this->setWindowTitle(m_strategyName);
    m_title = QString("策略: %1 , MACD: %2, %3, %4 ")
              .arg(m_strategyName).arg(m_macdTime[0]).arg(m_macdTime[1]).arg (m_macdTime[2]);

    ui->Title_Label->setText(m_title);

    initChartView();
    initTheme();

    if (NULL != m_strategyChartView) {
        ui->gridLayout->addWidget (m_strategyChartView, 0, 0);
    }
    if (NULL != m_votrunoverChartView) {
        ui->gridLayout->addWidget (m_votrunoverChartView, 1, 0);
    }
    if (NULL != m_macdChartView) {
        ui->gridLayout->addWidget (m_macdChartView, 2, 0);
    }
    ui->gridLayout->setRowStretch(0,5);
    ui->gridLayout->setRowStretch(1,2);
    ui->gridLayout->setRowStretch(2,3);
    this->setMouseTracking(true);
    m_timeAxisUpdateData = m_timeData;
    m_isLayoutSetted = true;

    m_endTime = QTime::currentTime();
    int costSecs = m_endTime.msecsSinceStartOfDay() - m_startTime.msecsSinceStartOfDay();
    updateProgramInfo (m_programInfoTableView, QString("读取计算数据,绘制图像耗费: %1s, 线程: %2")
                                               .arg(costSecs/1000).arg(m_threadNumb));

    initTableContextMenu();

    setLineColor();
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

void ChartForm::initChartView() {
    setStrategyChartView();
    setVotRunoverChartView ();
    setMACDChartView();
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
        QString info = QString(QString("昨日点差: %1").arg(m_preSpread));
        ui->preCLoseSpreadValue_Label->setText(info);
        qDebug() << info;
        for (int i = 0; i < m_strategyData.size (); ++i) {
            m_preSpreadSeries->append(i, m_preSpread);
        }
    }

    QPen red(Qt::red);
    red.setWidth(3);
    m_strategySeries->setPen(red);
    m_strategySeries->setName(QString("点差"));
    m_strategySeries->setUseOpenGL (true);
    m_preSpreadSeries->setName(QString("昨日点差"));
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

    m_strategyChartRange = axisYRange;
    m_strategyLabelSeries = new QLineSeries;
    m_strategyLabelSeries->append(m_oldLabelIndex, m_strategyChartRange[0]);
    m_strategyLabelSeries->append(m_oldLabelIndex, m_strategyChartRange[1]);

    m_strategyChart = new QChart();
    m_strategyChart->addSeries (m_strategySeries);
    m_strategyChart->addSeries(m_preSpreadSeries);
    m_strategyChart->addSeries(m_strategyLabelSeries);

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

    m_strategyLabelSeries->attachAxis(axisX);
    m_strategyLabelSeries->attachAxis(axisY);
}

void ChartForm::setVotRunoverChartView () {
    QCategoryAxis* axisX = getAxisX();
    m_votBarSeries = new QStackedBarSeries();
    QBarSet *set = new QBarSet(QString("成交额:"));
    for (int i = 0; i < m_votData.size (); ++i) {
        set->append(m_votData.at (i));
    }
    m_votBarSeries->append(set);
    m_votBarSeries->setUseOpenGL (true);
    m_votBarSeries->setBarWidth(0);

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getChartYvalueRange(m_votData);
    axisY -> setRange (0, axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_votChartRange = axisYRange;
    m_votLabelSeries = new QLineSeries;
    m_votLabelSeries->append(m_oldLabelIndex, m_votChartRange[0]);
    m_votLabelSeries->append(m_oldLabelIndex, m_votChartRange[1]);

    m_votrunoverChart = new QChart();
    m_votrunoverChart->addSeries(m_votBarSeries);
    m_votrunoverChart->addSeries(m_votLabelSeries);
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

    m_votLabelSeries->attachAxis(axisX);
    m_votLabelSeries->attachAxis(axisY);
}

void ChartForm::setMACDChartView () {
    m_diffSeries = new QLineSeries();
    m_diffSeries->setName("DIFF");
    m_deaSeries = new QLineSeries();
    m_deaSeries->setName("DEA");
    m_macdSeries = new QStackedBarSeries();
    QBarSet *macdSetUp = new QBarSet("MACD 大于0");
    QBarSet *macdSetDown = new QBarSet("MACD 小于0");

    QCategoryAxis* axisX = getAxisX();
    for (int i = 0; i < m_macdData.size (); ++i) {
        m_diffSeries->append (i, m_macdData.at(i).m_diff);
        m_deaSeries->append (i, m_macdData.at(i).m_dea);
        if (m_macdData.at(i).m_macd >0) {
            macdSetUp->append (m_macdData.at(i).m_macd);
            macdSetDown->append (0);
        } else {
            macdSetUp->append (0);
            macdSetDown->append (m_macdData.at(i).m_macd);
        }
    }
    m_macdSeries->append(macdSetUp);
    m_macdSeries->append(macdSetDown);

    m_macdSeries->setBarWidth(0);
    m_diffSeries->setUseOpenGL (true);
    m_deaSeries->setUseOpenGL (true);
    m_macdSeries->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getMACDRange(m_macdData,10);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_macdChartRange = axisYRange;
    m_macdLabelSeries = new QLineSeries;
    m_macdLabelSeries->append(m_oldLabelIndex, m_macdChartRange[0]);
    m_macdLabelSeries->append(m_oldLabelIndex, m_macdChartRange[1]);

    m_macdChart = new QChart();
    m_macdChart->addSeries (m_diffSeries);
    m_macdChart->addSeries (m_deaSeries);
    m_macdChart->addSeries (m_macdSeries);
    m_macdChart->addSeries(m_macdLabelSeries);
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

    m_macdLabelSeries->attachAxis(axisX);
    m_macdLabelSeries->attachAxis(axisY);
}

void ChartForm::initTheme() {
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

void ChartForm::setLineColor() {
//    m_strategySeries->setColor(QColor(156,204,101));
    if (m_isRealTime) {
         m_strategySeries->setPen(QPen(QBrush(QColor(67,160,71)), 1));
         m_diffSeries->setPen(QPen(QBrush(QColor(220,220,220)), 1));
         m_deaSeries->setPen(QPen(QBrush(QColor(255,255,0)), 1));
    } else {
         m_strategySeries->setPen(QPen(QBrush(QColor(67,160,71)), 2));
         m_diffSeries->setPen(QPen(QBrush(QColor(220,220,220)), 2));
         m_deaSeries->setPen(QPen(QBrush(QColor(255,255,0)), 2));
    }

    m_macdSeries->barSets()[0]->setBorderColor(QColor(216,67,21));
    m_macdSeries->barSets()[0]->setColor(QColor(216,67,21));
    m_macdSeries->barSets()[1]->setBorderColor(QColor(0,229,255));
    m_macdSeries->barSets()[1]->setColor(QColor(0,229,255));

    m_votBarSeries->barSets()[0]->setBorderColor(QColor(38,166,154));
    m_votBarSeries->barSets()[0]->setColor(QColor(38,166,154));

//    m_votBarSeries->barSets()[0]->setBorderColor(QColor(34,139,34));
//    m_votBarSeries->barSets()[0]->setColor(QColor(34,139,34));

    m_strategyLabelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
    m_votLabelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
    m_macdLabelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
}

void ChartForm::setPropertyValue(int currIndex) {
    if (currIndex >= 0 && currIndex < m_strategyData.size()) {
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(qint64(m_timeData.at(currIndex)));
        QString dateTimeString = curDatetime.toString (m_timeTypeFormat);
        qreal strategyValue = m_strategyData.at(currIndex);
        qreal votrunoverValue = m_votData.at(currIndex);
        qreal DIFF = m_macdData.at(currIndex).m_diff;
        qreal DEA = m_macdData.at(currIndex).m_dea;
        qreal Macd = m_macdData.at(currIndex).m_macd;

        ui->TimeLabel->setText(QString("%1 %2").arg(QString("时间: ")).arg(dateTimeString));
        ui->StrategyValue_Label->setText(QString("%1 %2").arg(QString("点差: ")).arg(strategyValue));
        ui->VotRunover_Label->setText(QString("%1 %2").arg(QString("成交额: ")).arg(votrunoverValue));

        ui->DIFF_Label->setText(QString("DIFF: %1").arg(DIFF));
        ui->DEA_Label->setText(QString("DEA: %1").arg(DEA));
        ui->MACD_Label->setText(QString("MACD: %1").arg(Macd));
    }
}

void ChartForm::updateLableSeries(int index) {
    if (index >= 0 && index < m_strategyData.size()) {
        for (QPointF point: m_strategyLabelSeries->points()) {
            m_strategyLabelSeries->remove(point);
        }
        for (QPointF point: m_votLabelSeries->points()) {
            m_votLabelSeries->remove(point);
        }
        for (QPointF point: m_macdLabelSeries->points()) {
            m_macdLabelSeries->remove(point);
        }

        m_strategyLabelSeries->append(index, m_strategyChartRange[0]);
        m_strategyLabelSeries->append(index, m_strategyChartRange[1]);
        m_votLabelSeries->append(index, m_votChartRange[0]);
        m_votLabelSeries->append(index, m_votChartRange[1]);
        m_macdLabelSeries->append(index, m_macdChartRange[0]);
        m_macdLabelSeries->append(index, m_macdChartRange[1]);
    }

}

void ChartForm::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    if (m_isKeyMove)  {
        m_isKeyMove = false;
    } else {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QPoint curPoint = mouseEvent->pos ();
        int currIndex = -1;
        if (watched == m_strategyChartView) {
            QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue (curPoint);
            currIndex = qFloor(curStrategyChartChartPoint.x());
        }
        if (watched == m_votrunoverChartView) {
            QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue (curPoint);
            currIndex = qFloor(curVotrunoverChartChartPoint.x());
        }
        if (watched == m_macdChartView) {
            QPointF curMacdChartChartPoint = m_macdChart->mapToValue (curPoint);
            currIndex = qFloor(curMacdChartChartPoint.x());
        }

        setPropertyValue(currIndex);
        updateLableSeries(currIndex);
    }
}

double ChartForm::getPointXDistance() {
    int testIndex = 0;
    QPointF pointa = m_strategyChart->mapToPosition(QPointF(testIndex, m_strategyData[testIndex]));
    QPointF pointb = m_strategyChart->mapToPosition(QPointF(testIndex+1, m_strategyData[testIndex+1]));
    double distance = pointb.x() - pointa.x();
    return distance;
}

void ChartForm::KeyReleaseFunc(QEvent *event) {
    QKeyEvent* keyEvent = (QKeyEvent*)event;
    int step = 0;

    if (keyEvent->key() == Qt::Key_Left) {
        step = -1;
    }
    if (keyEvent->key() == Qt::Key_Right) {
        step = 1;
    }

    moveMouse(step);
}

void ChartForm::moveMouse(int step) {
    double pointXDistance = getPointXDistance();
    if (m_oldPointDistance != pointXDistance) {
        if (m_oldPointDistance != -1) {
            double deltaDistance = pointXDistance - m_oldPointDistance;
            m_mouseInitPos.setX(m_mouseInitPos.x() + deltaDistance);
        }
        m_oldPointDistance = pointXDistance;
    }

    if (step != 0) {
        m_keyMoveCount += step;
        m_currTimeIndex += step;
        float move_distance = pointXDistance * m_keyMoveCount;

        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeData.size()) {
            m_isKeyMove = true;
            setPropertyValue(m_currTimeIndex);
            updateLableSeries(m_currTimeIndex);
        }

        if (move_distance >= 1 || move_distance <=-1 || move_distance == 0) {
            QCursor::setPos(m_mouseInitPos.x() + move_distance, m_mouseInitPos.y());
        }
    }
}

void ChartForm::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    QPointF transPoint;
    m_currTimeIndex = -1;
    m_mouseInitPos = QCursor::pos();
    m_keyMoveCount = 0;
    double deltaInGlobalPointAndChartPoint = 0;
    if (watched == m_strategyChartView) {
        QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(curStrategyChartChartPoint.x());
        transPoint = m_strategyChart->mapToPosition( (QPointF(m_currTimeIndex, m_strategyData[m_currTimeIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (watched == m_votrunoverChartView) {
        QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(curVotrunoverChartChartPoint.x());
        transPoint = m_votrunoverChart->mapToPosition( (QPointF(m_currTimeIndex, m_votData[m_currTimeIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (watched == m_macdChartView) {
        QPointF curMacdChartChartPoint = m_macdChart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(curMacdChartChartPoint.x());
        transPoint = m_macdChart->mapToPosition( (QPointF(m_currTimeIndex, m_macdData[m_currTimeIndex].m_macd)));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }

    if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeData.size()) {
        m_isKeyMove = true;
        setPropertyValue(m_currTimeIndex);
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
    }
}

void ChartForm::closeEvent(QCloseEvent *event) {
    event;
    if (!m_isclosed) {
        qDebug() << "--- Close -- ";
        if (m_isRealTime) {
            if (NULL != m_monitorWorker) {                
                m_monitorWorker->stopTimer();
            }            
        }
        emit windowClose_signal(m_chartViewID);
        m_isclosed = true;
    }
}
