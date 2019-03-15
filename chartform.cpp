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
#include <algorithm>
#include <math.h>

#include "chartform.h"
#include "ui_chartform.h"
#include "toolfunc.h"
#include "macd.h"
#include "time_func.h"
#include "secode_func.h"
#include "widget_func.h"
#include "io_func.h"
#include "excel_func.h"
#include "process_data_func.h"

#include "widgetprocess.h"

#pragma execution_character_set("utf-8")

using std::max;

ChartForm::ChartForm(int windowID, DatabaseParam dbParam, HedgedParam hedgedParam,QWidget *parent):
    m_windowID(windowID),
    BaseChart(1, parent),
    m_hedgedParam(hedgedParam), 
    m_dbParam(dbParam),
    ui(new Ui::ChartForm)
{
    initCommonData();
    if (m_hedgedParam.m_isRealTime)
    {
        initRealTimeData();
    }
    else
    {
        initHistoryData();
    }
}

ChartForm::~ChartForm()
{
    delete ui;

    releaseHistWorker();

    m_histdataThread.terminate();

    // m_histdataThread.quit();
    // m_histdataThread.wait();

    if (NULL != m_histdataWorker)
    {
        m_histdataWorker = NULL;
    }
}

void ChartForm::initCommonData()
{
    m_initTime = QDateTime::currentDateTime();
    m_getRealDataTime = QDateTime::currentDateTime();
    m_showRealDataTime = QDateTime::currentDateTime();
    m_getHistDataTime = QDateTime::currentDateTime();

    m_isTest = false;
    m_startTime = QTime::currentTime();
    m_keyMoveCount = 0;
    m_currTimeIndex = 0;
    m_isKeyMove = false;
    m_mouseInitPos = QPoint(-1, -1);
    m_oldPointDistance = -1;
    m_setMouseTimeIndex = false;
    m_timeAxisUpdatePercent = 0.2;
    m_oldLabelIndex = 0;

    m_strategyLabelSeries = NULL;
    m_votLabelSeries = NULL;
    m_macdLabelSeries = NULL;

    m_strategySeries = NULL;
    m_strategyChartView = NULL;
    m_strategyChart = NULL;
    m_preSpreadSeries = NULL;

    m_votrunoverChartView = NULL;
    m_votrunoverChart = NULL;
    m_votLineSeries = NULL;

    m_diffSeries = NULL;
    m_deaSeries = NULL;
    m_macdChartView = NULL;
    m_macdChart = NULL;
    m_macdUpLineSeries = NULL;
    m_macdDownLineSeries = NULL;

    m_histdataWorker = NULL;
    m_preSpread = 0;
    m_isclosed = false;
    m_isLayoutSetted = false;

    m_isBarSet = false;
    m_isSingleLine = true;

    registSignalParamsType();
    initExtractKeyValueList();
}

void ChartForm::initHistoryData()
{
    m_dbParam.m_databaseName = "MarketData_" + m_hedgedParam.m_timeType;

    if (m_hedgedParam.m_timeType.contains("m") && m_hedgedParam.m_timeType != "month")
    {
        m_timeTypeFormat = "yyyyMMdd h:mm";
    }
    else
    {
        m_timeTypeFormat = "yyyyMMdd";
    }
    m_chartXaxisTickCount = 10;

    initHistdataThread();

    emit getHistPortfolioData_Signal();
}

void ChartForm::initRealTimeData()
{
    m_dbParam.m_databaseName              = "MarketData_RealTime";
    m_updateTime                = 3 * 1000;
    m_timeTypeFormat            = "hh:mm:ss";
    m_chartXaxisTickCount       = 5;
    m_getPreCloseDataTime       = QTime(9, 25, 0);
    m_getPreCloseDataUpdateTime = 10000;

    initHistdataThread();

    connect(&m_getPreCloseDataTimer, SIGNAL(timeout()),
            this,                    SLOT(getPreCloseData_timer_slot()));

    if (isTradingStart())
    {
        emit getPreCloseData_signal();
    }
    else
    {
        initLayout();
        if (QTime::currentTime() < m_getPreCloseDataTime)
        {
            m_getPreCloseDataTimer.start(m_getPreCloseDataUpdateTime);
        }
        else
        {
            emit getPreCloseData_signal();
        }
    }
}

void ChartForm::registSignalParamsType()
{
    qRegisterMetaType<ChartData>("ChartData");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");    
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
}

void ChartForm::initExtractKeyValueList()
{
    m_extractKeyValueList.append(QString("对冲点差值"));
    m_extractKeyValueList.append(QString("DIFF"));
    m_extractKeyValueList.append(QString("DEA"));
    m_extractKeyValueList.append(QString("MACD"));
    if (m_hedgedParam.m_isPortfolioHedge == false)
    {
        m_extractKeyValueList.append(QString("沪深300指数"));
        m_extractKeyValueList.append(QString("成交额"));
    }
}

QList<QMyChartView *> ChartForm::getChartViewList()
{
    QList<QMyChartView *> result;
    if (NULL != m_strategyChartView)
    {
        result.append(m_strategyChartView);
    }
    if (NULL != m_votrunoverChartView)
    {
        result.append(m_votrunoverChartView);
    }
    if (NULL != m_macdChartView)
    {
        result.append(m_macdChartView);
    }
    return result;
}

