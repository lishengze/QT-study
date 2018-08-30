#include "ui_indexchart.h"
#include "indexchart.h"
#include "process_data_func.h"
#include "io_func.h"
#include "compute_func.h"
#include "time_func.h"
#include <QtMath>
#include <QMessageBox>
#pragma execution_character_set("utf-8")

//IndexChart::IndexChart(QWidget *parent) :
//    BaseChart(parent),
//    ui(new Ui::IndexChart)
//{
//    ui->setupUi(this);
//}

IndexChart::IndexChart(int chartViewID, QString dbhost, QString databaseName,
            QString selectIndex, QString hedgedIndex,
            QString startDate, QString endDate,
            int aveNumb, double css12Rate,
            double cssRate1, double cssRate2,
            double maxCSS, double minCSS,
            QWidget *parent):
            m_chartViewID(chartViewID), m_dbhost(dbhost), m_databaseName(databaseName),
            m_selectIndex(selectIndex), m_hedgedIndex(hedgedIndex),
            m_startDate(startDate), m_endDate(endDate),
            m_aveNumb(aveNumb), m_csst12Rate(css12Rate),
            m_csstRate1(cssRate1), m_csstRate2(cssRate2),
            m_maxCSS(maxCSS), m_minCSS(minCSS),
            BaseChart(chartViewID, parent), ui(new Ui::IndexChart) {

    ui->setupUi(this);

    initCommonData();
    registSignalParamsType();
    initHistDataWorker();
    initMonitorWorker();
    initExtractKeyValueList();
    emit getIndexHistData_signal();
}

IndexChart::~IndexChart()
{
    delete ui;
    m_histDataThread.quit();
    m_histDataThread.wait();
    m_MonitorThread.quit();
    m_MonitorThread.wait();
}

void IndexChart::initCommonData() {
    m_chartXaxisTickCount = 5;
    m_updateTime = 3000;
    m_nextAddedTime = -1;
    m_isAddedTime = false;
    m_isKeyMove = false;
    m_keyMoveCount = 0;
    m_currTimeIndex = 0;
    m_mouseInitPos = QPoint(-1, -1);
    m_oldPointDistance = -1;

    m_testCSSValue = 80;
    setRealtimeFlag();
    setDataFrequency();
}

void IndexChart::setRealtimeFlag() {
    if (m_endDate == QDate::currentDate().toString("yyyyMMdd")) {
        m_isRealTime = true;
    } else {
        m_isRealTime = false;
    }
}

void IndexChart::setDataFrequency() {
    QStringList tmpdata = m_databaseName.split("_");
    QString timeInfo = tmpdata[1];
    m_timeType = timeInfo;
    int minuteNumb = -1;
    if (timeInfo != "day") {
        timeInfo.remove("m");
        minuteNumb = timeInfo.toInt();
    }
    m_timeFrequency = minuteNumb * 60 * 1000;
}

void IndexChart::initHistDataWorker() {
    m_histDataWorker = new HistoryData(m_chartViewID, m_dbhost, m_databaseName,
                                       m_selectIndex, m_hedgedIndex,
                                       m_startDate, m_endDate, m_isRealTime,
                                       m_aveNumb, m_csst12Rate,
                                       m_csstRate1, m_csstRate2,
                                       m_maxCSS, m_minCSS);

    connect(this, SIGNAL(getIndexHistData_signal()),
            m_histDataWorker, SLOT(getIndexHistData_slot()));

    connect(m_histDataWorker, SIGNAL(sendLatestHistIndexData_signal(QString, double, double, QList<int>)),
            this, SLOT(sendLatestHistIndexData_slot(QString, double, double, QList<int>)));

    connect(m_histDataWorker, SIGNAL(sendHistIndexData_signal(QList<QString>, QList<double>, QList<double>,
                                                               QList<double>,  QList<double>, QList<double>)),
            this, SLOT(sendHistIndexData_slot(QList<QString>, QList<double>, QList<double>,
                                               QList<double>,  QList<double>, QList<double>)));

    connect(m_histDataWorker, SIGNAL(sendHistIndexError_signal(QString)),
            this, SLOT(sendHistIndexError_slot(QString)));

    m_histDataWorker->moveToThread(&m_histDataThread);
    connect(&m_histDataThread, SIGNAL(finished()),
            m_histDataWorker, SLOT(deleteLater()));

    m_histDataThread.start();
}

