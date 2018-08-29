#include "csschartformone.h"
#include "ui_csschartformone.h"
#include "process_data_func.h"
#include "io_func.h"
#include <QMouseEvent>
#include <QtMath>
#include <QLegendMarker>
#include <QChartView>
#include <QBarSet>
#include <QColor>
#include <algorithm>
#include <QPainter>
#include "time_func.h"
#include "compute_func.h"
using std::max;

#pragma execution_character_set("utf-8")

//CSSChartFormOne::CSSChartFormOne(QWidget *parent) :
//    BaseChart(parent),ui(new Ui::CSSChartFormOne)
//{
//    ui->setupUi(this);
//}

CSSChartFormOne::CSSChartFormOne(int chartID, QString dbhost, QString timeType,
                                QString startDate, QString endDate, QString codeName,
                                QList<int> aveNumbList, QList<bool> isEMAList,
                                int mainAveNumb, int subAveNumb, int energyAveNumb,
                                double css12Rate, double mainCssRate1, double mainCssRate2,
                                double energyCssRate1, double energyCssRate2,
                                double maxCSS, double minCSS,
                                QWidget *parent):
                                m_dbhost(dbhost), m_timeType(timeType),
                                m_startDate(startDate), m_endDate(endDate), m_singleCodeName(codeName),
                                m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
                                m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
                                m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
                                m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
                                m_maxCSS(maxCSS), m_minCSS(minCSS),
                                BaseChart(chartID, parent),ui(new Ui::CSSChartFormOne)
{
    ui->setupUi(this);
    initCommonData();

    initHistoryData();
    registSignalParamsType();
    startGetData();

    addPropertyLabel();
}

CSSChartFormOne::~CSSChartFormOne()
{
    delete ui;

    if (NULL != m_histdataWorker) {
        delete m_histdataWorker;
        m_histdataWorker = NULL;
    }

    m_histdataThread.quit();
    m_histdataThread.wait();

}

void CSSChartFormOne::initCommonData() {
    m_databaseName = QString("MarketData_%1").arg(m_timeType);
    m_chartXaxisTickCount = 5;
    m_keyMoveCount = 0;
    m_currTimeIndex = 0;
    m_isKeyMove = false;
    m_mouseInitPos = QPoint(-1, -1);
    m_oldPointDistance = -1;
    m_cssMarkValueList << -200  << -158  << -130 << -80 << 80 << 130 << 158<< 200<< 300;
}

void CSSChartFormOne::initHistoryData() {
    int maxPreTimeNumb = Max(m_aveNumbList, 0, m_aveNumbList.size());
    m_newStartDate = getPreDate(m_startDate, m_timeType, maxPreTimeNumb);
    qDebug() << QString("maxPreTimeNumb: %1, newStartDate: %2").arg(maxPreTimeNumb).arg(m_newStartDate);
    m_histdataWorker = new HistoryData(m_dbhost, m_databaseName, m_newStartDate, m_endDate,
                                       m_singleCodeName, m_aveNumbList, m_isEMAList,
                                       m_mainAveNumb, m_subAveNumb, m_energyAveNumb,
                                       m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                       m_energyCssRate1, m_energyCssRate2,
                                       m_maxCSS, m_minCSS, 0);

    connect(&m_histdataThread, SIGNAL(finished()),
            m_histdataWorker, SLOT(deleteLater()));

    connect(this, SIGNAL(getCSSData_signal()),
            m_histdataWorker, SLOT(getCSSData_slot()));

    connect(m_histdataWorker, SIGNAL(sendCSSData_signal(QList<QString>, QList<QList<double>>,  QList<QList<double>>, int)),
            this, SLOT(sendCSSData_slot(QList<QString>, QList<QList<double>>,  QList<QList<double>>, int)));

    m_histdataWorker->moveToThread(&m_histdataThread);

    m_histdataThread.start();
}

void CSSChartFormOne::registSignalParamsType() {
    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
}

