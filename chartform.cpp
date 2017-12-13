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

#include <QMetaType>
#include <QMutexLocker>

#include "chartform.h"
#include "ui_chartform.h"
#include "toolfunc.h"
#include "macd.h"
#include "callout.h"


//#pragma execution_character_set("utf-8")

ChartForm::ChartForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartForm)
{
    ui->setupUi(this);
//    QGridLayout* mainLayout = new QGridLayout(this);
//    mainLayout->addWidget (ui->gridLayoutWidget);
}

ChartForm::ChartForm(QWidget *parent, int chartViewID,
                     QString startDate, QString endDate, QString timeType,
                     QList<strategy_ceil> strategyList, QString strategyName,
                     QString hedgeIndexCode, int hedgeIndexCount,
                     QList<int> macdTime, int threadNumb,
                     QString databaseName):
    QWidget(parent), m_chartViewID(chartViewID),
    m_startDate(startDate), m_endDate(endDate), m_timeType(timeType),
    m_strategy(strategyList), m_strategyName(strategyName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),    
    m_macdTime(macdTime), m_readDataThreadCount(0), m_threadNumb(threadNumb),
    m_macdTooltip(NULL), m_strategyTooltip(NULL), m_votrunoverTooltip(NULL),
    ui(new Ui::ChartForm)
{
    registSignalParamsType();
    initData(databaseName, timeType, strategyList);
    startReadData ();
}

void ChartForm::registSignalParamsType () {
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
}

void ChartForm::initData (QString databaseName, QString timeType, QList<strategy_ceil> strategyList) {
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
    qDebug() << "oriThreadData: " << oriThreadData;
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

void ChartForm::receiveOriginalData (QMap<QString, QList<QStringList>> subThreadData) {
    QMutexLocker locker(&m_mutex);
    m_completeTableData.unite (subThreadData);
    ++m_readDataThreadCount;
    qDebug() << "ChartForm::receiveProcessedData: " << QThread::currentThreadId() << " dataNumb: " << subThreadData.size ();
    qDebug() << "m_readDataThreadCount: " << m_readDataThreadCount << "\n";

    if (m_readDataThreadCount == m_threadNumb) {
        releaseDataReaderSrc ();
        m_readDataThreadCount = 0;
        qDebug() << "m_completeTableData.size: " <<  m_completeTableData.size ();
        startProcessData();
        m_completeTableData.clear ();
    }
}

void ChartForm::startProcessData () {
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

//    setTestView();
//    if (NULL != m_testChartView) {
//        ui->gridLayout->addWidget (m_testChartView, 1, 0);
//    }

    this->setMouseTracking(true);
}

QCategoryAxis* ChartForm::getAxisX () {
    QCategoryAxis* axisX = new QCategoryAxis;
    QList<int> axisXPosList = getNumbList(m_timeData.size (), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_timeData[xpos]);
        axisX->append (tmpDatetime.toString (m_timeTypeFormat), xpos);
    }
    return axisX;
}

void ChartForm::setStrategyChartView () {
    if (m_strategyData.size () == 0) {
        QMessageBox::warning (this, "Waring", "净值点差曲线在所选时间范围内没有数据");
        return ;
    }    

    QCategoryAxis* axisX = getAxisX();
    QValueAxis *axisY = new QValueAxis;
    QLineSeries* series = new QLineSeries;
    for (int i = 0; i < m_strategyData.size (); ++i) {
        series->append (i, m_strategyData.at (i));
    }
    series->setName("点差");
    series->setUseOpenGL (true);

    QList<double>  axisYRange = getChartYvalueRange(m_strategyData);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_strategyChart = new QChart();
    m_strategyChart->addSeries (series);
    m_strategyChart->legend()->setAlignment(Qt::AlignTop);

    m_strategyChartView = new QMyChartView(m_strategyChart);
    m_strategyChartView->setRenderHint(QPainter::Antialiasing);
    m_strategyChartView->installEventFilter(this);
    m_strategyChartView->setMouseTracking(true);

    m_strategyChart->addAxis (axisX, Qt::AlignBottom);
    series->attachAxis (axisX);
    m_strategyChart->addAxis (axisY, Qt::AlignLeft);
    series->attachAxis (axisY);
}