void ChartForm::initHistdataThread()
{
    m_histdataWorker = new HistoryData(m_dbParam, m_hedgedParam, CSSParam());

    connect(&m_histdataThread, SIGNAL(finished()),
            m_histdataWorker, SLOT(deleteLater()));

    connect(this, SIGNAL(getPreCloseData_signal()),
            m_histdataWorker, SLOT(getPreCloseData_slot()));

    connect(m_histdataWorker, SIGNAL(sendPreCloseData_signal(double)),
            this, SLOT(sendPreCloseData_slot(double)));

    connect(this, SIGNAL(getHistPortfolioData_Signal()),
            m_histdataWorker, SLOT(getHistPortfolioData_slot()));

    connect(m_histdataWorker, SIGNAL(sendHistPortfolioData_Signal(QList<QList<double>>)),
            this, SLOT(sendHistPortfolioData_slot(QList<QList<double>>)));

    connect(this, SIGNAL(getRealtimeData_signal()),
            m_histdataWorker, SLOT(getRealtimeData_slot()));

    connect(this, SIGNAL(processRealtimeDataComplete_signal()),
            m_histdataWorker, SLOT(processRealtimeDataComplete_slot()));

    connect(m_histdataWorker, SIGNAL(sendRealtimeSpreadMACDData_signal(ChartData)),
            this, SLOT(sendRealtimeSpreadMACDData_slot(ChartData)));

    m_histdataWorker->moveToThread(&m_histdataThread);

    m_histdataThread.start();
}

void ChartForm::releaseHistWorker()
{
    if (NULL != m_histdataWorker)
    {
        disconnect(this, SIGNAL(getRealtimeData_signal()),
                m_histdataWorker, SLOT(getRealtimeData_slot()));

        disconnect(this, SIGNAL(processRealtimeDataComplete_signal()),
                m_histdataWorker, SLOT(processRealtimeDataComplete_slot()));

        disconnect(m_histdataWorker, SIGNAL(sendRealtimeSpreadMACDData_signal(ChartData)),
                this, SLOT(sendRealtimeSpreadMACDData_slot(ChartData)));

        m_histdataWorker->releaseMonitorWorker();
    }
}

void ChartForm::getPreCloseData_timer_slot()
{
    if (QTime::currentTime() > m_getPreCloseDataTime)
    {
        emit getPreCloseData_signal();

        if (m_getPreCloseDataTimer.isActive())
        {
            m_getPreCloseDataTimer.stop();
        }
    }
}

void ChartForm::sendPreCloseData_slot(double preSpread)
{
    if (m_hedgedParam.m_hedgedType > 0)
    {
        m_preSpread = preSpread;
    }
    else
    {
        m_preSpread = 0;
    }

    if (m_isLayoutSetted)
    {
        ui->preCLoseSpreadValue_Label->setText(QString("昨日点差: %1").arg(m_preSpread));
    }

    if (isTradingStart())
    {
        emit getHistPortfolioData_Signal();
    }
    else if (m_hedgedParam.m_isRealTime)
    {
        emit getRealtimeData_signal();
    }

    m_getHistDataTime = QDateTime::currentDateTime();
    qDebug() << "Get PreClose Data cost: " << m_getHistDataTime.toMSecsSinceEpoch() - m_initTime.toMSecsSinceEpoch()
             << " MSecs";
}

void ChartForm::sendHistPortfolioData_slot(QList<QList<double>> allData)
{
    WidgetProcess::Instance().setWidgetProcess(true);
    WidgetProcess::Instance().lock("Hist " + m_windowName);

    QDateTime startTime = QDateTime::currentDateTime();
    qDebug() << "Get Hist Data cost: " << startTime.toMSecsSinceEpoch() - m_getHistDataTime.toMSecsSinceEpoch()
             << " MSecs, DataNumb: " << allData[0].size()
             << m_windowName;

    m_timeData = allData[0];
    m_earningList = allData[1];
    m_votList = allData[2];
    QList<double> macdData = allData[3];

    if (false == m_hedgedParam.m_isPortfolioHedge)
    {
        m_indexCodeData = allData[4];
    }
    for (int i = 0; i < macdData.size(); i += 5)
    {
        m_MACDList.append(MACD(macdData[i], macdData[i + 1],
                               macdData[i + 2], macdData[i + 3], macdData[i + 4]));
    }

    QString info = QString("完成数据计算, 计算后数据总数: %1 \n显示图像").arg(m_timeData.size() * 6);
    emit updateProgramInfo_signal(info, false);

    /*
        qDebug() << "timeData.size:      " << m_timeData.size ();
        qDebug() << "strategyData.size:  " << m_earningList.size ();
        qDebug() << "votData.size:       " << m_votList.size ();
        qDebug() << "macdData.size:      " << m_MACDList.size ();
        qDebug() << "indexCodeData.size: " << m_indexCodeData.size ();
    */

    initLayout();

    if (m_isTest == false)
    {
        if (m_hedgedParam.m_isRealTime && !isTradingOver())
        {
            emit getRealtimeData_signal();
        }
    }
    else
    {
        emit getRealtimeData_signal();
    }

    qDebug() << "Show Hist Pic cost: " << QDateTime::currentMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()
             << " MSecs";
    qDebug() << "";

    WidgetProcess::Instance().unlock("Hist " + m_windowName);
    WidgetProcess::Instance().setWidgetProcess(false);       
}

void ChartForm::sendRealtimeSpreadMACDData_slot(ChartData currChartData)
{
    if (WidgetProcess::Instance().isProcessing())
    {
        qDebug() << m_windowName << " update give up";
        return;
    }

    WidgetProcess::Instance().setWidgetProcess(true);
    WidgetProcess::Instance().lock("Real " + m_windowName);

    m_getRealDataTime = QDateTime::currentDateTime();

    // qDebug() << "ChartForm:  " << currChartData.m_earningData 
    //          << QDateTime::fromMSecsSinceEpoch(currChartData.m_timeData).toString("yyyyMMdd hh:mm:ss");

    qDebug() << "Get  Real Data Cost " << m_getRealDataTime.toMSecsSinceEpoch() - m_showRealDataTime.toMSecsSinceEpoch()
             << " MSecs" << m_windowName;

    m_earningList.append(currChartData.m_earningData);
    m_votList.append(currChartData.m_votData);
    m_timeData.append(currChartData.m_timeData);
    m_MACDList.append(currChartData.m_MACDData);
    m_indexCodeData.append(currChartData.m_hedgeData);

    updateChart();

    m_showRealDataTime = QDateTime::currentDateTime();

    qDebug() << "Show Real Data Cost " << m_showRealDataTime.toMSecsSinceEpoch() - m_getRealDataTime.toMSecsSinceEpoch()
             << " MSecs";
    qDebug() << "";

    emit processRealtimeDataComplete_signal();

    WidgetProcess::Instance().unlock("Real " + m_windowName);
    WidgetProcess::Instance().setWidgetProcess(false);    
}