void CSSChartFormOne::addPropertyLabel() {
    int width = 120;
    int height = 20;
    int start_x = 10;
    int start_y = 20;
    int y_space = 10;
    QLabel* timeLabel = new QLabel(ui->groupBox);
    timeLabel->setObjectName(QStringLiteral("time_Label"));
    timeLabel->setGeometry(QRect(start_x, start_y, width, height));
    timeLabel->setText("时间:");
    m_labelList.append(timeLabel);


    QLabel* closeLabel = new QLabel(ui->groupBox);
    closeLabel->setObjectName(QStringLiteral("close_Label"));
    start_y = start_y + height + y_space;
    closeLabel->setGeometry(QRect(start_x, start_y, width, height));
    closeLabel->setText("收盘价:");
    m_labelList.append(closeLabel);

    for (int i = 0; i < m_aveNumbList.size(); ++i) {
        QLabel* aveLabel = new QLabel(ui->groupBox);
        if (m_isEMAList[i]) {
            aveLabel->setObjectName(QString("EMA_%1_Label").arg(m_aveNumbList[i]));
            aveLabel->setText(QString("EMA_%1: ").arg(m_aveNumbList[i]));
        } else {
            aveLabel->setObjectName(QString("MA_%1_Label").arg(m_aveNumbList[i]));
            aveLabel->setText(QString("MA_%1: ").arg(m_aveNumbList[i]));
        }
        start_y = start_y + height + y_space;
        aveLabel->setGeometry(QRect(start_x, start_y, width, height));
        m_labelList.append(aveLabel);
    }

    QLabel* mainCSSLabel = new QLabel(ui->groupBox);
    mainCSSLabel->setObjectName(QString("mainCSS_Label"));
    start_y = start_y + height + y_space;
    mainCSSLabel->setGeometry(QRect(start_x, start_y, width, height));
    mainCSSLabel->setText(QString("主指标_%1: ").arg(m_mainAveNumb));
    m_labelList.append(mainCSSLabel);

    QLabel* subCSSLabel = new QLabel(ui->groupBox);
    subCSSLabel->setObjectName(QString("subCSS_Label"));
    start_y = start_y + height + y_space;
    subCSSLabel->setGeometry(QRect(start_x, start_y, width, height));
    subCSSLabel->setText(QString("从指标_%1: ").arg(m_subAveNumb));
    m_labelList.append(subCSSLabel);

    QLabel* energyCSSLabel = new QLabel(ui->groupBox);
    energyCSSLabel->setObjectName(QString("energyCSS_Label"));
    start_y = start_y + height + y_space;
    energyCSSLabel->setGeometry(QRect(start_x, start_y, width, height));
    energyCSSLabel->setText(QString("势能指标_%1: ").arg(m_energyAveNumb));
    m_labelList.append(energyCSSLabel);

}

void CSSChartFormOne::initLayout() {
    QString time_type = m_databaseName.split("_")[1];
    ui->title_Label->setText(QString("%1: [%2, %3],%4, 指标图")
                             .arg(m_singleCodeName).arg(m_startDate)
                             .arg(m_endDate).arg(time_type));
    initChartView();

//    initTableContextMenu();

    if (m_aveChartView != NULL) {
        ui->gridLayout->addWidget(m_aveChartView, 1, 0);
    }

    if (m_cssChartView != NULL) {
        ui->gridLayout->addWidget(m_cssChartView, 2, 0);
    }

    connectMarkers();

    this->setMouseTracking(true);

    initTheme();

    setLineColor();
}

void CSSChartFormOne::initTheme() {
    m_aveChart->setTheme(QChart::ChartThemeDark);
    m_cssChart->setTheme(QChart::ChartThemeDark);
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0x121218));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    window()->setPalette(pal);
    ui->groupBox->setPalette(pal);
    ui->title_Label->setStyleSheet(QStringLiteral("color: rgb(250, 250, 250);font: 75 14pt \"微软雅黑\";"));
}

QLineSeries* getTestLineSeries() {
    QLineSeries* result = new QLineSeries;
    for (int i = 0; i < 100; ++i) {
        result->append(i,i);
    }
    return result;
}

