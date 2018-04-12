#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QDebug>

#include "futurechart.h"
#include "ui_futurechart.h"
#include "toolfunc.h"

FutureChart::FutureChart(int chartViewID, QTableView* programInfoTableView,
                         QString futureName, double maxSpreadValue, double minSpreadValue,
                         QWidget *parent) :
    m_chartViewID(chartViewID), m_programInfoTableView(programInfoTableView),
    m_futureName(futureName), m_maxSpreadValue(maxSpreadValue), m_minSpreadValue(minSpreadValue),
    QWidget(parent),
    ui(new Ui::FutureChart)
{
    ui->setupUi(this);

    initCommonData();
}

void FutureChart::initCommonData() {
    m_dbhost = "192.168.211.165";
    m_chartXaxisTickCount = 5;
    m_updateTime = 3000;
    initMonitorWorker();
    registSignalParamsType();

    if (isTradingStart()) {
        emit getHistFutureData_signal();
    } else {
        initLayout();
        m_monitorWorker->startTimer();
    }
}

void FutureChart::initMonitorWorker() {
    m_monitorWorker = new MonitorRealTimeData(QString("%1").arg(m_chartViewID),
                                              m_dbhost, m_futureName, m_updateTime);

    connect(this, SIGNAL(getHistFutureData_signal()),
            m_monitorWorker, SLOT(getHistFutureData_slot()));

    connect(m_monitorWorker, SIGNAL(sendTradeOver()),
            this, SLOT(tradeOver_slot()));

    connect(m_monitorWorker, SIGNAL(sendHistFutureData_signal(QList<double>)),
            this, SLOT(sendHistFutureData_slot(QList<double>)));

    connect(m_monitorWorker, SIGNAL(sendFutureData_signal(QList<double>)),
            this, SLOT(sendFutureData_slot(QList<double>)));

    m_monitorWorker->moveToThread(&m_MonitorThread);

    connect(&m_MonitorThread, SIGNAL(finished()),
            m_monitorWorker, SLOT(deleteLater()));

    m_MonitorThread.start();
}

void FutureChart::registSignalParamsType() {
    qRegisterMetaType<QList<double>>("QList<double>");
}

void FutureChart::initLayout() {
    this->setWindowTitle(m_futureName);
    ui->title_label->setText(QString::fromLocal8Bit("期货 %1, %2 日基差实时图")
                             .arg(m_futureName).arg(QDate::currentDate().toString("yyyy:MM:dd")));
    initSpreadChartView();

    if (m_spreadChartView != NULL) {
        ui->chart_gridLayout->addWidget(m_spreadChartView, 1, 0);
    }
    this->setMouseTracking(true);

    initTheme();
}

void FutureChart::initTheme() {
    m_spreadChart->setTheme(QChart::ChartThemeDark);
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0x121218));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    window()->setPalette(pal);
    ui->groupBox->setPalette(pal);
    ui->title_label->setStyleSheet(QStringLiteral("color: rgb(250, 250, 250);font: 75 14pt \"微软雅黑\";"));
}

void FutureChart::transOriFutureData(QList<double> histFutureData) {
    for (int i = 0; i < histFutureData.size(); i+=2) {
        m_futureSpread.append(histFutureData[i]);
        m_futureTime.append(transIntDateTime(histFutureData[i+1]));
    }
}

void FutureChart::sendHistFutureData_slot(QList<double> histFutureData) {
    qDebug() << "histFutureData.size: " << histFutureData.size();
    transOriFutureData(histFutureData);
    initLayout();
    m_monitorWorker->startTimer();
}

void FutureChart::sendFutureData_slot (QList<double> realtimeFutureData) {
    qDebug() << realtimeFutureData;
    m_futureSpread.append(realtimeFutureData[0]);
    m_futureTime.append(transIntDateTime(realtimeFutureData[1]));

    updateAxis();
    updateSeries();
}

void FutureChart::tradeOver_slot() {
    qDebug() << "FutureChart trade is over";
    m_monitorWorker->stopTimer();
}

QList<QDateTime> FutureChart::getExtendedFutureTime(QList<QDateTime> oriTime,
                                       int chartXaxisTickCount, int updateTime) {
    QList<QDateTime> result = oriTime;
    if (oriTime.size() >= chartXaxisTickCount) {
        double addedPercent = 1.0 / chartXaxisTickCount;
        int addedNumb = oriTime.size() * addedPercent;
        QDateTime latestDateTime = oriTime.back();
        for (int i = 0; i < addedNumb; ++i) {
            latestDateTime = latestDateTime.addMSecs(updateTime);
//            if (isStockTrading(latestDateTime.time())) {
//                result.append(latestDateTime);
//            } else {
//                break;
//            }
            result.append(latestDateTime);
        }
    }
    return result;
}