void ChartForm::receiveTableViewInfo_slot(QString msg)
{
    bool isWarning = false;
    if (msg.indexOf("Error") >= 0)
    {
        isWarning = true;
    }
    emit updateProgramInfo_signal(msg, isWarning);
}

void ChartForm::updateChart()
{

    updateAxis();
    updateSeries();
    updateMousePos();


}

QCategoryAxis *ChartForm::getTimeAxis()
{
    QCategoryAxis *axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(m_timeAxisUpdateData.size(), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i)
    {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch(m_timeAxisUpdateData[xpos]);
        axisX->append(tmpDatetime.toString(m_timeTypeFormat), xpos);
    }
    axisX->setMax(m_timeAxisUpdateData.size() - 1);
    return axisX;
}

void ChartForm::setTimeAxisUpdateData()
{
    int addedNumb = m_timeData.size() * m_timeAxisUpdatePercent;
    QDateTime curDateTime = QDateTime::fromMSecsSinceEpoch(m_timeData[m_timeData.size() - 1]);
    QTime amStopTime = QTime(11, 30, 20);
    QTime pmStartTime = QTime(13, 0, 0);
    for (int i = 0; i < addedNumb; ++i)
    {
        QDateTime tmpTime = curDateTime.addMSecs(m_updateTime * (i + 1));
        if (tmpTime.time() >= amStopTime && tmpTime.time() < pmStartTime)
        {
            break;
        }
        else
        {
            m_timeAxisUpdateData.append(tmpTime.toMSecsSinceEpoch());
        }
    }
}

void ChartForm::updateAxis()
{
    int curPointNumb = m_timeData.size();

    QCategoryAxis *strategyAxisX = NULL;
    QCategoryAxis *votAxisX = NULL;
    QCategoryAxis *macdAxisX = NULL;

    if (NULL != m_strategyChart)
    {
        QValueAxis *strategyAxisY = dynamic_cast<QValueAxis *>(m_strategyChart->axisY());
        QList<double> allStrategyData = m_earningList;

        if (m_preSpread == 0)
        {
            if (m_hedgedParam.m_hedgedType > 0 && m_hedgedParam.m_isPortfolioHedge == false)
            {
                emit(getPreCloseData_signal());
            }

            if (m_earningList.last() > strategyAxisY->max() || m_earningList.last() < strategyAxisY->min())
            {
                QList<double> strategyAxisYRange = getChartYvalueRange(allStrategyData);
                strategyAxisY->setRange(strategyAxisYRange[0], strategyAxisYRange[1]);
            }
        }
        else
        {
            allStrategyData.append(m_preSpread);
            if (m_preSpread > strategyAxisY->max() || m_preSpread < strategyAxisY->min() ||
                m_earningList.last() > strategyAxisY->max() || m_earningList.last() < strategyAxisY->min())
            {
                QList<double> strategyAxisYRange = getChartYvalueRange(allStrategyData);
                m_strategyChartRange = strategyAxisYRange;
                strategyAxisY->setRange(strategyAxisYRange[0], strategyAxisYRange[1]);
            }
        }

        strategyAxisX = dynamic_cast<QCategoryAxis *>(m_strategyChart->axisX());
    }

    if (NULL != m_votrunoverChart)
    {
        QValueAxis *votAxisY = dynamic_cast<QValueAxis *>(m_votrunoverChart->axisY());
        if (m_votList.last() > votAxisY->max() || m_votList.last() < votAxisY->min())
        {
            QList<double> votAxisYRange = getChartYvalueRange(m_votList);
            m_votChartRange = votAxisYRange;
            votAxisY->setRange(votAxisYRange[0], votAxisYRange[1]);
        }
        votAxisX = dynamic_cast<QCategoryAxis *>(m_votrunoverChart->axisX());
    }

    if (NULL != m_macdChart)
    {
        QValueAxis *macdAxisY = dynamic_cast<QValueAxis *>(m_macdChart->axisY());
        if (m_MACDList.last().m_dea > macdAxisY->max() || m_MACDList.last().m_dea < macdAxisY->min() ||
            m_MACDList.last().m_diff > macdAxisY->max() || m_MACDList.last().m_diff < macdAxisY->min() ||
            m_MACDList.last().m_macd > macdAxisY->max() || m_MACDList.last().m_macd < macdAxisY->min())
        {
            QList<double> macdAxisYRange = getMACDRange(m_MACDList, 10);
            m_macdChartRange = macdAxisYRange;
            macdAxisY->setRange(macdAxisYRange[0], macdAxisYRange[1]);
        }
        macdAxisX = dynamic_cast<QCategoryAxis *>(m_macdChart->axisX());
    }

    if (curPointNumb < m_chartXaxisTickCount)
    {
        QString curTime = QDateTime::fromMSecsSinceEpoch(m_timeData[curPointNumb - 1]).toString(m_timeTypeFormat);
        if (NULL != strategyAxisX)
        {
            strategyAxisX->append(curTime, curPointNumb - 1);
            strategyAxisX->setMax(curPointNumb - 1);
        }

        if (NULL != votAxisX)
        {
            votAxisX->append(curTime, curPointNumb - 1);
            votAxisX->setMax(curPointNumb - 1);
        }

        if (NULL != macdAxisX)
        {
            macdAxisX->append(curTime, curPointNumb - 1);
            macdAxisX->setMax(curPointNumb - 1);
        }
        m_timeAxisUpdateData = m_timeData;
    }
    else if (m_timeAxisUpdateData.size() <= m_timeData.size())
    {
        setTimeAxisUpdateData();
        QCategoryAxis *newStrategyAxisX = getTimeAxis();
        QCategoryAxis *newVotAxisX = getTimeAxis();
        QCategoryAxis *newMacdAxisX = getTimeAxis();

        if (NULL != m_strategyChart)
        {
            m_strategyChart->removeAxis(strategyAxisX);
            m_strategyChart->addAxis(newStrategyAxisX, Qt::AlignBottom);
        }

        if (NULL != m_strategySeries)
        {
            m_strategySeries->attachAxis(newStrategyAxisX);
        }

        if (NULL != m_preSpreadSeries)
        {
            m_preSpreadSeries->attachAxis(newStrategyAxisX);
        }

        if (NULL != m_strategyLabelSeries)
        {
            m_strategyLabelSeries->attachAxis(newStrategyAxisX);
        }

        if (NULL != m_votrunoverChart)
        {
            m_votrunoverChart->removeAxis(votAxisX);
            m_votrunoverChart->addAxis(newVotAxisX, Qt::AlignBottom);
        }

        if (NULL != m_votLineSeries)
        {
            m_votLineSeries->attachAxis(newVotAxisX);
        }

        if (NULL != m_votLabelSeries)
        {
            m_votLabelSeries->attachAxis(newVotAxisX);
        }

        if (NULL != m_macdChart)
        {
            m_macdChart->removeAxis(macdAxisX);
            m_macdChart->addAxis(newMacdAxisX, Qt::AlignBottom);
        }

        if (NULL != m_diffSeries)
        {
            m_diffSeries->attachAxis(newMacdAxisX);
        }

        if (NULL != m_deaSeries)
        {
            m_deaSeries->attachAxis(newMacdAxisX);
        }

        if (NULL != m_macdUpLineSeries && NULL != m_macdDownLineSeries)
        {
            m_macdDownLineSeries->attachAxis(newMacdAxisX);
            m_macdUpLineSeries->attachAxis(newMacdAxisX);
        }

        if (NULL != m_macdLabelSeries)
        {
            m_macdLabelSeries->attachAxis(newMacdAxisX);
        }
    }
}

