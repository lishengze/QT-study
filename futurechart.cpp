#include <QLineSeries>
#include <QBarSeries>
#include <QValueAxis>
#include <QDebug>
#include <QtMath>
#include <QMessageBox>
#include <QFont>

#include "futurechart.h"
#include "ui_futurechart.h"
#include "time_func.h"
#include "toolfunc.h"
#include "widget_func.h"
#include "process_data_func.h"
#pragma execution_character_set("utf-8")

FutureChart::FutureChart(int chartViewID, QString dbhost, QTableView* programInfoTableView,
                         QString futureName, double maxSpreadValue, double minSpreadValue,
                         QWidget *parent) :
    m_chartViewID(chartViewID), m_dbhost(dbhost), m_programInfoTableView(programInfoTableView),
    m_futureName(futureName), m_maxSpreadValue(maxSpreadValue), m_minSpreadValue(minSpreadValue),
    BaseChart(chartViewID, parent),
    ui(new Ui::FutureChart)
{
    ui->setupUi(this);

    initCommonData();
}

void FutureChart::initCommonData() {
    m_chartXaxisTickCount = 5;
    m_updateTime = 3000;

    m_visibleBoxNumb = 3;
    m_allBoxNumb = 0;

    m_keyMoveCount = 0;
    m_currTimeIndex = 0;
    m_isKeyMove = false;
    m_mouseInitPos = QPoint(-1, -1);
    m_oldPointDistance = -1;

    m_oldLabelIndex = 0;

    initMonitorWorker();
    registSignalParamsType();
    initExtractKeyValueList();

    if (isTradingStart()) {
        emit getHistFutureData_signal();
    } else {
        initLayout();
        m_monitorWorker->startTimer();
    }
}

void FutureChart::initMonitorWorker() {
    m_monitorWorker = new MonitorRealTimeData(m_dbhost, m_updateTime, m_futureName);

    connect(this, SIGNAL(getHistFutureData_signal()),
            m_monitorWorker, SLOT(getHistFutureData_slot()));

    connect(m_monitorWorker, SIGNAL(sendTradeOver()),
            this, SLOT(tradeOver_slot()));

    connect(m_monitorWorker, SIGNAL(sendHistFutureData_signal(QList<double>)),
            this, SLOT(sendHistFutureData_slot(QList<double>)));

    connect(m_monitorWorker, SIGNAL(sendRealtimeFutureData_signal(QList<double>)),
            this, SLOT(sendRealtimeFutureData_slot(QList<double>)));

    m_monitorWorker->moveToThread(&m_MonitorThread);

    connect(&m_MonitorThread, SIGNAL(finished()),
            m_monitorWorker, SLOT(deleteLater()));

    m_MonitorThread.start();
}

void FutureChart::registSignalParamsType() {
    qRegisterMetaType<QList<double>>("QList<double>");
}

void FutureChart::initExtractKeyValueList() {
    m_extractKeyValueList.append("基差");
}

QList<QMyChartView*> FutureChart::getChartViewList() {
    QList<QMyChartView*> result;
    result.append(m_spreadChartView);
    return result;
}

QString FutureChart::getExcelFileName(QStringList keyValueList, QString fileDir) {
    QString fileName = fileDir;
    fileName += QString("%1_%2_实时_%3.xlsx").arg(m_futureName)
                                            .arg(QDate::currentDate().toString("yyyyMMdd"))
                                            .arg(keyValueList.join("_"));
    return fileName;
}

QList<QStringList> FutureChart::getExcelData(QStringList choosenKeyValueList) {
    QList<QStringList> excelData;
    QStringList timeData;
    timeData.append("时间");
    for (int i = 0; i < m_futureTime.size(); ++i) {
        timeData.append(m_futureTime[i].toString("hh:mm:ss"));
    }
    excelData.append(timeData);

    if (choosenKeyValueList.indexOf("基差") >= 0 ) {
        excelData.append(transNumbData("基差", m_futureSpread));
    }
    return excelData;
}