QCategoryAxis* FutureChart::getAxisX (QList<QDateTime> m_valueList, int tickCount) {
    QCategoryAxis* axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(m_valueList.size (), tickCount);
    qDebug() << "axisXPosList: " << axisXPosList;
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList[i];
        axisX->append (m_valueList[xpos].toString("hh:mm:ss"), xpos);
    }
    axisX->setMax(m_valueList.size());
    return axisX;
}

void FutureChart::initSpreadChartView() {
//    m_extendedFutureTime = getExtendedFutureTime(m_futureTime, m_chartXaxisTickCount, m_updateTime);
//    qDebug() << "m_extendedFutureTime.size(): " << m_extendedFutureTime.size()
//             << ", m_futureTime.size(): " << m_futureTime.size();
    m_extendedFutureTime = m_futureTime;

    QCategoryAxis* axisX = getAxisX(m_extendedFutureTime, m_chartXaxisTickCount);
    QValueAxis* axisY = new QValueAxis;
    m_spreadLineSeries = new QLineSeries;

    for (int i = 0; i < m_futureSpread.size(); ++i) {
        m_spreadLineSeries->append(i, m_futureSpread[i]);
    }

    m_spreadLineSeries->setName(QString::fromLocal8Bit("基差"));
    m_spreadLineSeries->setUseOpenGL(true);

    QList<double> axisYRange = getChartYvalueRange(m_futureSpread);
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    axisY -> setLabelFormat ("%1.1e");

    m_spreadChart = new QChart();
    m_spreadChart->addSeries(m_spreadLineSeries);
    m_spreadChart->legend()->hide();
    m_spreadChart->setAnimationOptions(QChart::NoAnimation);

    m_spreadChartView = new QMyChartView(m_spreadChart);
    m_spreadChartView->setRenderHint(QPainter::Antialiasing);
    m_spreadChartView->installEventFilter(this);
    m_spreadChartView->setMouseTracking(true);

    m_spreadChart->addAxis(axisX, Qt::AlignBottom);
    m_spreadLineSeries->attachAxis(axisX);
    m_spreadChart->addAxis (axisY, Qt::AlignLeft);
    m_spreadLineSeries->attachAxis (axisY);
}

void FutureChart::updateAxis() {
    QList<double> axisYRange = getChartYvalueRange(m_futureSpread);
    QValueAxis* curYAxis = dynamic_cast<QValueAxis *>(m_spreadChart->axisY());
    curYAxis-> setRange (axisYRange[0], axisYRange[1]);

    if (m_futureTime.size() > m_extendedFutureTime.size()) {
        m_spreadChart->removeAxis(m_spreadChart->axisX());
        m_extendedFutureTime = getExtendedFutureTime(m_futureTime, m_chartXaxisTickCount, m_updateTime);
        QCategoryAxis* newAxisX = getAxisX(m_extendedFutureTime, m_chartXaxisTickCount);
        m_spreadChart->addAxis(newAxisX, Qt::AlignBottom);
        m_spreadLineSeries->attachAxis(newAxisX);
    }
}

void FutureChart::updateSeries() {
    m_spreadLineSeries->append(m_futureSpread.size()-1, m_futureSpread.back());
}

void FutureChart::updateMousePos() {

}

bool FutureChart::eventFilter (QObject *watched, QEvent *event) {
    if (event->type () == QEvent::MouseMove) {
//        mouseMoveEvenFunc(watched, event);
    }
    if (event->type() == QEvent::KeyRelease) {
//        KeyReleaseFunc(event);
    }
    if (event->type() == QEvent::MouseButtonRelease) {
//        mouseButtonReleaseFunc(watched, event);
    }
    return QWidget::eventFilter (watched, event);
}

void FutureChart::closeEvent(QCloseEvent *event) {
    event;
    if (NULL != m_monitorWorker) {
        m_monitorWorker->stopTimer();
    }
}

FutureChart::~FutureChart()
{
    delete ui;
    m_MonitorThread.quit();
    m_MonitorThread.wait();

    if (NULL != m_monitorWorker) {
        m_monitorWorker->stopTimer();
        m_monitorWorker = NULL;
    }
}