void ChartForm::setVotRunoverChartView () {
    if (m_votData.size () == 0) {
        QMessageBox::warning (this, "Waring", "销售额曲线在所选时间范围内没有数据");
        return;
    }

    QCategoryAxis* axisX = getAxisX();
    QStackedBarSeries *barseries = new QStackedBarSeries();
    QBarSet *set = new QBarSet("金额");
    for (int i = 0; i < m_votData.size (); ++i) {
        set->append(m_votData.at (i));
    }
    barseries->append(set);
    barseries->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getChartYvalueRange(m_votData);
    axisY -> setRange (0, axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_votrunoverChart = new QChart();
    m_votrunoverChart->addSeries(barseries);

    m_votrunoverChart->legend()->setAlignment(Qt::AlignTop);

    m_votrunoverChartView = new QMyChartView(m_votrunoverChart, this);
    m_votrunoverChartView->setRenderHint(QPainter::Antialiasing);
    m_votrunoverChartView->installEventFilter(this);
    m_votrunoverChartView->setMouseTracking(true);

    m_votrunoverChart->addAxis (axisX, Qt::AlignBottom);
    barseries->attachAxis(axisX);

    m_votrunoverChart->addAxis (axisY, Qt::AlignLeft);
    barseries->attachAxis (axisY);

//    m_votrunoverChart->createDefaultAxes();
//    connect (series, SIGNAL(hovered(QPointF,bool)), this, SLOT(votRunoverToolTip(QPointF,bool)));
}

void ChartForm::setMACDChartView () {
    if (m_macdData.size () == 0) {
         QMessageBox::warning (this, "Waring", "MACD曲线在所选时间范围内没有数据");
        return;
    }

    QLineSeries* diffSeries = new QLineSeries();
    diffSeries->setName("DIFF");
    QLineSeries* deaSeries = new QLineSeries();
    deaSeries->setName("DEA");
    QStackedBarSeries *macdSeries = new QStackedBarSeries();
    QBarSet *macdSet = new QBarSet("MACD");

    QCategoryAxis* axisX = getAxisX();
    for (int i = 0; i < m_macdData.size (); ++i) {
        diffSeries->append (i, m_macdData.at(i).m_diff);
        deaSeries->append (i, m_macdData.at(i).m_dea);
        macdSet->append (m_macdData.at(i).m_macd);
    }
    macdSeries->append(macdSet);
    diffSeries->setUseOpenGL (true);
    deaSeries->setUseOpenGL (true);
    macdSeries->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    QList<double>  axisYRange = getMACDRange(m_macdData);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_macdChart = new QChart();
    m_macdChart->addSeries (diffSeries);
    m_macdChart->addSeries (deaSeries);
    m_macdChart->addSeries (macdSeries);
    m_macdChart->legend()->setAlignment(Qt::AlignTop);

    m_macdChartView = new QMyChartView(m_macdChart, this);
    m_macdChartView->setRenderHint(QPainter::Antialiasing);
    m_macdChartView->installEventFilter(this);
    m_macdChartView->setMouseTracking(true);

    m_macdChart->addAxis(axisX, Qt::AlignBottom);
    diffSeries->attachAxis(axisX);
    deaSeries->attachAxis (axisX);
    macdSeries->attachAxis (axisX);

    m_macdChart->addAxis(axisY, Qt::AlignLeft);
    diffSeries->attachAxis(axisY);
    deaSeries->attachAxis (axisY);
    macdSeries->attachAxis (axisY);
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
//        qDebug() << "currIndex: " << currIndex;
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(qint64(m_timeData.at(currIndex)));
        QString dateTimeString = curDatetime.toString (m_timeTypeFormat);
        qreal strategyValue = m_strategyData.at(currIndex);
        qreal votrunoverValue = m_votData.at(currIndex);
        qreal DIFF = m_macdData.at(currIndex).m_diff;
        qreal DEA = m_macdData.at(currIndex).m_dea;
        qreal Macd = m_macdData.at(currIndex).m_macd;

//        qDebug() << "dateTimeString: " << dateTimeString;
//        qDebug() << "strategyValue: " << strategyValue;
//        qDebug() << "votrunoverValue: " << votrunoverValue;
//        qDebug() << "DIFF: " << DIFF;
//        qDebug() << "DEA: " << DEA;
//        qDebug() << "Macd: " << Macd;

        QString timeChineseString = QStringLiteral("时间: ");
        QString strategyChineseString = "\347\233\256\346\240\207\345\200\274:";
//        QString strategyChineseString = QString::fromLocal8Bit("目标值: ");
//        QString strategyChineseString = tr("目标值: ");
//        QString strategyChineseString = QString::fromUtf8("目标值: ");
//        QString strategyChineseString = QStringLiteral("目标值: ");
        QString votRunoverChineseString = QString::fromLocal8Bit("金额: ");

        ui->TimeLabel->setText(QString("%1 %2").arg("时间: ").arg(dateTimeString));
        ui->StrategyValue_Label->setText(QString("%1 %2").arg(strategyChineseString).arg(strategyValue));
        ui->VotRunover_Label->setText(QString("%1 %2").arg("金额: ").arg(votrunoverValue));

        ui->DIFF_Label->setText(QString("DIFF: %1").arg(DIFF));
        ui->DEA_Label->setText(QString("DEA: %1").arg(DEA));
        ui->MACD_Label->setText(QString("MACD: %1").arg(Macd));
    }
}