void FutureChart::initLayout() {
    this->setWindowTitle(m_futureName);
    ui->title_label->setText(QString("期货 %1, %2 日基差实时图")
                             .arg(m_futureName).arg(QDate::currentDate().toString("yyyy-MM-dd")));
    initChartView();

    initTableContextMenu();

    if (m_spreadChartView != NULL) {
        ui->chart_gridLayout->addWidget(m_spreadChartView, 1, 0);
    }
    this->setMouseTracking(true);

    initTheme();
    setLineColor();
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

void FutureChart::monitorSpread(double spread, QDateTime time) {
    if (spread > m_maxSpreadValue || spread < m_minSpreadValue) {
        QString msg = QString("%1 基差: %2, 时间: %3")
                                            .arg(m_futureName).arg(spread)
                                            .arg(time.toString("hh:mm:ss"));
        updateProgramInfo(m_programInfoTableView, msg);
        QMessageBox* box = new QMessageBox();
        box->setText(msg);
        box->setStyleSheet("color:red");
        QFont font;
        font.setPixelSize(48);
        box->setFont(font);
        box->setFixedSize(400, 400);
        box->setWindowTitle(QString("%1 基差提醒!").arg(m_futureName));
        box->setWindowFlags(Qt::WindowStaysOnTopHint);
        box->show();

        m_monitorBoxList.append(box);
        while (m_monitorBoxList.size() > m_visibleBoxNumb) {
            QMessageBox* tmpBox = m_monitorBoxList.first();
            if (NULL != tmpBox) {
                tmpBox -> close();
                delete tmpBox;
            }
            m_monitorBoxList.pop_front();
        }
        qDebug() << "m_allBoxNumb: " << ++m_allBoxNumb;
    }
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

void FutureChart::sendRealtimeFutureData_slot (QList<double> realtimeFutureData) {
//    qDebug() << realtimeFutureData;
    monitorSpread(realtimeFutureData[0], transIntDateTime(realtimeFutureData[1]));
    m_futureSpread.append(realtimeFutureData[0]);
    m_futureTime.append(transIntDateTime(realtimeFutureData[1]));

    if (!m_isKeyMove) {
        setPropertyValue(m_futureSpread.size()-1);
    }

    updateAxis();
    updateSeries();
    updateMousePos();
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
            if (isStockTrading(latestDateTime.time())) {
                result.append(latestDateTime);
            } else {
                break;
            }
//            result.append(latestDateTime);
        }
    }
    return result;
}

QCategoryAxis* FutureChart::getAxisX (QList<QDateTime> m_valueList, int tickCount) {
    QCategoryAxis* axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(m_valueList.size (), tickCount);
//    qDebug() << "axisXPosList: " << axisXPosList;
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList[i];
        axisX->append (m_valueList[xpos].toString("hh:mm:ss"), xpos);
    }
    axisX->setMax(m_valueList.size()-1);
    return axisX;
}

void FutureChart::initChartView() {
    m_extendedFutureTime = m_futureTime;
    QCategoryAxis* axisX = getAxisX(m_extendedFutureTime, m_chartXaxisTickCount);
    QValueAxis* axisY = new QValueAxis;
    m_spreadLineSeries = new QLineSeries;

    for (int i = 0; i < m_futureSpread.size(); ++i) {
        m_spreadLineSeries->append(i, m_futureSpread[i]);
    }

    m_spreadLineSeries->setName(QString("基差"));
    m_spreadLineSeries->setUseOpenGL(true);

    QList<double> axisYRange = getChartYvalueRange(m_futureSpread);
    m_labelRange = axisYRange;
    axisY -> setRange (axisYRange[0], axisYRange[1]);
    m_labelSeries = new QLineSeries;
    m_labelSeries->append(m_oldLabelIndex, m_labelRange[0]);
    m_labelSeries->append(m_oldLabelIndex, m_labelRange[1]);

    m_spreadChart = new QChart();
    m_spreadChart->addSeries(m_spreadLineSeries);
    m_spreadChart->addSeries(m_labelSeries);
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
    m_labelSeries->attachAxis(axisX);
    m_labelSeries->attachAxis(axisY);
}

void FutureChart::setLineColor() {
    m_labelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
    m_spreadLineSeries->setPen(QPen(QBrush(QColor(67,160,71)), 1));
}