void CSSChartFormOne::setLineColor() {
    m_aveLineSeries[0]->setPen(QPen(QBrush(QColor(200,80,0)), 1.5));
    m_aveLineSeries[1]->setColor(QColor(211, 211, 211));
    m_aveLineSeries[2]->setColor(QColor(34,139,34));
    m_aveLineSeries[3]->setColor(QColor(255,255,0));
    m_aveLineSeries[4]->setColor(QColor(213,0,249));
    m_aveLineSeries[5]->setColor(QColor(130,119,23));
    m_aveLineSeries[6]->setColor(QColor(0,172,193));
    m_aveLineSeries[7]->setColor(QColor(67,160,71));
    m_aveLineSeries[8]->setColor(QColor(229,115,115));
    m_aveLineSeries[9]->setColor(QColor(255, 255, 255));
    m_aveLineSeries[10]->setColor(QColor(255, 0, 0));

    m_cssLineSeries[0]->setColor(QColor(255,255,0));
    m_cssLineSeries[1]->setColor(QColor(34,139,34));

    m_cssMarkLineSeries[0]->setPen(QPen(QBrush(QColor(253,216,53)), 0.5));   // -200
    m_cssMarkLineSeries[1]->setPen(QPen(QBrush(QColor(117,117,117)), 0.5));  // -158
    m_cssMarkLineSeries[2]->setPen(QPen(QBrush(QColor(229,115,115)), 2));    // -130
    m_cssMarkLineSeries[3]->setPen(QPen(QBrush(QColor(211,47,47)), 0.5));    // -80
    m_cssMarkLineSeries[4]->setPen(QPen(QBrush(QColor(0,172,193)), 0.5));    // 80
    m_cssMarkLineSeries[5]->setPen(QPen(QBrush(QColor(0,172,193)), 0.5));    // 130
    m_cssMarkLineSeries[6]->setPen(QPen(QBrush(QColor(117,117,117)), 0.5));  // 158
    m_cssMarkLineSeries[7]->setPen(QPen(QBrush(QColor(253,216,53)), 0.5));   // 200
    m_cssMarkLineSeries[8]->setPen(QPen(QBrush(QColor(180,180,180)), 0.5));  // 300

    m_aveChartLabelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
    m_cssChartLabelSeries->setPen(QPen(QBrush(QColor(255,255,255)), 1));
//    m_cssMarkLineSeries[0]->setColor(QColor(253,216,53));
//    m_cssMarkLineSeries[1]->setColor(QColor(253,216,53));
//    m_cssMarkLineSeries[2]->setColor(QColor(117,117,117));
//    m_cssMarkLineSeries[3]->setColor(QColor(117,117,117));
//    m_cssMarkLineSeries[4]->setColor(QColor(229,115,115));
//    m_cssMarkLineSeries[5]->setColor(QColor(0,172,193));
//    m_cssMarkLineSeries[6]->setColor(QColor(211,47,47));
//    m_cssMarkLineSeries[7]->setColor(QColor(0,172,193));

    QList<QBarSet*> upBarSet =  m_energySeriesList[0]->barSets();
    upBarSet[0]->setBorderColor(QColor(239,154,154));
    upBarSet[0]->setColor(QColor(239,154,154));
    QList<QBarSet*> downBarSet =  m_energySeriesList[1]->barSets();
    downBarSet[0]->setBorderColor(QColor(0,229,255));
    downBarSet[0]->setColor(QColor(0,229,255));
}

QCategoryAxis* CSSChartFormOne::getCCSAxisY () {
    QCategoryAxis* axisY = new QCategoryAxis;
    axisY->setStartValue(m_minCSS);
    axisY->setMax(m_maxCSS);
    axisY->setMin(m_minCSS);
    axisY->setGridLineVisible(false);
    for (auto markValue:m_cssMarkValueList) {
        int pos = markValue - m_minCSS;
        axisY->append(QString("%1").arg(markValue), pos);
    }
    return axisY;
}

QBarCategoryAxis* CSSChartFormOne::getQBarCategoryAxisAxisY() {
    QBarCategoryAxis* axisY = new QBarCategoryAxis;
//    axisY->setMax(m_maxCSS);
//    axisY->setMin(m_minCSS);
    axisY->setGridLineVisible(false);
    for (auto markValue:m_cssMarkValueList) {
        int pos = markValue - m_minCSS;
        axisY->insert(pos, QString("%1").arg(markValue));
        qDebug() << pos << markValue;
    }
    return axisY;
}

void CSSChartFormOne::initChartView() {
    initAveChart();
    initCSSChart();
}

void CSSChartFormOne::initAveChart() {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList, m_chartXaxisTickCount);
    QValueAxis* aveAxisY = new QValueAxis;
    QList<double> aveRange =getChartYvalueRange(m_aveList);
    aveAxisY->setRange(aveRange[0], aveRange[1]);
    m_aveChartMaxValue = aveRange[1];
    m_aveChartMinValue = aveRange[0];

    for (int i = 0; i < m_aveList.size(); ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_aveList[i].size(); ++j) {
            currSeries->append(j, m_aveList[i][j]);
        }
        if (i == 0) {
            currSeries->setName(QString("收盘价"));
        } else {
            currSeries->setName(QString("%1").arg(m_aveNumbList[i-1]));
        }
        currSeries->setUseOpenGL(true);
        m_aveLineSeries.append(currSeries);
    }

    m_oldLabelIndex = m_timeList.size() / 2;