void ChartForm::updateSeries()
{
    int curPointNumb = m_timeData.size();
    m_strategySeries->append(curPointNumb - 1, m_earningList[curPointNumb - 1]);

    if (m_preSpreadSeries->points().size() != m_strategySeries->points().size() - 1 && m_preSpread != 0)
    {
        m_preSpreadSeries->clear();
        for (int i = 0; i < m_strategySeries->points().size() - 1; ++i)
        {
            m_preSpreadSeries->append(i, m_preSpread);
        }
    }

    if (m_preSpread != 0)
    {
        m_preSpreadSeries->append(curPointNumb - 1, m_preSpread);
    }

    if (NULL != m_diffSeries)
    {
        m_diffSeries->append(curPointNumb - 1, m_MACDList[curPointNumb - 1].m_diff);
    }

    if (NULL != m_deaSeries)
    {
        m_deaSeries->append(curPointNumb - 1, m_MACDList[curPointNumb - 1].m_dea);
    }

    if (NULL != m_votLineSeries)
    {
        m_votLineSeries->append(curPointNumb - 1, 0);
        m_votLineSeries->append(curPointNumb - 1, m_votList[curPointNumb - 1]);
        m_votLineSeries->append(curPointNumb - 1, 0);
    }

    if (NULL != m_macdUpLineSeries && NULL != m_macdDownLineSeries)
    {
        if (m_MACDList[curPointNumb - 1].m_macd > 0)
        {
            m_macdUpLineSeries->append(curPointNumb - 1, 0);
            m_macdUpLineSeries->append(curPointNumb - 1, m_MACDList[curPointNumb - 1].m_macd);
            m_macdUpLineSeries->append(curPointNumb - 1, 0);
            m_macdDownLineSeries->append(curPointNumb - 1, 0);
        }
        else
        {
            m_macdDownLineSeries->append(curPointNumb - 1, 0);
            m_macdDownLineSeries->append(curPointNumb - 1, m_MACDList[curPointNumb - 1].m_macd);
            m_macdDownLineSeries->append(curPointNumb - 1, 0);
            m_macdUpLineSeries->append(curPointNumb - 1, 0);
        }
    }

    if (!m_isKeyMove)
    {
        setPropertyValue(m_timeData.size() - 1);
    }
}

void ChartForm::updateMousePos()
{
    moveMouse(0);
}

void ChartForm::initLayout()
{
    ui->setupUi(this);
    setWindowName();

    this->setWindowTitle(m_windowName);
    ui->Title_Label->setText(m_windowName);

    initChartView();

    initTheme();

    if (NULL != m_strategyChartView)
    {
        ui->gridLayout->addWidget(m_strategyChartView, 0, 0);
    }
    if (NULL != m_votrunoverChartView)
    {
        ui->gridLayout->addWidget(m_votrunoverChartView, 1, 0);
    }
    if (NULL != m_macdChartView)
    {
        ui->gridLayout->addWidget(m_macdChartView, 2, 0);
    }
    ui->gridLayout->setRowStretch(0, 5);
    ui->gridLayout->setRowStretch(1, 2);
    ui->gridLayout->setRowStretch(2, 3);
    this->setMouseTracking(true);
    m_timeAxisUpdateData = m_timeData;
    m_isLayoutSetted = true;

    m_endTime = QTime::currentTime();
    int costSecs = m_endTime.msecsSinceStartOfDay() - m_startTime.msecsSinceStartOfDay();

    QString info = QString("读取计算数据,绘制图像耗费: %1s, 线程: %2").arg(costSecs / 1000).arg(m_threadNumb);
    emit updateProgramInfo_signal(info, false);

    initTableContextMenu();

    setLineColor();
}