void IndexChart::initMonitorWorker() {
    m_monitorWorker = new MonitorRealTimeData(QString("%1").arg(m_chartViewID+1),
                                              m_dbhost, m_selectIndex, m_hedgedIndex,
                                              m_updateTime);

    connect(m_monitorWorker, SIGNAL(sendTradeOver()),
            this, SLOT(tradeOver_slot()));

    connect(m_monitorWorker, SIGNAL(sendRealTimeIndexData_signal(QStringList, QStringList)),
            this, SLOT(sendRealTimeIndexData_slot(QStringList, QStringList)));

    connect(m_monitorWorker, SIGNAL(sendRealTimeIndexDataError_signal(QString)),
            this, SLOT(sendRealTimeIndexDataError_slot(QString)));

    m_monitorWorker->moveToThread(&m_MonitorThread);

    connect(&m_MonitorThread, SIGNAL(finished()),
            m_monitorWorker, SLOT(deleteLater()));

    m_MonitorThread.start();
}

void IndexChart::registSignalParamsType() {
    qRegisterMetaType<QList<double>>("QList<double>");
    qRegisterMetaType<QList<QString>>("QList<QString>");
}

void IndexChart::initExtractKeyValueList() {
    m_extractKeyValueList.append("累计净值");
    m_extractKeyValueList.append("CSS");
}

QList<QMyChartView*> IndexChart::getChartViewList() {
    QList<QMyChartView*> result;
    result.append(m_ChartView);
    return result;
}

QString IndexChart::getExcelFileName(QStringList keyValueList, QString fileDir) {
    QString fileName = fileDir;
    fileName += QString("%1_%2_%3_%4_%5_%6.xlsx").arg(m_selectIndex).arg(m_hedgedIndex).arg(m_timeType)
                                                 .arg(m_startDate).arg(m_endDate).arg(keyValueList.join("_"));
    return fileName;
}

QList<QStringList> IndexChart::getExcelData(QStringList choosenKeyValueList) {
    QList<QStringList> excelData;
    QStringList timeData = m_timeList;
    timeData.append("时间");
    excelData.append(timeData);

    if (choosenKeyValueList.indexOf("累计净值") >=0 ) {
        excelData.append(transNumbData("累计净值", m_earningList));
    }

    if (choosenKeyValueList.indexOf("CSS") >=0 ) {
        excelData.append(transNumbData("CSS", m_CSSList));
    }
    return excelData;
}

void IndexChart::updateNextAddedTime() {
    m_nextAddedTime = -1;
    for (int i = 0; i < m_standardTimeList.size(); ++i) {
        if (m_standardTimeList[i] > m_latestTime) {
            m_nextAddedTime = m_standardTimeList[i];
            break;
        }
    }
    if (m_nextAddedTime == -1) {
        m_nextAddedTime = m_standardTimeList.last();
    }
}

void IndexChart::sendLatestHistIndexData_slot(QString latestTime,
                                              double latestSelectIndexPrice,
                                              double latestHedgedIndexPrice,
                                              QList<int> standardTimeList) {
    m_latestTime = latestTime.split(" ").at(1).toInt();
    m_standardTimeList = standardTimeList;
    m_latestSelectIndexPrice = latestSelectIndexPrice;
    m_latestHedgedIndexPrice = latestHedgedIndexPrice;

    updateNextAddedTime();
    qDebug() << "m_latestTime: " << m_latestTime
             << ", latestSelectIndexPrice: " << latestSelectIndexPrice
             << ", latestHedgedIndexPrice: " << latestHedgedIndexPrice
             << ", m_nextAddedTime: " << m_nextAddedTime;
    //    printList(m_lastestSelectIndexEarningList, "m_lastestSelectIndexEarningList");
    //    printList(m_lastestHedgedIndexEarningList, "m_lastestHedgedIndexEarningList");
    qDebug() << "first";
}