//    QBarSet* aveLabel = new QBarSet("");
//    aveLabel->insert(m_oldLabelIndex, aveRange[1]);
//    m_aveChartLabelSeries = new QStackedBarSeries();
//    m_aveChartLabelSeries->append(aveLabel);
//    m_aveChartLabelSeries->setBarWidth(0);
//    m_LabelSetList.append(aveLabel);

    m_aveChartLabelSeries = new QLineSeries();
    m_aveChartLabelSeries->append(m_oldLabelIndex, m_aveChartMinValue);
    m_aveChartLabelSeries->append(m_oldLabelIndex, m_aveChartMaxValue);

    m_aveChart = new QChart();
    for (auto lineSeries:m_aveLineSeries) {
        m_aveChart->addSeries(lineSeries);
    }
    m_aveChart->addSeries(m_aveChartLabelSeries);

    m_aveChartView = new QMyChartView(m_aveChart);
    m_aveChartView->setRenderHint(QPainter::Antialiasing);
    m_aveChartView->installEventFilter(this);
    m_aveChartView->setMouseTracking(true);

    m_aveChart->setAnimationOptions(QChart::NoAnimation);
    m_aveChart->addAxis(aveAxisY, Qt::AlignLeft);
    m_aveChart->addAxis(timeAxisX, Qt::AlignBottom);
    for (auto lineSeries:m_aveLineSeries) {
        lineSeries->attachAxis(aveAxisY);
    }
    m_aveChartLabelSeries->attachAxis(aveAxisY);
    m_aveChartLabelSeries->attachAxis(timeAxisX);
}

void CSSChartFormOne::initCSSChart() {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList, m_chartXaxisTickCount);
    QValueAxis* cssAxisY = new QValueAxis;
    cssAxisY->setRange(m_minCSS, m_maxCSS);
    cssAxisY->setGridLineVisible(false);
    cssAxisY->setLineVisible(true);

    QList<int> aveList;
    aveList << m_mainAveNumb << m_subAveNumb;
    for (int i = 0; i < 2; ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_cssList[i].size(); ++j) {
            currSeries->append(j, m_cssList[i][j]);
        }
        currSeries->setUseOpenGL(true);
        m_cssLineSeries.append(currSeries);
    }
    m_cssLineSeries[0]->setName("主指标");
    m_cssLineSeries[1]->setName("从指标");

    QBarSet *energySetUp = new QBarSet("势能指标大于0");
    QBarSet *energySetDown = new QBarSet("势能指标小于0");
    for (int i = 0; i < m_cssList[2].size(); ++i) {
        if (m_cssList[2][i] > 0) {
            energySetUp->append(m_cssList[2][i]);
            energySetDown->append(0);
        } else {
            energySetDown->append(m_cssList[2][i]);
            energySetUp->append(0);
        }
    }

    for (int i = 0; i < 2; ++i) {
        QStackedBarSeries* energySeries = new QStackedBarSeries;
        m_energySeriesList.append(energySeries);
    }
    m_energySeriesList[0]->append(energySetUp);
    m_energySeriesList[1]->append(energySetDown);

    for (int i = 0; i < m_cssMarkValueList.size(); ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_timeList.size(); ++j) {
            currSeries->append(j, m_cssMarkValueList[i]);
        }
        currSeries->setUseOpenGL(true);
        m_cssMarkLineSeries.append(currSeries);
        currSeries->setName(QString("%1").arg(m_cssMarkValueList[i]));
    }