void FutureChart::updateAxis() {
    QList<double> axisYRange = getChartYvalueRange(m_futureSpread);
    QValueAxis* curYAxis = dynamic_cast<QValueAxis *>(m_spreadChart->axisY());
    curYAxis-> setRange (axisYRange[0], axisYRange[1]);
    m_labelRange = axisYRange;

    if (m_futureTime.size() > m_extendedFutureTime.size()) {
        m_spreadChart->removeAxis(m_spreadChart->axisX());
        m_extendedFutureTime = getExtendedFutureTime(m_futureTime, m_chartXaxisTickCount, m_updateTime);
        QCategoryAxis* newAxisX = getAxisX(m_extendedFutureTime, m_chartXaxisTickCount);
        m_spreadChart->addAxis(newAxisX, Qt::AlignBottom);
        m_spreadLineSeries->attachAxis(newAxisX);
        m_labelSeries->attachAxis(newAxisX);
    }
}

void FutureChart::updateSeries() {
    m_spreadLineSeries->append(m_futureSpread.size()-1, m_futureSpread.back());
}



void FutureChart::updateMousePos() {
    moveMouse(0);
}

void FutureChart::setPropertyValue(int index) {
    if (index >=0 && index < m_futureSpread.size()) {
        ui->time_label->setText(QString("时间: %1")
                                .arg(m_futureTime[index].toString("hh:mm:ss")));

        ui->spread_label->setText(QString("基差: %1")
                                  .arg(m_futureSpread[index]));

    }
}

void FutureChart::updateLabelSeries(int index) {
    if (index >=0 && index < m_futureSpread.size()) {
        QList<QPointF> pointList = m_labelSeries->points();
        for (QPointF point: pointList) {
            m_labelSeries->remove(point);
        }
        m_labelSeries->append(index, m_labelRange[0]);
        m_labelSeries->append(index, m_labelRange[1]);
    }

}

void FutureChart::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    if (m_isKeyMove)  {
        m_isKeyMove = false;
    } else {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QPoint curPoint = mouseEvent->pos ();
        int currIndex = -1;
        if (watched == m_spreadChartView) {
            QPointF currChartPoint = m_spreadChart->mapToValue (curPoint);
            currIndex = qFloor(currChartPoint.x());
        }

        if (currIndex>=0 && currIndex < m_futureSpread.size()) {
            setPropertyValue(currIndex);
            updateLabelSeries(currIndex);
        }
    }
}

void FutureChart::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    QPointF transPoint;
    m_currTimeIndex = -1;
    m_mouseInitPos = QCursor::pos();
    m_keyMoveCount = 0;
    double deltaInGlobalPointAndChartPoint = 0;

    if (watched == m_spreadChartView) {
        QPointF currChartPoint = m_spreadChart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(currChartPoint.x());
        transPoint = m_spreadChart->mapToPosition( (QPointF(m_currTimeIndex, m_futureSpread[m_currTimeIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }

    if (m_currTimeIndex >= 0 && m_currTimeIndex < m_futureSpread.size()) {
        m_isKeyMove = true;
        setPropertyValue(m_currTimeIndex);
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
    }
}

void FutureChart::KeyReleaseFunc(QEvent *event) {
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

void FutureChart::moveMouse(int step) {
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

        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_futureSpread.size()) {
            m_isKeyMove = true;
            setPropertyValue(m_currTimeIndex);
            updateLabelSeries(m_currTimeIndex);
        }

        if (move_distance >= 1 || move_distance <=-1 || move_distance == 0) {
            QCursor::setPos(m_mouseInitPos.x() + move_distance, m_mouseInitPos.y());
        }
    }
}

double FutureChart::getPointXDistance() {
    int testIndex = 0;
    QPointF pointa = m_spreadChart->mapToPosition(QPointF(testIndex, m_futureSpread[testIndex]));
    QPointF pointb = m_spreadChart->mapToPosition(QPointF(testIndex+1, m_futureSpread[testIndex+1]));
    double distance = pointb.x() - pointa.x();
    return distance;
}

void FutureChart::closeEvent(QCloseEvent *event) {
    event;
    if (NULL != m_monitorWorker) {
        m_monitorWorker->stopTimer();
    }
    emit windowClose_signal(m_chartViewID);
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

    for (int i = 0; i < m_monitorBoxList.size(); ++i) {
        if (NULL != m_monitorBoxList[i]) {
            delete m_monitorBoxList[i];
            m_monitorBoxList[i] = NULL;
        }
    }
}