void IndexChart::sendHistIndexData_slot(QList<QString> timeList, QList<double> earningList,
                                        QList<double>TypeList, QList<double>CSSTList,
                                        QList<double>preCSSList, QList<double> CSSList) {
    qDebug() << "second";
    m_timeList = timeList;
    m_earningList = earningList;
    m_typeList = TypeList;
    m_CSSTList = CSSTList;
    m_preCSSList = preCSSList;
    m_CSSList = CSSList;
    m_extendedTimeList = getExtendedFutureTime(m_timeList, m_chartXaxisTickCount, m_timeFrequency);
    //    printList(m_timeList, "m_timeList");
    //    printList(m_earningList, "m_earningList");
    //    printList(m_CSSList, "m_CSSList");

    initLayout();

//    m_monitorWorker->startTimer();

    if (isStockTrading() && m_isRealTime) {
        m_monitorWorker->startTimer();
    }
}

void IndexChart::sendHistIndexError_slot(QString errMsg) {
    QMessageBox::critical(NULL, "Error", QString("获取指数历史数据错误: %1").arg(errMsg));
}

void IndexChart::sendRealTimeIndexDataError_slot(QString errMsg)  {
    QMessageBox::critical(NULL, "Error", QString("获取指数实时数据错误: %1").arg(errMsg));
}

bool IndexChart::isAddedTime(int time) {
    bool result = false;
    if (time >= m_nextAddedTime && m_nextAddedTime != m_standardTimeList.last()) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

void IndexChart::computeNewValue(QStringList selectData, QStringList hedgedData) {
    double selectEarning = (selectData[2].toDouble() - m_latestSelectIndexPrice) / m_latestSelectIndexPrice;
    double hedgedEarning = (hedgedData[2].toDouble() - m_latestHedgedIndexPrice) / m_latestHedgedIndexPrice;
    double relativeEarning = selectEarning - hedgedEarning;
    double newAcmuEarning = m_earningList.last() * (1 + relativeEarning);
    m_earningList.append(newAcmuEarning);

    double newType = (newAcmuEarning + m_earningList[m_earningList.size()-2])/2;
    m_typeList.append(newType);
//    printList(getSubList(m_typeList, m_typeList.size()-m_aveNumb, m_typeList.size()), "m_typeList");
//    qDebug() << "m_typeList.size: " << m_typeList.size();
    double newCSST11 = newType - getAveValue(getSubList(m_typeList, m_typeList.size()-m_aveNumb, m_typeList.size()));
    double newCSST12 = m_csst12Rate * getAveDev(getSubList(m_typeList, m_typeList.size()-m_aveNumb,  m_typeList.size()));
    double newCSST13 = newCSST11 / newCSST12;
    double newCSST = m_CSSTList[m_CSSTList.size()-1] * m_csstRate1 + newCSST13 * m_csstRate2;
    m_CSSTList.append(newCSST);
    double newPreCSS = reboundValue(newCSST, m_maxCSS, m_minCSS);
    m_preCSSList.append(newPreCSS);
    double newCSSValue = (m_preCSSList[m_preCSSList.size()-1] + m_preCSSList[m_preCSSList.size() - 2]) / 2;
    m_CSSList.append(newCSSValue);
//    qDebug() << "relativeEarning: " << relativeEarning << "newAcmuEarning: " << newAcmuEarning
//             << ", newType: " << newType << ", newCSST11: " << newCSST11
//             << ", newCSST13: " << newCSST13 << ", newCSST: " << newCSST
//             << ", newPreCSS: " << newPreCSS << ", newCSSValue: " << newCSSValue;

}

void IndexChart::updateNewValue(QStringList selectData, QStringList hedgedData) {
    if (!m_isAddedTime) {
        m_timeList.pop_back();
        m_earningList.pop_back();
        m_typeList.pop_back();
        m_CSSTList.pop_back();
        m_preCSSList.pop_back();
        m_CSSList.pop_back();
    } else {
        m_latestSelectIndexPrice = selectData[2].toDouble();
        m_latestHedgedIndexPrice = hedgedData[2].toDouble();
    }
}

void IndexChart::sendRealTimeIndexData_slot(QStringList selectData, QStringList hedgedData) {
    m_isAddedTime = isAddedTime(selectData[1].toInt());
    if (m_isAddedTime) {
        m_latestTime = m_nextAddedTime;
        updateNextAddedTime();
    }
    m_newTime = QString("%1 %2").arg(selectData[0]).arg(m_latestTime);
    m_timeList.append(m_newTime);
    //    qDebug() << "isAddedTime: " << m_isAddedTime << ", m_nextAddedTime: " << m_nextAddedTime
    //             << ", m_latestTime: " << m_latestTime;
    computeNewValue(selectData, hedgedData);
    updateAxis();
    updateSeries();
    updateNewValue(selectData, hedgedData);
}

void IndexChart::tradeOver_slot() {
    m_monitorWorker->stopTimer();
}

void IndexChart::initLayout() {
    this->setWindowTitle(QString("%1").arg(m_selectIndex));
    ui->title_label->setText(QString("指数: %1 与指数: %2, %3频, 实时对冲图")
                             .arg(m_selectIndex).arg(m_hedgedIndex).arg(m_timeType));
    initChartView();

    initTableContextMenu();

    if (m_ChartView != NULL) {
        ui->chart_gridLayout->addWidget(m_ChartView, 1, 0);
    }
    this->setMouseTracking(true);

    initTheme();

    setLineColor();
}

void IndexChart::initTheme() {
    m_Chart->setTheme(QChart::ChartThemeDark);
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0x121218));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    window()->setPalette(pal);
    ui->groupBox->setPalette(pal);
    ui->title_label->setStyleSheet(QStringLiteral("color: rgb(250, 250, 250);font: 75 14pt \"微软雅黑\";"));
}