//    QBarSet* cssLabelUp = new QBarSet("");
//    cssLabelUp->insert(m_oldLabelIndex, m_maxCSS);
//    QBarSet* cssLabelDown = new QBarSet("");
//    cssLabelDown->insert(m_oldLabelIndex, m_minCSS);
//    m_cssChartLabelSeries = new QStackedBarSeries();
//    m_cssChartLabelSeries->append(cssLabelUp);
//    m_cssChartLabelSeries->append(cssLabelDown);
//    m_cssChartLabelSeries->setBarWidth(0);
//    m_LabelSetList.append(cssLabelUp);
//    m_LabelSetList.append(cssLabelDown);

    m_cssChartLabelSeries = new QLineSeries();
    m_cssChartLabelSeries->append(m_oldLabelIndex, m_maxCSS);
    m_cssChartLabelSeries->append(m_oldLabelIndex, m_minCSS);

    m_cssChart = new QChart();
    for (auto lineSeries:m_cssLineSeries) {
        m_cssChart->addSeries(lineSeries);
    }
    for (auto barSeries:m_energySeriesList) {
        m_cssChart->addSeries(barSeries);
    }
    for (auto lineSeries:m_cssMarkLineSeries) {
        m_cssChart->addSeries(lineSeries);
    }
    m_cssChart->addSeries(m_cssChartLabelSeries);

    m_cssChartView = new QMyChartView(m_cssChart);
    m_cssChartView->setRenderHint(QPainter::Antialiasing);
    m_cssChartView->installEventFilter(this);
    m_cssChartView->setMouseTracking(true);

    m_cssChart->setAnimationOptions(QChart::NoAnimation);
    m_cssChart->addAxis(timeAxisX, Qt::AlignBottom);
    m_cssChart->addAxis(cssAxisY, Qt::AlignLeft);

    for (auto lineSeries:m_cssLineSeries) {
        lineSeries->attachAxis(timeAxisX);
        lineSeries->attachAxis(cssAxisY);
    }
    for (auto lineSeries:m_cssMarkLineSeries) {
        lineSeries->attachAxis(timeAxisX);
        lineSeries->attachAxis(cssAxisY);
    }
    for (auto barSeries:m_energySeriesList) {
        barSeries->attachAxis(timeAxisX);
        barSeries->attachAxis(cssAxisY);
    }
    m_cssChartLabelSeries->attachAxis(timeAxisX);
    m_cssChartLabelSeries->attachAxis(cssAxisY);
}

void CSSChartFormOne::sendCSSData_slot(QList<QString> timeList, QList<QList<double>> aveList,
                                       QList<QList<double>> cssList, int dataType) {

    qDebug() << "CSSChartFormOne::sendCSSData_slot";
    int startPos = getStartIndex(m_startDate, timeList);

    qDebug() << QString("startPos: %1").arg(startPos);

    m_timeList = getSubList(timeList, startPos, timeList.size());
    for (int i = 0; i < aveList.size(); ++i) {
        m_aveList.append(getSubList(aveList[i], startPos, aveList[i].size()));
    }
    for (int i = 0; i < cssList.size(); ++i) {
        m_cssList.append(getSubList(cssList[i], startPos, cssList[i].size()));
    }

//    qDebug() << "m_timeList.size: " << m_timeList.size();
//    for (int i =0; i < m_aveList.size(); ++i) {
//        qDebug() << QString("m_aveList[%1].size: %2").arg(i).arg(m_aveList[i].size());
//    }
    for (int i =0; i < m_cssList.size(); ++i) {
        qDebug() << QString("m_cssList[%1].size: %2").arg(i).arg(m_cssList[i].size());
    }
    initLayout();
}

void CSSChartFormOne::startGetData() {
    emit getCSSData_signal();
}

void CSSChartFormOne::initExtractKeyValueList() {

}

QList<QMyChartView*> CSSChartFormOne::getChartViewList() {
    QList<QMyChartView*>  result;
    return result;
}

QString CSSChartFormOne::getExcelFileName(QStringList keyValueList, QString fileDir) {
    QString result;
    return result;
}

QList<QStringList> CSSChartFormOne::getExcelData(QStringList keyValueList) {
    QList<QStringList> result;
    return result;
}

void CSSChartFormOne::setPropertyValue(int index) {
    if (index >-1 && index < m_timeList.size()) {
        m_labelList[0]->setText(QString("时间: %1").arg(m_timeList[index]));
        m_labelList[1]->setText(QString("收盘价: %1").arg(m_aveList[0][index]));
        for (int i = 0; i < m_aveNumbList.size(); ++i) {
            if (m_isEMAList[i]) {
                m_labelList[i+2]->setText(QString("EMA_%1: %2")
                                          .arg(m_aveNumbList[i]).arg(m_aveList[i+1][index]));
            } else {
                m_labelList[i+2]->setText(QString("MA_%1: %2")
                                          .arg(m_aveNumbList[i]).arg(m_aveList[i+1][index]));
            }
        }
        m_labelList[m_aveNumbList.size()+2]->setText(QString("主指标_%1: %2")
                                                     .arg(m_mainAveNumb).arg(m_cssList[0][index]));
        m_labelList[m_aveNumbList.size()+3]->setText(QString("从指标_%1: %2")
                                                     .arg(m_subAveNumb).arg(m_cssList[1][index]));
        m_labelList[m_aveNumbList.size()+4]->setText(QString("势能指标_%1: %2")
                                                     .arg(m_energyAveNumb).arg(m_cssList[2][index]));

        updateLableSeries(index);
    }
}