void ChartForm::setWindowName()
{
    if (m_hedgedParam.m_hedgedType == 0)
    {
        m_windowName = QString("%1 净值曲线, 权重日期: %2 ")
                      .arg(m_hedgedParam.m_portfolioName)
                      .arg(m_hedgedParam.m_weightDate);
    }
    else if (m_hedgedParam.m_hedgedType > 0)
    {
        m_windowName = QString("%1 %2 点差对冲")
                      .arg(m_hedgedParam.m_portfolioName)
                      .arg(m_hedgedParam.m_hedgedCode);
    }
    else if (m_hedgedParam.m_hedgedType < 0)
    {
        m_windowName = QString("%1 %2 净值对冲, 权重日期: %3")
                      .arg(m_hedgedParam.m_portfolioName)
                      .arg(m_hedgedParam.m_hedgedCode)
                      .arg(m_hedgedParam.m_weightDate);
    }
}

QCategoryAxis *ChartForm::getAxisX()
{
    QCategoryAxis *axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(m_timeData.size(), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i)
    {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch(m_timeData[xpos]);
        axisX->append(tmpDatetime.toString(m_timeTypeFormat), xpos);
    }
    axisX->setMax(m_timeData.size() - 1);
    return axisX;
}

void ChartForm::initChartView()
{
    setStrategyChartView();
    setVotRunoverChartView();
    setMACDChartView();
}

void ChartForm::setStrategyChartView()
{
    QCategoryAxis *axisX = getAxisX();
    QValueAxis *axisY = new QValueAxis;
    m_strategySeries = new QLineSeries;
    m_preSpreadSeries = new QLineSeries;

    for (int i = 0; i < m_earningList.size(); ++i)
    {
        m_strategySeries->append(i, m_earningList.at(i));
    }

    if (m_hedgedParam.m_isRealTime && m_preSpread != 0)
    {
        QString info = QString(QString("昨日点差: %1").arg(m_preSpread));
        ui->preCLoseSpreadValue_Label->setText(info);
        for (int i = 0; i < m_earningList.size(); ++i)
        {
            m_preSpreadSeries->append(i, m_preSpread);
        }
    }

    QPen red(Qt::red);
    red.setWidth(3);
    m_strategySeries->setPen(red);
    m_strategySeries->setName(QString("点差"));
    m_strategySeries->setUseOpenGL(true);
    m_preSpreadSeries->setName(QString("昨日点差"));
    m_preSpreadSeries->setUseOpenGL(true);

    QList<double> axisYRange;
    if (m_hedgedParam.m_isRealTime && m_preSpread != 0)
    {
        QList<double> allYdata = m_earningList;
        allYdata.append(m_preSpread);
        axisYRange = getChartYvalueRange(allYdata);
    }
    else
    {
        axisYRange = getChartYvalueRange(m_earningList);
    }

    axisY->setRange(axisYRange[0], axisYRange[1]);
    axisY->setLabelFormat("%1.1e");

    m_strategyChartRange = axisYRange;
    m_strategyLabelSeries = new QLineSeries;
    m_strategyLabelSeries->append(m_oldLabelIndex, m_strategyChartRange[0]);
    m_strategyLabelSeries->append(m_oldLabelIndex, m_strategyChartRange[1]);

    m_strategyChart = new QChart();
    m_strategyChart->addSeries(m_strategySeries);
    m_strategyChart->addSeries(m_preSpreadSeries);
    m_strategyChart->addSeries(m_strategyLabelSeries);

    m_strategyChart->legend()->hide();
    m_strategyChart->setAnimationOptions(QChart::NoAnimation);

    m_strategyChartView = new QMyChartView(m_strategyChart);
    m_strategyChartView->setRenderHint(QPainter::Antialiasing);
    m_strategyChartView->installEventFilter(this);
    m_strategyChartView->setMouseTracking(true);

    m_strategyChart->addAxis(axisX, Qt::AlignBottom);
    m_strategySeries->attachAxis(axisX);
    m_preSpreadSeries->attachAxis(axisX);
    m_strategyChart->addAxis(axisY, Qt::AlignLeft);
    m_strategySeries->attachAxis(axisY);
    m_preSpreadSeries->attachAxis(axisY);

    m_strategyLabelSeries->attachAxis(axisX);
    m_strategyLabelSeries->attachAxis(axisY);
}