void IndexChart::initChartView() {
//    printList(m_extendedTimeList, "m_extendedTimeList");
    QCategoryAxis* timeAxisX = getTimeAxisX(m_extendedTimeList, m_chartXaxisTickCount);
    QValueAxis* earningAxisY = new QValueAxis;
    QValueAxis* CSSAxisY = new QValueAxis;

    m_earningLineSeries = new QLineSeries;
    m_CSSLineSeries = new QLineSeries;

    for (int i = 0; i < m_earningList.size(); ++i) {
        m_earningLineSeries->append(i, m_earningList[i]);
    }

    for (int i = m_aveNumb; i < m_CSSList.size(); ++i) {
        m_CSSLineSeries->append(i, m_CSSList[i]);
    }


    m_earningLineSeries->setName(QString("累计净值"));
    m_earningLineSeries->setUseOpenGL(true);

    m_CSSLineSeries->setName(QString("CSS"));
    m_CSSLineSeries->setUseOpenGL(true);

    QList<double> earningYRange = getChartYvalueRange(m_earningList);
    earningAxisY -> setRange (earningYRange[0], earningYRange[1]);

    QList<double> CSSYRange = getChartYvalueRange(m_CSSList);
    CSSAxisY -> setRange (CSSYRange[0], CSSYRange[1]);

    m_labelSeries = new QLineSeries;
    m_labelRange = earningYRange;
    m_labelSeries->append(0, m_labelRange[0]);
    m_labelSeries->append(0, m_labelRange[1]);

    m_Chart = new QChart();
    m_Chart->addSeries(m_earningLineSeries);
    m_Chart->addSeries(m_CSSLineSeries);
    m_Chart->addSeries(m_labelSeries);
//    m_Chart->legend()->hide();
    m_Chart->setAnimationOptions(QChart::NoAnimation);

    m_ChartView = new QMyChartView(m_Chart);
    m_ChartView->setRenderHint(QPainter::Antialiasing);
    m_ChartView->installEventFilter(this);
    m_ChartView->setMouseTracking(true);

    m_Chart->addAxis(timeAxisX, Qt::AlignBottom);
    m_Chart->addAxis (earningAxisY, Qt::AlignLeft);
    m_Chart->addAxis (CSSAxisY, Qt::AlignRight);

    m_labelSeries->attachAxis(timeAxisX);
    m_labelSeries->attachAxis(earningAxisY);

    m_earningLineSeries->attachAxis(timeAxisX);
    m_earningLineSeries->attachAxis(earningAxisY);
    m_CSSLineSeries->attachAxis(timeAxisX);
    m_CSSLineSeries->attachAxis(CSSAxisY);
}