void CSSChartFormOne::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    if (m_isKeyMove)  {
        m_isKeyMove = false;
    } else {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        QPoint curPoint = mouseEvent->pos ();
        int currIndex = -1;
        if (watched == m_aveChartView) {
            QPointF curAveChartChartPoint = m_aveChart->mapToValue (curPoint);
            currIndex = qFloor(curAveChartChartPoint.x());
        }
        if (watched == m_cssChartView) {
            QPointF curCSSChartChartPoint = m_cssChart->mapToValue (curPoint);
            currIndex = qFloor(curCSSChartChartPoint.x());
        }
        setPropertyValue(currIndex);
    }
}

void CSSChartFormOne::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    QPointF transPoint;
    m_currTimeIndex = -1;
    m_mouseInitPos = QCursor::pos();
    m_keyMoveCount = 0;
    double deltaInGlobalPointAndChartPoint = 0;
    if (watched == m_aveChartView) {
        QPointF aveChartPoint = m_aveChart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(aveChartPoint.x());
        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeList.size()) {
            transPoint = m_aveChart->mapToPosition( (QPointF(m_currTimeIndex, m_aveList[0][m_currTimeIndex])));
            deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
        }
    }

    if (watched == m_cssChartView) {
        QPointF cssChartPoint = m_cssChart->mapToValue (curPoint);
        m_currTimeIndex = qFloor(cssChartPoint.x());
        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeList.size()) {
            transPoint = m_cssChart->mapToPosition( (QPointF(m_currTimeIndex, m_cssList[0][m_currTimeIndex])));
            deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
        }
    }

    if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeList.size()) {
        m_isKeyMove = true;
        setPropertyValue(m_currTimeIndex);
        m_mouseInitPos.setX(m_mouseInitPos.x() + deltaInGlobalPointAndChartPoint);
        QCursor::setPos(m_mouseInitPos);
    }

    updateMouseLine();
}

void CSSChartFormOne::KeyReleaseFunc(QEvent *event) {
    QKeyEvent* keyEvent = (QKeyEvent*)event;
    int step = 0;

    if (keyEvent->key() == Qt::Key_Left) {
        step = -1;
    }
    if (keyEvent->key() == Qt::Key_Right) {
        step = 1;
    }
    moveMouse(step);
//    updateMouseLine();
}

void CSSChartFormOne::moveMouse(int step) {
    double pointXDistance = getPointXDistance();

    if (step != 0) {
        m_keyMoveCount += step;
        m_currTimeIndex += step;
        float move_distance = pointXDistance * m_keyMoveCount;

        if (m_currTimeIndex >= 0 && m_currTimeIndex < m_timeList.size()) {
            m_isKeyMove = true;
            setPropertyValue(m_currTimeIndex);
        }

        if (move_distance >= 1 || move_distance <=-1 || move_distance == 0) {
            QCursor::setPos(m_mouseInitPos.x() + move_distance, m_mouseInitPos.y());
        }
    }
}

double CSSChartFormOne::getPointXDistance() {
    int testIndex = 0;
    QPointF pointa = m_aveChart->mapToPosition(QPointF(testIndex, m_aveList[0][testIndex]));
    QPointF pointb = m_aveChart->mapToPosition(QPointF(testIndex+1, m_aveList[0][testIndex+1]));
    double distance = pointb.x() - pointa.x();
    return distance;
}

void CSSChartFormOne::updateMouseLine() {
//    update();
//   QPoint cursorPos = QCursor::pos();
//   QPoint windowPos = window()->pos();
//   QSize windowSize = window()->size();
//   qDebug() << "cursorPos: " << cursorPos
//            << "windowPos: " << windowPos
//            << "windowSize: " << windowSize;

//   QPoint topPoint(cursorPos.x(), windowPos.y());
//   QPoint buttomPoint(cursorPos.x(), windowPos.y()+windowSize.height());
//   QPainter painter(this);
//   painter.setPen(QPen(Qt::blue,4));         // 设置画笔形式
//   painter.drawLine(topPoint,buttomPoint);   // 画直线
}

void CSSChartFormOne::connectMarkers()
{
    // Connect all markers to handler
    foreach (QLegendMarker* marker, m_aveChart->legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()),
                            this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()),
                         this, SLOT(handleMarkerClicked()));
    }

    foreach (QLegendMarker* marker, m_cssChart->legend()->markers()) {
        // Disconnect possible existing connection to avoid multiple connections
        QObject::disconnect(marker, SIGNAL(clicked()),
                            this, SLOT(handleMarkerClicked()));
        QObject::connect(marker, SIGNAL(clicked()),
                         this, SLOT(handleMarkerClicked()));
    }
}