void ChartForm::setVotRunoverChartView()
{
    QCategoryAxis *axisX = getAxisX();
    QValueAxis *axisY = new QValueAxis;
    QList<double> axisYRange = getChartYvalueRange(m_votList);
    axisY->setRange(0, axisYRange[1]);
    axisY->setLabelFormat("%1.1e");

    m_votLineSeries = new QLineSeries;
    for (int i = 0; i < m_votList.size(); ++i)
    {
        if (i % 2 == 0)
        {
            m_votLineSeries->append(i, 0);
            m_votLineSeries->append(i, m_votList[i]);
            m_votLineSeries->append(i, 0);
        }
        else
        {
            m_votLineSeries->append(i, 0);
            m_votLineSeries->append(i, m_votList[i]);
            m_votLineSeries->append(i, 0);
        }
    }

    m_votChartRange = axisYRange;
    m_votLabelSeries = new QLineSeries;
    m_votLabelSeries->append(m_votList.size()-1, 0);
    m_votLabelSeries->append(m_votList.size()-1, m_votChartRange[1]);

    m_votrunoverChart = new QChart();

    m_votrunoverChart->addSeries(m_votLabelSeries);
    m_votrunoverChart->addSeries(m_votLineSeries);

    m_votrunoverChart->setAnimationOptions(QChart::NoAnimation);
    m_votrunoverChart->legend()->hide();

    m_votrunoverChartView = new QMyChartView(m_votrunoverChart, this);
    m_votrunoverChartView->setRenderHint(QPainter::Antialiasing);
    m_votrunoverChartView->installEventFilter(this);
    m_votrunoverChartView->setMouseTracking(true);

    m_votrunoverChart->addAxis(axisX, Qt::AlignBottom);
    m_votrunoverChart->addAxis(axisY, Qt::AlignLeft);

    m_votLabelSeries->attachAxis(axisX);
    m_votLabelSeries->attachAxis(axisY);

    m_votLineSeries->attachAxis(axisX);
    m_votLineSeries->attachAxis(axisY);
}

void ChartForm::setMACDChartView()
{
    QCategoryAxis *axisX = getAxisX();
    QValueAxis *axisY = new QValueAxis;
    QList<double> axisYRange = getMACDRange(m_MACDList, 10);
    axisY->setRange(axisYRange[0], axisYRange[1]);
    axisY->setLabelFormat("%1.1e");

    m_macdUpLineSeries = new QLineSeries();
    m_macdDownLineSeries = new QLineSeries();
    for (int i = 0; i < m_MACDList.size(); ++i)
    {
        if (m_MACDList.at(i).m_macd > 0)
        {
            m_macdUpLineSeries->append(i, 0);
            m_macdUpLineSeries->append(i, m_MACDList.at(i).m_macd);
            m_macdUpLineSeries->append(i, 0);
            m_macdDownLineSeries->append(i, 0);
        }
        else
        {
            m_macdDownLineSeries->append(i, 0);
            m_macdDownLineSeries->append(i, m_MACDList.at(i).m_macd);
            m_macdDownLineSeries->append(i, 0);
            m_macdUpLineSeries->append(i, 0);
        }
    }

    m_diffSeries = new QLineSeries();
    m_diffSeries->setName("DIFF");
    m_deaSeries = new QLineSeries();
    m_deaSeries->setName("DEA");

    for (int i = 0; i < m_MACDList.size(); ++i)
    {
        m_diffSeries->append(i, m_MACDList.at(i).m_diff);
        m_deaSeries->append(i, m_MACDList.at(i).m_dea);
    }

    m_diffSeries->setUseOpenGL(true);
    m_deaSeries->setUseOpenGL(true);

    m_macdChartRange = axisYRange;
    m_macdLabelSeries = new QLineSeries;
    m_macdLabelSeries->append(m_MACDList.size() - 1, m_macdChartRange[0]);
    m_macdLabelSeries->append(m_MACDList.size() - 1, m_macdChartRange[1]);

    m_macdChart = new QChart();
    m_macdChart->addSeries(m_diffSeries);
    m_macdChart->addSeries(m_deaSeries);
    m_macdChart->addSeries(m_macdLabelSeries);

    m_macdChart->addSeries(m_macdUpLineSeries);
    m_macdChart->addSeries(m_macdDownLineSeries);

    m_macdChart->legend()->hide();
    m_macdChart->setAnimationOptions(QChart::NoAnimation);

    m_macdChartView = new QMyChartView(m_macdChart, this);
    m_macdChartView->setRenderHint(QPainter::Antialiasing);
    m_macdChartView->installEventFilter(this);
    m_macdChartView->setMouseTracking(true);

    m_macdChart->addAxis(axisX, Qt::AlignBottom);
    m_diffSeries->attachAxis(axisX);
    m_deaSeries->attachAxis(axisX);

    m_macdChart->addAxis(axisY, Qt::AlignLeft);
    m_diffSeries->attachAxis(axisY);
    m_deaSeries->attachAxis(axisY);

    m_macdLabelSeries->attachAxis(axisX);
    m_macdLabelSeries->attachAxis(axisY);

    m_macdUpLineSeries->attachAxis(axisX);
    m_macdUpLineSeries->attachAxis(axisY);
    m_macdDownLineSeries->attachAxis(axisX);
    m_macdDownLineSeries->attachAxis(axisY);
}

void ChartForm::initTheme()
{
    if (NULL != m_strategyChart)
    {
        m_strategyChart->setTheme(QChart::ChartThemeDark);
    }

    if (NULL != m_votrunoverChart)
    {
        m_votrunoverChart->setTheme(QChart::ChartThemeDark);
    }

    if (NULL != m_macdChart)
    {
        m_macdChart->setTheme(QChart::ChartThemeDark);
    }

    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0x121218));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    window()->setPalette(pal);
    ui->groupBox->setPalette(pal);
    ui->Title_Label->setStyleSheet(QStringLiteral("color: rgb(250, 250, 250);font: 75 14pt \"微软雅黑\";"));
}