void IndexChart::setLineColor() {
    m_labelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
}

QCategoryAxis* IndexChart::getTimeAxisX  (QList<QString> timeList, int tickCount) {
    QCategoryAxis* axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(timeList.size(), tickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList[i];
        axisX->append (timeList[xpos], xpos);
    }
    axisX->setMax(timeList.size()-1);
    return axisX;
}

QList<QString> IndexChart::getExtendedFutureTime(QList<QString> oriTime,
                                                 int chartXaxisTickCount, int updateTime) {
    QList<QString> result = oriTime;
    if (oriTime.size() >= chartXaxisTickCount) {
        double addedPercent = 1.0 / chartXaxisTickCount;
        int addedNumb = oriTime.size() * addedPercent;
        QDateTime latestDateTime = transQStringDateTime(oriTime.back());
        qDebug() << "latestDateTime: " << latestDateTime << ", updateTime: " << updateTime;
        for (int i = 0; i < addedNumb; ++i) {
            latestDateTime = latestDateTime.addMSecs(updateTime);
            if (isStockTrading(latestDateTime.time())) {
                result.append(latestDateTime.toString("yyyyMMdd HHmmss"));
            }
            if (isStockTradingOver(latestDateTime.time())) {
                latestDateTime = QDateTime(getNextTradingData(latestDateTime.date()), transIntTime(m_standardTimeList.first()));
                result.append(latestDateTime.toString("yyyyMMdd HHmmss"));
            }
        }
    }
    return result;
}

void IndexChart::updateAxis() {
    QValueAxis* earningAxisY = dynamic_cast<QValueAxis *>(m_Chart->axisY(m_earningLineSeries));
    if (m_earningList.last() > earningAxisY->max() || m_earningList.last() < earningAxisY->min()) {
        QList<double> earningYRange = getChartYvalueRange(m_earningList);
        m_labelRange = earningYRange;
        earningAxisY -> setRange (earningYRange[0], earningYRange[1]);
    }

    QValueAxis* CSSAxisY = dynamic_cast<QValueAxis *>(m_Chart->axisY(m_CSSLineSeries));
    if (m_CSSList.last() > CSSAxisY->max() || m_CSSList.last() < CSSAxisY->min()) {
        QList<double> CSSYRange = getChartYvalueRange(m_CSSList);
        CSSAxisY -> setRange (CSSYRange[0], CSSYRange[1]);
    }

    if (m_timeList.size() > m_extendedTimeList.size() && m_isAddedTime) {
        qDebug() << "update series";
        m_Chart->removeAxis(m_Chart->axisX());
        m_extendedTimeList = getExtendedFutureTime(m_timeList, m_chartXaxisTickCount, m_timeFrequency);
        QCategoryAxis* newAxisX = getTimeAxisX(m_extendedTimeList, m_chartXaxisTickCount);
        m_Chart->addAxis(newAxisX, Qt::AlignBottom);
        m_earningLineSeries->attachAxis(newAxisX);
        m_CSSLineSeries->attachAxis(newAxisX);
        m_labelSeries->attachAxis(newAxisX);
    }
}