void CSSChartFormOne::disconnectMarkers()
{
    foreach (QLegendMarker* marker, m_aveChart->legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()),
                            this, SLOT(handleMarkerClicked()));
    }
    foreach (QLegendMarker* marker, m_cssChart->legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()),
                            this, SLOT(handleMarkerClicked()));
    }
}

void CSSChartFormOne::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);
//    qDebug() << "handleMarkerClicked";

    // Toggle visibility of series
    marker->series()->setVisible(!marker->series()->isVisible());

    // Turn legend marker back to visible, since hiding series also hides the marker
    // and we don't want it to happen now.
    marker->setVisible(true);
    // Dim the marker, if series is not visible
    qreal alpha = 1.0;

    if (!marker->series()->isVisible()) {
        alpha = 0.5;
    }

    QColor color;
    QBrush brush = marker->labelBrush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setLabelBrush(brush);

    brush = marker->brush();
    color = brush.color();
    color.setAlphaF(alpha);
    brush.setColor(color);
    marker->setBrush(brush);

    QPen pen = marker->pen();
    color = pen.color();
    color.setAlphaF(alpha);
    pen.setColor(color);
    marker->setPen(pen);

    /*
    switch (marker->type())
    {
//        case QLegendMarker::LegendMarkerTypeXY:
        case true:
        {
            // Toggle visibility of series
            marker->series()->setVisible(!marker->series()->isVisible());

            // Turn legend marker back to visible, since hiding series also hides the marker
            // and we don't want it to happen now.
            marker->setVisible(true);
            // Dim the marker, if series is not visible
            qreal alpha = 1.0;

            if (!marker->series()->isVisible()) {
                alpha = 0.5;
            }

            QColor color;
            QBrush brush = marker->labelBrush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setLabelBrush(brush);

            brush = marker->brush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setBrush(brush);

            QPen pen = marker->pen();
            color = pen.color();
            color.setAlphaF(alpha);
            pen.setColor(color);
            marker->setPen(pen);

            break;
        }
    default:
        {
        qDebug() << "Unknown marker type";
        break;
        }
    }
    */
}