void ChartForm::setLineColor()
{
    if (m_hedgedParam.m_isRealTime)
    {
        if (NULL != m_strategySeries)
        {
            m_strategySeries->setPen(QPen(QBrush(QColor(67, 160, 71)), 1));
        }
        if (NULL != m_diffSeries)
        {
            m_diffSeries->setPen(QPen(QBrush(QColor(220, 220, 220)), 1));
        }
        if (NULL != m_deaSeries)
        {
            m_deaSeries->setPen(QPen(QBrush(QColor(255, 255, 0)), 1));
        }
    }
    else
    {
        if (NULL != m_strategySeries)
        {
            m_strategySeries->setPen(QPen(QBrush(QColor(67, 160, 71)), 2));
        }
        if (NULL != m_diffSeries)
        {
            m_diffSeries->setPen(QPen(QBrush(QColor(220, 220, 220)), 2));
        }
        if (NULL != m_deaSeries)
        {
            m_deaSeries->setPen(QPen(QBrush(QColor(255, 255, 0)), 2));
        }
    }

    if (NULL != m_votLineSeries)
    {
        m_votLineSeries->setPen(QPen(QBrush(QColor(38, 166, 154)), 1));
    }

    if (NULL != m_macdUpLineSeries && NULL != m_macdDownLineSeries)
    {
        m_macdUpLineSeries->setPen(QPen(QBrush(QColor(216, 67, 21)), 1));
        m_macdDownLineSeries->setPen(QPen(QBrush(QColor(0, 229, 255)), 1));
    }

    if (NULL != m_strategyLabelSeries)
    {
        m_strategyLabelSeries->setPen(QPen(QBrush(QColor(255, 255, 255)), 1));
    }

    if (NULL != m_votLabelSeries)
    {
        m_votLabelSeries->setPen(QPen(QBrush(QColor(255, 255, 255)), 1));
    }

    if (NULL != m_macdLabelSeries)
    {
        m_macdLabelSeries->setPen(QPen(QBrush(QColor(255, 255, 255)), 1));
    }
}

void ChartForm::setPropertyValue(int currIndex)
{
    if (currIndex >= 0 && currIndex < m_earningList.size())
    {
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(qint64(m_timeData.at(currIndex)));
        QString dateTimeString = curDatetime.toString(m_timeTypeFormat);
        qreal strategyValue = m_earningList.at(currIndex);
        qreal votrunoverValue = m_votList.at(currIndex);
        qreal DIFF = m_MACDList.at(currIndex).m_diff;
        qreal DEA = m_MACDList.at(currIndex).m_dea;
        qreal Macd = m_MACDList.at(currIndex).m_macd;

        ui->TimeLabel->setText(QString("%1 %2").arg(QString("时间: ")).arg(dateTimeString));
        ui->StrategyValue_Label->setText(QString("%1 %2").arg(QString("点差: ")).arg(strategyValue));
        ui->VotRunover_Label->setText(QString("%1 %2").arg(QString("成交额: ")).arg(votrunoverValue));

        ui->DIFF_Label->setText(QString("DIFF: %1").arg(DIFF));
        ui->DEA_Label->setText(QString("DEA: %1").arg(DEA));
        ui->MACD_Label->setText(QString("MACD: %1").arg(Macd));
    }
}

void ChartForm::updateLableSeries(int index)
{
    if (index >= 0 && index < m_earningList.size())
    {
        if (NULL != m_strategyLabelSeries)
        {
            for (QPointF point : m_strategyLabelSeries->points())
            {
                m_strategyLabelSeries->remove(point);
            }

            m_strategyLabelSeries->append(index, m_strategyChartRange[0]);
            m_strategyLabelSeries->append(index, m_strategyChartRange[1]);
        }

        if (NULL != m_votLabelSeries)
        {
            // qDebug() <<"index: " << index << m_votLabelSeries->points();

            for (QPointF point : m_votLabelSeries->points())
            {
                m_votLabelSeries->remove(point);
            }
            m_votLabelSeries->append(index, 0);
            m_votLabelSeries->append(index, m_votChartRange[1]);
        }

        if (NULL != m_macdLabelSeries)
        {
            for (QPointF point : m_macdLabelSeries->points())
            {
                m_macdLabelSeries->remove(point);
            }
            m_macdLabelSeries->append(index, m_macdChartRange[0]);
            m_macdLabelSeries->append(index, m_macdChartRange[1]);
        }
    }
}

void ChartForm::mouseMoveEvenFunc(QObject *watched, QEvent *event)
{
    if (WidgetProcess::Instance().isProcessing())
    {
        return;
    }

    if (m_isKeyMove)
    {
        m_isKeyMove = false;
    }
    else
    {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QPoint curPoint = mouseEvent->pos();
        int currIndex = -1;
        if (watched == m_strategyChartView)
        {
            QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue(curPoint);
            currIndex = qFloor(curStrategyChartChartPoint.x());
        }
        if (watched == m_votrunoverChartView)
        {
            QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue(curPoint);
            currIndex = qFloor(curVotrunoverChartChartPoint.x());
        }
        if (watched == m_macdChartView)
        {
            QPointF curMacdChartChartPoint = m_macdChart->mapToValue(curPoint);
            currIndex = qFloor(curMacdChartChartPoint.x());
        }

        setPropertyValue(currIndex);
        updateLableSeries(currIndex);
    }
}

double ChartForm::getPointXDistance()
{
    int testIndex = 0;
    QPointF pointa = m_strategyChart->mapToPosition(QPointF(testIndex, m_earningList[testIndex]));
    QPointF pointb = m_strategyChart->mapToPosition(QPointF(testIndex + 1, m_earningList[testIndex + 1]));
    double distance = pointb.x() - pointa.x();
    return distance;
}

void ChartForm::KeyReleaseFunc(QEvent *event)
{
    if (WidgetProcess::Instance().isProcessing())
    {
        return;
    }

    QKeyEvent *keyEvent = (QKeyEvent *)event;
    int step = 0;

    if (keyEvent->key() == Qt::Key_Left)
    {
        step = -1;
    }
    if (keyEvent->key() == Qt::Key_Right)
    {
        step = 1;
    }

    moveMouse(step);
}