void IndexChart::updateSeries() {
    if (m_isAddedTime) {
        qDebug() << "Update append";
        m_CSSLineSeries->append(m_CSSList.size()-1, m_CSSList.back());
        m_earningLineSeries->append(m_earningList.size()-1, m_earningList.back());

    } else {
    //        qDebug() << "Update Replace";
    //        double newCSSValue = m_CSSList.back();
    //        double newCSSValue = ++m_testCSSValue;
//        qDebug() <<"Before replace " << m_CSSLineSeries->points().last() << ", m_CSSList.size: " << m_CSSList.size()
//                 << "newCSSValue: " << newCSSValue;
        qDebug() <<"Before CSS:  " << m_CSSLineSeries->points().last()
                << ", Earning: " << m_earningLineSeries->points().last();

        m_CSSLineSeries->replace(m_CSSLineSeries->points().last().rx(), m_CSSLineSeries->points().last().ry(),
                                 m_CSSLineSeries->points().last().rx(), m_CSSList.last());
        m_earningLineSeries->replace(m_earningLineSeries->points().last().rx(), m_earningLineSeries->points().last().ry(),
                                     m_earningLineSeries->points().last().rx(), m_earningList.last());

        qDebug() <<"After  CSS:  " << m_CSSLineSeries->points().last()
                << ", Earning: " << m_earningLineSeries->points().last();
    }
    qDebug() << m_timeList.last();
    setPropertyValue(m_earningList.size()-1);
}

void IndexChart::updateMousePos() {
    moveMouse(0);
}

void IndexChart::setPropertyValue(int index) {
    if (index >=0 && index < m_timeList.size()) {
        ui->time_label->setText(QString("时间: %1")
                                .arg(m_timeList[index]));

        ui->acmuEarning_label->setText(QString("累计净值: %1")
                                  .arg(m_earningList[index]));

        ui->cssValue_label->setText(QString("CSS: %1")
                                  .arg(m_CSSList[index]));
    }
}

void IndexChart::updateLabelSeries(int index) {
    if (index >=0 && index < m_timeList.size()) {
        for (QPointF point: m_labelSeries->points()) {
            m_labelSeries->remove(point);
        }
        m_labelSeries->append(index, m_labelRange[0]);
        m_labelSeries->append(index, m_labelRange[1]);
    }
}

void IndexChart::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    if (m_isKeyMove)  {
        m_isKeyMove = false;
    } else {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QPoint curPoint = mouseEvent->pos ();
        int currIndex = -1;
        if (watched == m_ChartView) {
            QPointF currChartPoint = m_Chart->mapToValue (curPoint);
            currIndex = qFloor(currChartPoint.x());
        }

        setPropertyValue(currIndex);
        updateLabelSeries(currIndex);
    }
}

void IndexChart::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    QPointF transPoint;
    m_currTimeIndex = -1;
    m_mouseInitPos = QCursor::pos();
    m_keyMoveCount = 0;
    double deltaInGlobalPointAndChartPoint = 0;

    if (watched == m_ChartView) {
        QPointF currChartPoint = m_Chart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(currChartPoint.x());
        transPoint = m_Chart->mapToPosition( (QPointF(m_currTimeIndex, m_earningList[m_currTimeIndex])));
        deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
    }

    if (m_currTimeIndex >= 0 && m_currTimeIndex < m_earningList.size()) {
        m_isKeyMove = true;
        setPropertyValue(m_currTimeIndex);
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
    }
}

void IndexChart::KeyReleaseFunc(QEvent *event) {
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

void IndexChart::moveMouse(int step) {
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

        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_earningList.size()) {
            m_isKeyMove = true;
            setPropertyValue(m_currTimeIndex);
            updateLabelSeries(m_currTimeIndex);
        }

        if (move_distance >= 1 || move_distance <=-1 || move_distance == 0) {
            QCursor::setPos(m_mouseInitPos.x() + move_distance, m_mouseInitPos.y());
        }
    }
}

double IndexChart::getPointXDistance() {
    int testIndex = 0;
    QPointF pointa = m_Chart->mapToPosition(QPointF(testIndex, m_earningList[testIndex]));
    QPointF pointb = m_Chart->mapToPosition(QPointF(testIndex+1, m_earningList[testIndex+1]));
    double distance = pointb.x() - pointa.x();
    return distance;
}

void IndexChart::closeEvent(QCloseEvent *event) {
    event;
    if (NULL != m_monitorWorker) {
        m_monitorWorker->stopTimer();
    }

    emit windowClose_signal(m_chartViewID);
}