void CSSChartFormOne::updateLableSeries(int index) {
    /**/
//    qDebug() <<"m_oldLabelIndex: " << m_oldLabelIndex <<", index: " << index;

//    m_LabelSetList[0]->remove(m_oldLabelIndex);
//    m_LabelSetList[1]->remove(m_oldLabelIndex);
//    m_LabelSetList[2]->remove(m_oldLabelIndex);

//    m_LabelSetList[0]->insert(index, m_aveChartMaxValue);
//    m_LabelSetList[1]->insert(index, m_maxCSS);
//    m_LabelSetList[2]->insert(index, m_minCSS);

//    qDebug() << QString("m_LabelSetList[0]->count(): %1").arg(m_LabelSetList[0]->count());
//    qDebug() << QString("m_LabelSetList[1]->count(): %1").arg(m_LabelSetList[1]->count());
//    qDebug() << QString("m_LabelSetList[2]->count(): %1").arg(m_LabelSetList[1]->count());

    //    m_aveChartLabelSeries->barSets()[0]->remove(m_oldLabelIndex);
    //    m_cssChartLabelSeries->barSets()[0]->remove(m_oldLabelIndex);
    //    m_cssChartLabelSeries->barSets()[1]->remove(m_oldLabelIndex);

//    qDebug() << "m_aveChartLabelSeries->barSets()[0]->count(): " << m_aveChartLabelSeries->barSets()[0]->count();
//    qDebug() << "m_cssChartLabelSeries->barSets()[0]->count(): " << m_cssChartLabelSeries->barSets()[0]->count();
//    qDebug() << "m_cssChartLabelSeries->barSets()[1]->count(): " << m_cssChartLabelSeries->barSets()[1]->count();
//    qDebug() << QString("m_aveChartLabelSeries->barSets()[0]->at(%1): %2")
//                .arg(m_oldLabelIndex).arg(m_aveChartLabelSeries->barSets()[0]->at(m_oldLabelIndex));
//    qDebug() << QString("m_cssChartLabelSeries->barSets()[0]->at(%1): %2")
//                .arg(m_oldLabelIndex).arg(m_cssChartLabelSeries->barSets()[0]->at(m_oldLabelIndex));
//    qDebug() << QString("m_cssChartLabelSeries->barSets()[1]->at(%1): %2")
//                .arg(m_oldLabelIndex).arg(m_cssChartLabelSeries->barSets()[1]->at(m_oldLabelIndex));



//    qDebug() << QString("m_LabelSetList[0]->at(%1): %2")
//                .arg(m_oldLabelIndex).arg(m_LabelSetList[0]->at(m_oldLabelIndex));
//    qDebug() << QString("m_LabelSetList[1]->at(%1): %2")
//                .arg(m_oldLabelIndex).arg(m_LabelSetList[1]->at(m_oldLabelIndex));
//    qDebug() << QString("m_LabelSetList[2]->at(%1): %2")
//                .arg(m_oldLabelIndex).arg(m_LabelSetList[2]->at(m_oldLabelIndex));

    m_cssChartLabelSeries->remove(m_oldLabelIndex, m_maxCSS);
    m_cssChartLabelSeries->remove(m_oldLabelIndex, m_minCSS);
    m_aveChartLabelSeries->remove(m_oldLabelIndex, m_aveChartMinValue);
    m_aveChartLabelSeries->remove(m_oldLabelIndex, m_aveChartMaxValue);

    m_cssChartLabelSeries->append(index, m_maxCSS);
    m_cssChartLabelSeries->append(index, m_minCSS);
    m_aveChartLabelSeries->append(index, m_aveChartMinValue);
    m_aveChartLabelSeries->append(index, m_aveChartMaxValue);

    update();

//    m_aveChartLabelSeries->barSets()[0]->insert(index, m_aveChartMaxValue);
//    m_cssChartLabelSeries->barSets()[0]->insert(index, m_maxCSS);
//    m_cssChartLabelSeries->barSets()[1]->insert(index, m_minCSS);

//    m_aveChart->removeSeries(m_aveChartLabelSeries);
//    m_aveChartLabelSeries->remove(m_aveChartLabelSeries->barSets()[0]);

//    QBarSet* aveLabel = new QBarSet("");
//    aveLabel->insert(index, m_aveChartMaxValue);
//    m_aveChartLabelSeries = new QStackedBarSeries();
//    m_aveChartLabelSeries->append(aveLabel);
//    m_aveChartLabelSeries->setBarWidth(0);
//    m_aveChartLabelSeries->attachAxis(m_aveChart->axisY());

//    m_aveChart->addSeries(m_aveChartLabelSeries);

//    m_cssChartLabelSeries->remove(m_cssChartLabelSeries->barSets()[0]);
//    m_cssChartLabelSeries->remove(m_cssChartLabelSeries->barSets()[0]);
//    QBarSet* cssLabelUp = new QBarSet("");
//    cssLabelUp->insert(index, m_maxCSS);
//    QBarSet* cssLabelDown = new QBarSet("");
//    cssLabelDown->insert(index, m_minCSS);
//    m_cssChartLabelSeries = new QStackedBarSeries();
//    m_cssChartLabelSeries->append(cssLabelUp);
//    m_cssChartLabelSeries->append(cssLabelDown);
//    m_cssChartLabelSeries->setBarWidth(0);

    m_oldLabelIndex = index;
//    m_aveChartLabelSeries->hide();
//    m_aveChartLabelSeries->show();
//    m_cssChartLabelSeries->hide();
//    m_cssChartLabelSeries->show();
//    qDebug() << "m_oldLabelIndex: " << m_oldLabelIndex;
//    qDebug() << m_aveChartLabelSeries->barSets()[0]->at(index);
//    qDebug() << m_cssChartLabelSeries->barSets()[0]->at(index);
//    qDebug() << m_cssChartLabelSeries->barSets()[1]->at(index);

}

void CSSChartFormOne::paintEvent(QPaintEvent *) {
//    QPoint cursorPos = QCursor::pos();
//    QPoint windowPos = window()->pos();
//    QSize windowSize = window()->size();


//    QPoint topPoint(cursorPos.x(), windowPos.y());
//    QPoint buttomPoint(cursorPos.x(), windowPos.y()+windowSize.height());
//    QPainter painter(this);
//    painter.setPen(QPen(Qt::red,4));         // 设置画笔形式
//    painter.drawLine(topPoint,buttomPoint);   // 画直线

//    qDebug() << "cursorPos: " << cursorPos
//             << ", windowPos: " << windowPos
//             << ", windowSize: " << windowSize
//             << ", topPoint: " << topPoint
//             << ", buttomPoint: " << buttomPoint;
}