void ChartForm::moveMouse(int step)
{
    double pointXDistance = getPointXDistance();
    if (m_oldPointDistance != pointXDistance)
    {
        if (m_oldPointDistance != -1)
        {
            double deltaDistance = pointXDistance - m_oldPointDistance;
            m_mouseInitPos.setX(m_mouseInitPos.x() + deltaDistance);
        }
        m_oldPointDistance = pointXDistance;
    }

    if (step != 0)
    {
        m_keyMoveCount += step;
        m_currTimeIndex += step;
        float move_distance = pointXDistance * m_keyMoveCount;

        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeData.size())
        {
            m_isKeyMove = true;
            setPropertyValue(m_currTimeIndex);
            updateLableSeries(m_currTimeIndex);
        }

        if (move_distance >= 1 || move_distance <= -1 || move_distance == 0)
        {
            QCursor::setPos(m_mouseInitPos.x() + move_distance, m_mouseInitPos.y());
        }
    }
}

void ChartForm::mouseButtonReleaseFunc(QObject *watched, QEvent *event)
{
    if (WidgetProcess::Instance().isProcessing())
    {
        return;
    }

    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos();
    QPointF transPoint;
    m_currTimeIndex = -1;
    m_mouseInitPos = QCursor::pos();
    m_keyMoveCount = 0;
    double deltaInGlobalPointAndChartPoint = 0;
    if (watched == m_strategyChartView)
    {
        QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue(curPoint);
        m_currTimeIndex = qFloor(curStrategyChartChartPoint.x());
        transPoint = m_strategyChart->mapToPosition((QPointF(m_currTimeIndex, m_earningList[m_currTimeIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (watched == m_votrunoverChartView)
    {
        QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue(curPoint);
        m_currTimeIndex = qFloor(curVotrunoverChartChartPoint.x());
        transPoint = m_votrunoverChart->mapToPosition((QPointF(m_currTimeIndex, m_votList[m_currTimeIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }
    if (watched == m_macdChartView)
    {
        QPointF curMacdChartChartPoint = m_macdChart->mapToValue(curPoint);
        m_currTimeIndex = qFloor(curMacdChartChartPoint.x());
        transPoint = m_macdChart->mapToPosition((QPointF(m_currTimeIndex, m_MACDList[m_currTimeIndex].m_macd)));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }

    if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeData.size())
    {
        m_isKeyMove = true;
        setPropertyValue(m_currTimeIndex);
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
    }
}

void ChartForm::closeEvent(QCloseEvent *event)
{
    event;
    if (!m_isclosed)
    {
        emit windowClose_signal(m_windowID, m_windowName);
        m_isclosed = true;
    }
}

void ChartForm::writeExcelData()
{
    QStringList timeStringList;
    QStringList strategyValueList;
    QList<QStringList> excelData;
    for (int i = 0; i < m_timeData.size(); ++i)
    {
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch(m_timeData[i]);
        timeStringList.append(tmpDatetime.toString(m_timeTypeFormat));
        strategyValueList.append(QString("%1").arg(m_earningList[i]));
    }
    QString fileName = "D:/excel/cplus/hedged.xlsx";
    excelData.append(timeStringList);
    excelData.append(strategyValueList);
    int result = writeMatrixData(fileName, excelData, "Sheet1", true);
    qDebug() << "writeExcelData: " << result;
}

QList<QStringList> ChartForm::getExcelData(QStringList choosenKeyValueList)
{
    QList<QStringList> excelData;
    QStringList timeData;
    timeData.append("时间");
    for (int i = 0; i < m_timeData.size(); ++i)
    {
        timeData.append(QDateTime::fromMSecsSinceEpoch(m_timeData[i]).toString(m_timeTypeFormat));
    }
    excelData.append(timeData);

    if (choosenKeyValueList.indexOf("对冲点差值") >= 0)
    {
        excelData.append(transNumbData("对冲点差值", m_earningList));
    }

    if (choosenKeyValueList.indexOf("沪深300指数") >= 0)
    {
        excelData.append(transNumbData("沪深300指数", m_indexCodeData));
    }

    if (choosenKeyValueList.indexOf("成交额") >= 0)
    {
        excelData.append(transNumbData("成交额", m_votList));
    }

    if (choosenKeyValueList.indexOf("DIFF") >= 0)
    {
        QStringList tmpData;
        tmpData.append("DIFF");
        for (int i = 0; i < m_MACDList.size(); ++i)
        {
            tmpData.append(QString("%1").arg(m_MACDList[i].m_diff));
        }
        excelData.append(tmpData);
    }

    if (choosenKeyValueList.indexOf("DEA") >= 0)
    {
        QStringList tmpData;
        tmpData.append("DEA");
        for (int i = 0; i < m_MACDList.size(); ++i)
        {
            tmpData.append(QString("%1").arg(m_MACDList[i].m_dea));
        }
        excelData.append(tmpData);
    }

    if (choosenKeyValueList.indexOf("MACD") >= 0)
    {
        QStringList tmpData;
        tmpData.append("MACD");
        for (int i = 0; i < m_MACDList.size(); ++i)
        {
            tmpData.append(QString("%1").arg(m_MACDList[i].m_macd));
        }
        excelData.append(tmpData);
    }
    return excelData;
}

QString ChartForm::getExcelFileName(QStringList keyValueList, QString fileDir)
{
    QString fileName = fileDir;
    fileName += QString("%1_%2").arg(m_hedgedParam.m_portfolioName).arg(m_hedgedParam.m_timeType);
    if (m_hedgedParam.m_isRealTime)
    {
        fileName += QString("_%1_实时数据").arg(QDate::currentDate().toString("yyyyMMdd"));
    }
    else
    {
        fileName += QString("_%1_%2").arg(m_hedgedParam.m_startDate).arg(m_hedgedParam.m_endDate);
    }
    fileName += QString("_%1.xlsx").arg(keyValueList.join("_"));
    return fileName;
}