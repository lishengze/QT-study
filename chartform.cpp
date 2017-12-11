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
                     int EVA1Time, int EVA2Time, int DIFFTime,
                     QString databaseName):
    QWidget(parent), m_chartViewID(chartViewID),
    m_startDate(startDate), m_endDate(endDate), m_timeType(timeType),
    m_strategy(strategyList), m_strategyName(strategyName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_EVA1Time(EVA1Time), m_EVA2Time(EVA2Time), m_DIFFTime(DIFFTime),
    m_database(NULL), m_macdTooltip(NULL), m_strategyTooltip(NULL), m_votrunoverTooltip(NULL),
    ui(new Ui::ChartForm)
{
    initData(databaseName, timeType);
    setLayout();
}

void ChartForm::registSignalParamsType () {
    qRegisterMetaType<QList<int>>("QList<int>");
}

void ChartForm::receiveProcessedData (QList<int> subThreadData) {

}

void ChartForm::initData (QString databaseName, QString timeType) {
    m_dbhost = "192.168.211.165";
//    m_dbhost = "localhost";
    m_database = new Database(this, QString(m_chartViewID), m_dbhost);
    m_databaseName = databaseName + "_" + timeType;

    if (m_timeType.contains("m") && m_timeType != "month") {
        m_timeTypeFormat = "yyyy-MM-dd h:mm";
    } else {
        m_timeTypeFormat = "yyyy-MM-dd";
    }

    m_chartXaxisTickCount = 10;
    setStrategyData();
    setVotRunoverData();
    setMacdData();
}

void ChartForm::setLayout () {
    ui->setupUi(this);
    m_title = QString("策略: %1 , MACD: %2, %3, %4 ").arg(m_strategyName).arg(m_EVA1Time).arg(m_EVA2Time).arg (m_DIFFTime);

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

void ChartForm::setStrategyData() {
    QList<QList<QPointF>> tableDataList;
    QList<int> buyCountList;
    for (int i = 0; i < m_strategy.size (); ++i) {
        QString tableName = m_strategy[i].m_secode;
        int buyCount = m_strategy[i].m_buyCount;
        QList<QPointF> curTableData = m_database->getOriChartData (m_startDate, m_endDate, "TCLOSE", tableName, m_databaseName);
        tableDataList.append (sortPointFList(curTableData));
        buyCountList.append (buyCount);
        qDebug() << "tableName: " << tableName << " datacount: " << curTableData.size ();
    }
    m_strategyData = computeStrategyData(tableDataList, buyCountList);
}

void ChartForm::setVotRunoverData() {
    QList<QList<QPointF>> tableDataList;
    QList<int> buyCountList;
    for (int i = 0; i < m_strategy.size (); ++i) {
        QString tableName = m_strategy[i].m_secode;
        int buyCount = m_strategy[i].m_buyCount;
        QList<QPointF> curTableData = m_database->getOriChartData (m_startDate, m_endDate, "VOTRUNOVER", tableName, m_databaseName);
        tableDataList.append (sortPointFList(curTableData));
        buyCountList.append (1);
        qDebug() << "tableName: " << tableName << " datacount: " << curTableData.size ();
    }
    m_votrunoverData = computeStrategyData(tableDataList, buyCountList);
}

void ChartForm::setMacdData () {
    QList<double> oriData;
    for (int i = 0; i<m_strategyData.size (); ++i) {
        oriData.append (m_strategyData.at(i).y ());
    }
    m_macdData = computeMACD (oriData, m_EVA1Time, m_EVA2Time, m_DIFFTime);
}

void ChartForm::setStrategyChartView () {
    if (m_strategyData.size () == 0) {
        ErrorMessage ("No strategy Data!");
        return ;
    }    
    QCategoryAxis* axisX = new QCategoryAxis;
    QList<int> axisXPosList = getNumbList(m_strategyData.size (), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_strategyData.at(xpos).x());
        axisX->append (tmpDatetime.toString (m_timeTypeFormat), xpos);
    }
    QLineSeries* series = new QLineSeries;
    for (int i = 0; i < m_strategyData.size (); ++i) {
        series->append (i, m_strategyData.at (i).y());
    }
    series->setName("目标");
    series->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    QList<QPointF> pointList = series->points ();
    qDebug() << "StrategyChart points.count: " << pointList.count();

    QList<double>  axisYRange = getChartYvalueRange(pointList);
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
    if (m_votrunoverData.size () == 0) {
        ErrorMessage ("No strategy Data!");
        return;
    }

    QCategoryAxis* axisX = new QCategoryAxis;
    QList<int> axisXPosList = getNumbList(m_votrunoverData.size (), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_strategyData.at(xpos).x());
        axisX->append (tmpDatetime.toString (m_timeTypeFormat), xpos);
    }

    QStackedBarSeries *barseries = new QStackedBarSeries();
    QBarSet *set = new QBarSet("金额");
    for (int i = 0; i < m_votrunoverData.size (); ++i) {
        set->append(m_votrunoverData.at (i).y());
    }
    barseries->append(set);
    barseries->setUseOpenGL (true);

    QValueAxis *axisY = new QValueAxis;
    qDebug() << "VotRunoverChart points.count: " << m_votrunoverData.count();
    QList<double>  axisYRange = getChartYvalueRange(m_votrunoverData);
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
        ErrorMessage ("No strategy Data!");
        return;
    }
    qDebug() << "MacdChart points.count: " << m_macdData.size ();

    QLineSeries* diffSeries = new QLineSeries();
    diffSeries->setName("DIFF");
    QLineSeries* deaSeries = new QLineSeries();
    deaSeries->setName("DEA");
    QStackedBarSeries *macdSeries = new QStackedBarSeries();
    QBarSet *macdSet = new QBarSet("MACD");

    QCategoryAxis* axisX = new QCategoryAxis;
    QList<int> axisXPosList = getNumbList(m_macdData.size (), m_chartXaxisTickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList.at(i);
        QDateTime tmpDatetime = QDateTime::fromMSecsSinceEpoch (m_strategyData.at(xpos).x());
        axisX->append (tmpDatetime.toString (m_timeTypeFormat), xpos);
    }
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
        QDateTime curDatetime = QDateTime::fromMSecsSinceEpoch(m_strategyData.at(currIndex).x ());
        QString dateTimeString = curDatetime.toString (m_timeTypeFormat);
        qreal strategyValue = m_strategyData.at(currIndex).y ();
        qreal votrunoverValue = m_votrunoverData.at(currIndex).y ();
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

QList<QPointF> ChartForm::computeStrategyData(QList<QList<QPointF>> allTableData, QList<int> buyCountList) {
    QList<QPointF> result;
    QList<QPointF> firstStockData = allTableData.at(0);
    int firstBuyCount = buyCountList.at(0);
    for (int i = 0; i < firstStockData.size (); ++i) {
        result.append (QPointF(firstStockData.at (i).x(), firstStockData.at(i).y () * firstBuyCount));
    }

    for (int i = 1; i < allTableData.size (); ++i) {
        QList<QPointF> tmpStockData = allTableData.at(i);
        int tmpBuyCount = buyCountList.at(i);
        result = mergeSortedPointedList (result, 1, tmpStockData, tmpBuyCount);
    }
    return result;
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

ChartForm::~ChartForm()
{
    delete ui;
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
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
}

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