bool ChartForm::eventFilter (QObject *watched, QEvent *event) {
    if (event->type () == QEvent::MouseMove) {
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
        setMouseMoveValue(currIndex);
    }
    return QWidget::eventFilter (watched, event);
}

QList<double> ChartForm::getChartYvalueRange(QList<QPointF> pointList ) {
    double maxValue = -1000000000000000000.0;
    double minValue = 10000000000000000000.0;
    for (int i = 0; i < pointList.size(); ++i) {
        maxValue = max(maxValue, pointList.at(i).y());
        minValue = min(minValue, pointList.at (i).y ());
    }

    int rangeInterval = 6;
    maxValue += (maxValue - minValue) / rangeInterval;
    minValue -= (maxValue - minValue) / rangeInterval;
    QList<double> result;
    result.append (minValue);
    result.append (maxValue);
    return result;
}

QList<double> ChartForm::getChartYvalueRange(QList<double> yValueList ) {
    double maxValue = -1000000000000000000.0;
    double minValue = 10000000000000000000.0;
    for (int i = 0; i < yValueList.size(); ++i) {
        maxValue = max(maxValue, yValueList[i]);
        minValue = min(minValue, yValueList[i]);
    }

    int rangeInterval = 6;
    maxValue += (maxValue - minValue) / rangeInterval;
    minValue -= (maxValue - minValue) / rangeInterval;
    QList<double> result;
    result.append (minValue);
    result.append (maxValue);
    return result;
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

//QList<QPointF> ChartForm::computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList) {
//    QList<QPointF> result;
//    QList<QPointF> firstStockData = allTableData.at(0);
//    int firstBuyCount = buyCountList.at(0);
//    for (int i = 0; i < firstStockData.size (); ++i) {
//        result.append (QPointF(firstStockData.at (i).x(), firstStockData.at(i).y () * firstBuyCount));
//    }

//    for (int i = 1; i < allTableData.size (); ++i) {
//        QList<QPointF> tmpStockData = allTableData.at(i);
//        int tmpBuyCount = buyCountList.at(i);
//        result = mergeSortedPointedList (result, 1, tmpStockData, tmpBuyCount);
//    }
//    return result;
//}

//void ChartForm::setStrategyData() {
//    QList<QList<QPointF>> tableDataList;
//    QList<int> buyCountList;
//    for (int i = 0; i < m_strategy.size (); ++i) {
//        QString tableName = m_strategy[i].m_secode;
//        int buyCount = m_strategy[i].m_buyCount;
//        QList<QPointF> curTableData = m_database->getOriChartData (m_startDate, m_endDate, "TCLOSE", tableName, m_databaseName);
//        tableDataList.append (sortPointFList(curTableData));
//        buyCountList.append (buyCount);
//        qDebug() << "tableName: " << tableName << " datacount: " << curTableData.size ();
//    }
//    m_strategyData = computeStrategyData(tableDataList, buyCountList);
//}

//void ChartForm::setVotRunoverData() {
//    QList<QList<QPointF>> tableDataList;
//    QList<int> buyCountList;
//    for (int i = 0; i < m_strategy.size (); ++i) {
//        QString tableName = m_strategy[i].m_secode;
//        int buyCount = m_strategy[i].m_buyCount;
//        QList<QPointF> curTableData = m_database->getOriChartData (m_startDate, m_endDate, "VOTRUNOVER", tableName, m_databaseName);
//        tableDataList.append (sortPointFList(curTableData));
//        buyCountList.append (1);
//        qDebug() << "tableName: " << tableName << " datacount: " << curTableData.size ();
//    }
//    m_votrunoverData = computeStrategyData(tableDataList, buyCountList);
//}

//void ChartForm::setMacdData () {
//    QList<double> oriData;
//    for (int i = 0; i<m_strategyData.size (); ++i) {
//        oriData.append (m_strategyData.at(i).y ());
//    }
//    m_macdData = computeMACD (oriData, m_EVA1Time, m_EVA2Time, m_DIFFTime);
//}

//    void resizeEvent(QResizeEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);

//    void macdToolTip(QPointF point, bool state);
//    void strategyToolTip(QPointF point, bool state);
//    void votRunoverToolTip(QPointF point, bool state);

//void ChartForm::strategyToolTip (QPointF point, bool state)
//{
//    qDebug() << "Strategy Hover";
//    if (m_strategyTooltip == 0)
//        m_strategyTooltip = new Callout(m_strategyChart);

//    if (state) {
//        int currIndex = qFloor (point.x());
//        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(m_strategyData.at(currIndex).x ());
//        QString xValue = curDatetime.toString ("yyyy-MM-dd h:mm");
//        double yValue = m_strategyData.at(currIndex).y ();
//        m_strategyTooltip->setText(QString("X: %1 \nY: %2 ").arg(xValue).arg(yValue));
//        m_strategyTooltip->setAnchor(point);
//        m_strategyTooltip->setZValue(11);
//        m_strategyTooltip->updateGeometry();
//        m_strategyTooltip->show();
//    } else {
//        m_strategyTooltip->hide();
//    }
//}

//void ChartForm::macdToolTip (QPointF point, bool state)
//{
//    if (m_macdTooltip == 0)
//        m_macdTooltip = new Callout(m_macdChart);

//    if (state) {
//        int currIndex = qFloor (point.x());
//        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(m_strategyData.at(currIndex).x ());
//        QString time = curDatetime.toString ("yyyy-MM-dd h:mm");
//        qreal DIFF = m_macdData.at(currIndex).m_diff;
//        qreal DEA = m_macdData.at(currIndex).m_dea;
//        qreal Macd = m_macdData.at(currIndex).m_macd;
//        QString text = QString("Time: %1 \nDIFF: %2\nDEA: %3\nMACD: %4").arg(time).arg(DIFF).arg (DEA).arg (Macd);
//        qDebug() << text;
//        m_strategyTooltip->setText(text);
//        m_macdTooltip->setAnchor(point);
//        m_macdTooltip->setZValue(11);
//        m_macdTooltip->updateGeometry();
//        m_macdTooltip->show();
//    } else {
//        m_macdTooltip->hide();
//    }
//}

//void ChartForm::votRunoverToolTip (QPointF point, bool state)
//{
//    if (m_votrunoverTooltip == 0)
//        m_votrunoverTooltip = new Callout(m_votrunoverChart);

//    if (state) {
//        int currIndex = qFloor (point.x());
//        qDebug() << "currIndex: " << currIndex;
//        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(m_votrunoverData.at(currIndex).x ());
//        QString xValue = curDatetime.toString ("yyyy-MM-dd h:mm");
//        qreal yValue = m_votrunoverData.at(currIndex).y ();
//        qDebug() << xValue << ", " << yValue;
//        m_strategyTooltip->setText(QString("X: %1 \nY: %2 ").arg(xValue).arg(yValue));
//        m_votrunoverTooltip->setAnchor(point);
//        m_votrunoverTooltip->setZValue(11);
//        m_votrunoverTooltip->updateGeometry();
//        m_votrunoverTooltip->show();
//    } else {
//        m_votrunoverTooltip->hide();
//    }
//}

//void ChartForm::setThemeBox () {
//    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
//    ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
//    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
//    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
//    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
//    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
//    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
//    ui->themeComboBox->addItem("Qt", QChart::ChartThemeQt);
//}

//void ChartForm::resizeEvent(QResizeEvent *event) {
//    qDebug() <<"m_strategyChartView size: " << m_strategyChartView->size();
//    qDebug() <<"m_votrunoverChartView size: " << m_votrunoverChartView->size();
//    qDebug() <<"m_macdChartView size: " << m_macdChartView->size();
//    m_votrunoverChartView->setFixedSize(m_strategyChartView->size().width(), m_strategyChartView->size().height() / 2);
//    m_macdChartView->setFixedSize(m_strategyChartView->size().width(), m_strategyChartView->size().height() / 2);
//    qDebug() << event;
//    qDebug() <<"m_strategyChartView size: " << m_strategyChartView->size();
//    qDebug() <<"m_votrunoverChartView size: " << m_votrunoverChartView->size();
//    qDebug() <<"m_macdChartView size: " << m_macdChartView->size();
//}

//void ChartForm::mouseMoveEvent (QMouseEvent *event) {
//    QPoint curPoint = event->pos ();
//    qDebug() << "ChartForm mouseMoveEvent!" << curPoint;
//    QPointF curStrategyChartChartPoint = m_strategyChart->mapToValue (curPoint);
//    qDebug() << "curStrategyChartChartPoint: " << curStrategyChartChartPoint;
//    QPointF curVotrunoverChartChartPoint = m_votrunoverChart->mapToValue (curPoint);
//    QPointF curMacdChartChartPoint = m_macdChart->mapToValue (curPoint);
//    qDebug() << "curVotrunoverChartChartPoint: " << curVotrunoverChartChartPoint;
//    qDebug() << "curMacdChartChartPoint: " << curMacdChartChartPoint;
//}

//void ChartForm::setOpenPriceChartView () {
//    QLineSeries* series = new QLineSeries();
//    series->append (m_database->getOriChartData (m_startDate, m_endDate, "TOPEN", "SH600000", m_databaseName));

//    QList<QPointF> datalist = series->points ();
//    qDebug() << "OpenPrice points.count: " << datalist.count();

//    m_openPriceChart = new QChart();
//    m_openPriceChart->addSeries (series);
//    m_openPriceChart->setTitle("Open Price Chart");
//    m_openPriceChart->legend()->hide();

//    m_openPriceChartView = new QChartView(m_openPriceChart);
//    m_openPriceChartView->setRenderHint(QPainter::Antialiasing);

//    QDateTimeAxis *axisX = new QDateTimeAxis;
//    axisX->setTickCount(10);
//    axisX->setFormat("yyyy-MM-dd h:mm");
//    axisX->setTitleText("Date");
//    m_openPriceChart->addAxis(axisX, Qt::AlignBottom);
//    series->attachAxis(axisX);

//    QValueAxis *axisY = new QValueAxis;
//    axisY->setTitleText("OPEN PRICE");
//    m_openPriceChart->addAxis(axisY, Qt::AlignLeft);
//    series->attachAxis(axisY);
//}

//void ChartForm::setTextItem () {
//    QGraphicsScene* testGraphicsScene = new QGraphicsScene(ui->gridLayout);

//    m_timeItem = new QGraphicsSimpleTextItem();
//    m_targetItem = new QGraphicsSimpleTextItem();
//    m_votrunoverItem = new QGraphicsSimpleTextItem();
//    m_diffItem = new QGraphicsSimpleTextItem();
//    m_deaItem = new QGraphicsSimpleTextItem();
//    m_macdItem = new QGraphicsSimpleTextItem();

//    qreal leftStartPos = this->size ().width () - 190;
//    qreal topStartPos = 20;
//    qreal vertialIntervel = 20;

////    qDebug() << m_strategyChart->size ();
//    qDebug() << m_strategyChartView->size ();
//    qDebug() << ui->gridLayout->sizeHint ();
//    qDebug() << this->size ();

//    m_timeItem->setPos (leftStartPos, topStartPos);
//    m_targetItem->setPos (leftStartPos, topStartPos+vertialIntervel*1);
//    m_votrunoverItem->setPos (leftStartPos, topStartPos+vertialIntervel*2);
//    m_diffItem->setPos (leftStartPos, topStartPos+vertialIntervel*3);
//    m_deaItem->setPos (leftStartPos, topStartPos+vertialIntervel*4);
//    m_macdItem->setPos (leftStartPos, topStartPos+vertialIntervel*5);

//    m_timeItem->setText ("Time: ");
//    m_targetItem->setText("Target: ");
//    m_votrunoverItem->setText ("Money: ");
//    m_diffItem->setText ("DIFF: ");
//    m_deaItem->setText ("DEA: ");
//    m_macdItem->setText ("MACD: ");

//    testGraphicsScene->addItem (m_timeItem);
//}
