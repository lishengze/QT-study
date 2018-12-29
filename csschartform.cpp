﻿#include "csschartform.h"
#include "ui_csschartform.h"
#include "process_data_func.h"
#include "io_func.h"
#include <QMouseEvent>
#include <QtMath>
#include <QLegendMarker>
#include <QChartView>
#include <QBarSet>
#include <QColor>
#include <QMessageBox>
#include <algorithm>
#include <QDesktopServices>
#include "secode_func.h"
#include "excel_func.h"
#include "time_func.h"
#include "compute_func.h"
using std::max;

#include <QtMath>

#pragma execution_character_set("utf-8")

CSSChartForm::CSSChartForm(int chartID, QString dbhost, QStringList timeTypeList,
                            QString startDate, QString endDate,
                            QString selectCodeName, QString hedgedCodeName,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent):
                            m_isPortfolio(false), m_dbhost(dbhost), m_timeTypeList(timeTypeList),
                            m_startDate(startDate), m_endDate(endDate),
                            m_selectCodeName(selectCodeName), m_hedgedCodeName(hedgedCodeName),
                            m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
                            m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
                            m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
                            m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
                            m_maxCSS(maxCSS), m_minCSS(minCSS),
                            BaseChart(chartID, parent),ui(new Ui::CSSChartForm)
{
    ui->setupUi(this);
    initCommonData();
    initListData();
    initHistoryData();
    initColors();
    registSignalParamsType();
    startGetData();
    qDebug() << m_selectCodeName << m_hedgedCodeName;
}

CSSChartForm::CSSChartForm(int chartID, QString dbhost, QStringList timeTypeList,
                            QString startDate, QString endDate,
                            QMap<QString, int> portfolioMap, QString portfolioName,
                            QString hedgeIndexCode, int hedgeIndexCount,
                            QList<int> aveNumbList, QList<bool> isEMAList,
                            int mainAveNumb, int subAveNumb, int energyAveNumb,
                            double css12Rate, double mainCssRate1, double mainCssRate2,
                            double energyCssRate1, double energyCssRate2,
                            double maxCSS, double minCSS,
                            QWidget *parent):
    m_isPortfolio(true), m_dbhost(dbhost), m_timeTypeList(timeTypeList),
    m_startDate(startDate), m_endDate(endDate),
    m_portfolioMap(portfolioMap), m_portfolioName(portfolioName),
    m_hedgeIndexCode(hedgeIndexCode), m_hedgeIndexCount(hedgeIndexCount),
    m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
    m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
    m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
    m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
    m_maxCSS(maxCSS), m_minCSS(minCSS),
    BaseChart(chartID, parent),ui(new Ui::CSSChartForm)
{
    ui->setupUi(this);
    initCommonData();
    initSecodeList();
    initListData();
    initHistoryData();
    initColors();
    registSignalParamsType();
    startGetData();
}

CSSChartForm::~CSSChartForm()
{
    delete ui;

    for (int i = 0; i < m_histdataWorkerList.size(); ++i) {
        if (m_histdataWorkerList[i]!=NULL) {
            delete m_histdataWorkerList[i];
            m_histdataWorkerList[i] = NULL;
        }
    }

    for (int i = 0; i < m_histdataThreadList.size(); ++i) {
        m_histdataThreadList[i]->quit();
        m_histdataThreadList[i]->wait();
    }
}

void CSSChartForm::initLayout() {
    for (int i = 0; i < m_timeTypeList.size(); ++i) {
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString("GridLayout_%1").arg(i));
        m_gridLayoutList.append(gridLayout);
    }

    m_currColNumb = min(m_maxColNumb, m_timeTypeList.size());
    if (m_gridLayoutList.size() > 3) {
        int rowNumb = 2;
        int rows = qCeil(double(m_gridLayoutList.size()) / double(rowNumb));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < rowNumb; ++j) {
                if (i * rowNumb + j >= m_gridLayoutList.size()) {
                    break;
                } else {
                    ui->main_gridLayout->addLayout(m_gridLayoutList[i*rowNumb + j], i, j, 1, 1);
                }
            }
        }
    } else {
        for (int i = 0; i < m_gridLayoutList.size(); ++i) {
            ui->main_gridLayout->addLayout(m_gridLayoutList[i], 0, i, 1, 1);
        }
    }
}

void CSSChartForm::initTheme() {
    QPalette pal = window()->palette();
    pal.setColor(QPalette::Window, QRgb(0x121218));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    window()->setPalette(pal);
}

void CSSChartForm::initCommonData() {
    m_maxColNumb = 3;
    m_chartXaxisTickCount = 5;
    m_cssMarkValueList << -265 << -240 << -200  << -158  << -130 << -80
                       << 80 << 130 << 158<< 200 << 240 << 265 << 300;

    initExtractKeyValueList();
    initLabelRowColNumb();
}

void CSSChartForm::initSecodeList() {
    m_secodeStyle = "tinysoft";
    m_portfolioMap = transStrategyMap(m_portfolioMap, m_secodeStyle);

    if (m_hedgeIndexCount != -1) {
        m_hedgeIndexCode = getCompleteIndexCode(m_hedgeIndexCode, m_secodeStyle);
        m_portfolioMap.insert (m_hedgeIndexCode, m_hedgeIndexCount);
    }

    m_secodeNameList = m_portfolioMap.keys();
}

void CSSChartForm::initListData() {
    for (auto timeType:m_timeTypeList) {
        m_databaseNameList.append(QString("MarketData_%1").arg(timeType));
        QList<QList<double>> emptyAVEData;
        QList<QList<double>> emptyCSSData;
        QList<QString> emptyTimeData;
        QList<double> emptyIndexData;
        m_aveList.append(emptyAVEData);
        m_cssList.append(emptyCSSData);
        m_timeList.append(emptyTimeData);
        m_indexDataList.append(emptyIndexData);
        m_hedgedDataList.append(emptyIndexData);

        m_aveListStore.append(emptyAVEData);
        m_cssListStore.append(emptyCSSData);
        m_timeListStore.append(emptyTimeData);
        m_indexDataListStore.append(emptyIndexData);
        m_hedgedDataListStore.append(emptyIndexData);

        QList<QLineSeries*> emptyLineSeries;
        m_aveLineSeriesList.append(emptyLineSeries);
        m_cssLineSeriesList.append(emptyLineSeries);
        m_cssMarkLineSeriesList.append(emptyLineSeries);

        QList<QStackedBarSeries*> emptyStackedBarSeries;
        m_energySeriesListList.append(emptyStackedBarSeries);

        QList<QGroupBox*> empytGroupBox;
        m_groupBoxListList.append(empytGroupBox);

        QMyChartView* emptyMyChartView;
        m_aveChartViewList.append(emptyMyChartView);
        m_cssChartViewList.append(emptyMyChartView);

        QChart* emptyChart;
        m_aveChartList.append(emptyChart);
        m_cssChartList.append(emptyChart);

        QList<QLabel*> emptyLabelList;
        m_aveChartlabelListList.append(emptyLabelList);
        m_cssChartlabelListList.append(emptyLabelList);

        QList<double> emptyChartRange;
        m_aveChartRange.append(emptyChartRange);

        m_aveChartLabelSeriesList.append(NULL);
        m_cssChartLabelSeriesList.append(NULL);

        m_keyMoveCountList.append(0);
        m_currTimeIndexList.append(0);
        m_isKeyMoveList.append(false);
        m_mouseInitPosList.append(QPoint(-1, -1));

        m_oldLabelIndexList.append(0);
    }
}

void CSSChartForm::initHistoryData() {
    for (int i = 0; i < m_timeTypeList.size(); ++i) {
        QString timeType = m_timeTypeList[i];
        QString databaseName = m_databaseNameList[i];
        int maxPreTimeNumb = Max(m_aveNumbList, 0, m_aveNumbList.size()) + 90;
        QString newStartDate = getPreDate(m_startDate, timeType, maxPreTimeNumb);
        qDebug() << QString("maxPreTimeNumb: %1, newStartDate: %2").arg(maxPreTimeNumb).arg(newStartDate);
        HistoryData* histdataWorker;
        if (m_isPortfolio)
        {
            int threadNumb = m_secodeNameList.size();
            QStringList keyValueList;
            keyValueList << "TCLOSE" << "VOTRUNOVER";

            histdataWorker = new HistoryData(m_dbhost, databaseName,
                                             newStartDate, m_endDate,
                                             threadNumb, m_secodeNameList, m_hedgeIndexCode,
                                             m_portfolioMap, keyValueList,
                                             m_aveNumbList, m_isEMAList,
                                             m_mainAveNumb, m_subAveNumb, m_energyAveNumb,
                                             m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                             m_energyCssRate1, m_energyCssRate2,
                                             m_maxCSS, m_minCSS, i);
        }
        else
        {
            histdataWorker = new HistoryData(m_dbhost, databaseName, newStartDate, m_endDate,
                                             m_selectCodeName, m_hedgedCodeName,
                                             m_aveNumbList, m_isEMAList,
                                             m_mainAveNumb, m_subAveNumb, m_energyAveNumb,
                                             m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                             m_energyCssRate1, m_energyCssRate2,
                                             m_maxCSS, m_minCSS, i);
        }

        QThread* histdataThread = new QThread();

        connect(histdataThread, SIGNAL(finished()),
                histdataWorker, SLOT(deleteLater()));

        connect(this, SIGNAL(getCSSData_signal()),
                histdataWorker, SLOT(getCSSData_slot()));

        connect(histdataWorker, SIGNAL(sendCSSData_signal(QList<QString>, QList<QList<double>>,  QList<QList<double>>, int)),
                this, SLOT(sendCSSData_slot(QList<QString>, QList<QList<double>>,  QList<QList<double>>, int)));

        histdataWorker->moveToThread(histdataThread);

        histdataThread->start();

        m_histdataWorkerList.append(histdataWorker);
        m_histdataThreadList.append(histdataThread);
    }
}

void CSSChartForm::initLabelRowColNumb() {
    switch (m_timeTypeList.size()) {
    case 1: m_labelRowNumb = 1; break;
    case 2:
    case 3: m_labelRowNumb = 2; break;
    case 4: m_labelRowNumb = 3; break;
    case 5:
    case 6: m_labelRowNumb = 4; break;
    default:
        m_labelRowNumb = m_timeTypeList.size();
    }

    m_labelColNumb = qCeil(double(m_aveNumbList.size() + 3)/double(m_labelRowNumb));
    qDebug() << QString("m_lableRowNumb: %1, m_labelColNumb: %2").arg(m_labelRowNumb).arg(m_labelColNumb);
}

void CSSChartForm::initExtractKeyValueList() {
    m_extractKeyValueList << "全选";
}

void CSSChartForm::initColors() {
    m_aveChartColorList.append(QColor(209,196,233));    // 频率标记颜色;
    m_aveChartColorList.append(QColor(220,220,220));    // 时间颜色;
    m_aveChartColorList.append(QColor(200,80,0));       // 收盘价;

    m_aveChartColorList.append(QColor(211,211,211));    // 均线;
    m_aveChartColorList.append(QColor(34,139,34));
    m_aveChartColorList.append(QColor(255,255,0));
    m_aveChartColorList.append(QColor(213,0,249));
    m_aveChartColorList.append(QColor(130,119,23));
    m_aveChartColorList.append(QColor(0,172,193));
    m_aveChartColorList.append(QColor(67,160,71));
    m_aveChartColorList.append(QColor(229,115,115));
    m_aveChartColorList.append(QColor(255, 255, 255));
    m_aveChartColorList.append(QColor(255, 0, 0));

    m_cssChartColorList.append(QColor(255,255,0));      // 主指标颜色
    m_cssChartColorList.append(QColor(34,139,34));      // 从指标颜色
    m_cssChartColorList.append(QColor(220,220,220));    // 势能label颜色

    m_cssChartColorList.append(QColor(244,143,177));   // -265
    m_cssChartColorList.append(QColor(0,229,255));   // -240
    m_cssChartColorList.append(QColor(253,216,53));   // -200
    m_cssChartColorList.append(QColor(117,117,117));  // -158
    m_cssChartColorList.append(QColor(229,115,115));  // -130
    m_cssChartColorList.append(QColor(211,47,47));  // -80
    m_cssChartColorList.append(QColor(0,172,193));  // 80
    m_cssChartColorList.append(QColor(0,172,193));  // 130
    m_cssChartColorList.append(QColor(117,117,117));  // 158
    m_cssChartColorList.append(QColor(253,216,53));  // 200    
    m_cssChartColorList.append(QColor(0,229,255));   // 240
    m_cssChartColorList.append(QColor(244,143,177));   // 265

    m_cssChartColorList.append(QColor(180,180,180));  // 300

    m_cssChartColorList.append(QColor(239,154,154));    // 势能大于0
    m_cssChartColorList.append(QColor(0,229,255));      // 势能小于0
}

void CSSChartForm::startGetData() {
    emit getCSSData_signal();
}

void CSSChartForm::registSignalParamsType() {
    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
}

void CSSChartForm::sendCSSData_slot(QList<QString> timeList, QList<QList<double>> aveList,
                                    QList<QList<double>> cssList, int dataID) {
    QMutexLocker locker(&m_mutex);
    int startPos = getStartIndex(m_startDate, timeList);

    qDebug() <<"dataID: " << dataID <<", startPos: " << startPos;

    m_timeList[dataID] = getSubList(timeList, startPos, timeList.size());
    m_realStartTime = m_timeList[dataID][0];
    m_realEndTime = m_timeList[dataID].last();

    for (int i = 0; i < m_aveNumbList.size() + 1; ++i) {
        m_aveList[dataID].append(getSubList(aveList[i], startPos, aveList[i].size()));
    }

    for (int i = 0; i < cssList.size(); ++i) {
        m_cssList[dataID].append(getSubList(cssList[i], startPos, cssList[i].size()));
    }

    if(m_isPortfolio)
    {
        m_hedgedDataList[dataID] = getSubList(aveList[aveList.size()-2], startPos, aveList.last().size()) ;
        m_indexDataList[dataID] = getSubList(aveList.last(), startPos, aveList.last().size()) ;
    }

    startPos = 0;
    m_timeListStore[dataID] = getSubList(timeList, startPos, timeList.size());
//    qDebug() << QString("m_timeListStore[%1].size: %2").arg(dataID).arg(timeList.size());
    for (int i = 0; i < m_aveNumbList.size() + 1; ++i) {
        m_aveListStore[dataID].append(getSubList(aveList[i], startPos, aveList[i].size()));
//        qDebug() << QString("aveList[%1].size: %2").arg(i).arg(aveList[i].size());
    }

    for (int i = 0; i < cssList.size(); ++i) {
        m_cssListStore[dataID].append(getSubList(cssList[i], startPos, cssList[i].size()));
//        qDebug() << QString("cssList[%1].size: %2").arg(i).arg(cssList[i].size());
    }

    if(m_isPortfolio)
    {
        m_hedgedDataListStore[dataID] = getSubList(aveList[aveList.size()-2], startPos, aveList.last().size()) ;
        m_indexDataListStore[dataID] = getSubList(aveList.last(), startPos, aveList.last().size()) ;
//        qDebug() << QString("m_hedgedDataListStore[%1].size: %2,  m_indexDataListStore[%1].size: %3")
//                    .arg(dataID).arg(aveList[aveList.size()-2].size()).arg(aveList.last().size());
    }

    setChart(dataID);
    initTheme();

    m_currDataIDList.append(dataID);
    if (m_currDataIDList.size() == m_timeTypeList.size()) {
        setChartView();
        setLabels();
        setColors();
        connectMarkers();
        setWindowTitleName();
    }
}

void CSSChartForm::setChart(int dataID) {
    setAVEChart(dataID);
    setCSSChart(dataID);
}

void CSSChartForm::setAVEChart(int dataID) {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList[dataID], m_chartXaxisTickCount);
    QValueAxis* aveAxisY = new QValueAxis;
    QList<double> aveRange =getChartYvalueRange(m_aveList[dataID]);
    m_aveChartRange[dataID] = aveRange;
    aveAxisY->setRange(aveRange[0], aveRange[1]);

    for (int i = 0; i < m_aveList[dataID].size(); ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_aveList[dataID][i].size(); ++j) {
            currSeries->append(j, m_aveList[dataID][i][j]);
        }
        if (i == 0) {
            currSeries->setName(QString("收盘价"));
        } else {
            currSeries->setName(QString("%1").arg(m_aveNumbList[i-1]));
        }
        currSeries->setUseOpenGL(true);
        m_aveLineSeriesList[dataID].append(currSeries);
    }

    QLineSeries* aveChartLabelSeries = new QLineSeries();
    aveChartLabelSeries->append(m_oldLabelIndexList[dataID], m_aveChartRange[dataID][0]);
    aveChartLabelSeries->append(m_oldLabelIndexList[dataID], m_aveChartRange[dataID][1]);

    QChart* aveChart = new QChart();
    for (auto lineSeries:m_aveLineSeriesList[dataID]) {
        aveChart->addSeries(lineSeries);
    }
    aveChart->addSeries(aveChartLabelSeries);
    m_aveChartLabelSeriesList[dataID] = aveChartLabelSeries;

    QMyChartView* aveChartView = new QMyChartView(aveChart);
    aveChartView->setRenderHint(QPainter::Antialiasing);
    aveChartView->installEventFilter(this);
    aveChartView->setMouseTracking(true);

    aveChart->setAnimationOptions(QChart::NoAnimation);
    aveChart->addAxis(aveAxisY, Qt::AlignLeft);
    aveChart->addAxis(timeAxisX, Qt::AlignBottom);
    for (auto lineSeries:m_aveLineSeriesList[dataID]) {
        lineSeries->attachAxis(aveAxisY);
    }
    aveChartLabelSeries->attachAxis(aveAxisY);
    aveChartLabelSeries->attachAxis(timeAxisX);
    m_aveChartViewList[dataID] = aveChartView;
    m_aveChartList[dataID] = aveChart;
}

void CSSChartForm::setCSSChart(int dataID) {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList[dataID], m_chartXaxisTickCount);
    QValueAxis* cssAxisY = new QValueAxis;
    cssAxisY->setRange(m_minCSS, m_maxCSS);
    cssAxisY->setGridLineVisible(false);
    cssAxisY->setLineVisible(true);

    QList<int> aveList;
    aveList << m_mainAveNumb << m_subAveNumb;
    for (int i = 0; i < 2; ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_cssList[dataID][i].size(); ++j) {
            currSeries->append(j, m_cssList[dataID][i][j]);
        }
        currSeries->setUseOpenGL(true);
        m_cssLineSeriesList[dataID].append(currSeries);
    }
    m_cssLineSeriesList[dataID][0]->setName("主指标");
    m_cssLineSeriesList[dataID][1]->setName("从指标");

    QBarSet *energySetUp = new QBarSet("势能指标大于0");
    QBarSet *energySetDown = new QBarSet("势能指标小于0");
    for (int i = 0; i < m_cssList[dataID][2].size(); ++i) {
        if (m_cssList[dataID][2][i] > 0) {
            energySetUp->append(m_cssList[dataID][2][i]);
            energySetDown->append(0);
        } else {
            energySetDown->append(m_cssList[dataID][2][i]);
            energySetUp->append(0);
        }
    }

    for (int i = 0; i < 2; ++i) {
        QStackedBarSeries* energySeries = new QStackedBarSeries;
        m_energySeriesListList[dataID].append(energySeries);
    }
    m_energySeriesListList[dataID][0]->append(energySetUp);
    m_energySeriesListList[dataID][1]->append(energySetDown);

    for (int i = 0; i < m_cssMarkValueList.size(); ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_timeList[dataID].size(); ++j) {
            currSeries->append(j, m_cssMarkValueList[i]);
        }
        currSeries->setUseOpenGL(true);
        m_cssMarkLineSeriesList[dataID].append(currSeries);
        currSeries->setName(QString("%1").arg(m_cssMarkValueList[i]));
    }

    QLineSeries* cssChartLabelSeries = new QLineSeries();
    cssChartLabelSeries->append(m_oldLabelIndexList[dataID], m_maxCSS);
    cssChartLabelSeries->append(m_oldLabelIndexList[dataID], m_minCSS);

    QChart* cssChart = new QChart();
    for (auto lineSeries:m_cssLineSeriesList[dataID]) {
        cssChart->addSeries(lineSeries);
    }
    for (auto barSeries:m_energySeriesListList[dataID]) {
        cssChart->addSeries(barSeries);
    }
    for (auto lineSeries:m_cssMarkLineSeriesList[dataID]) {
        cssChart->addSeries(lineSeries);
    }
    cssChart->addSeries(cssChartLabelSeries);
    m_cssChartLabelSeriesList[dataID] = cssChartLabelSeries;

    QMyChartView* cssChartView = new QMyChartView(cssChart);
    cssChartView->setRenderHint(QPainter::Antialiasing);
    cssChartView->installEventFilter(this);
    cssChartView->setMouseTracking(true);

    cssChart->setAnimationOptions(QChart::NoAnimation);
    cssChart->addAxis(timeAxisX, Qt::AlignBottom);
    cssChart->addAxis(cssAxisY, Qt::AlignLeft);

    for (auto lineSeries:m_cssLineSeriesList[dataID]) {
        lineSeries->attachAxis(timeAxisX);
        lineSeries->attachAxis(cssAxisY);
    }
    for (auto lineSeries:m_cssMarkLineSeriesList[dataID]) {
        lineSeries->attachAxis(timeAxisX);
        lineSeries->attachAxis(cssAxisY);
    }
    for (auto barSeries:m_energySeriesListList[dataID]) {
        barSeries->attachAxis(timeAxisX);
        barSeries->attachAxis(cssAxisY);
    }
    cssChartLabelSeries->attachAxis(timeAxisX);
    cssChartLabelSeries->attachAxis(cssAxisY);
    m_cssChartViewList[dataID] = cssChartView;
    m_cssChartList[dataID] = cssChart;
}

void CSSChartForm::setChartView() {
    for (int i = 0; i < m_aveChartViewList.size(); ++i) {
        QGroupBox* groupBox1 = new QGroupBox();
        QGroupBox* groupBox2 = new QGroupBox();
        ui->main_gridLayout->addWidget(groupBox1, 0, i);
        ui->main_gridLayout->addWidget(m_aveChartViewList[i], 1, i);
        ui->main_gridLayout->addWidget(groupBox2, 2, i);
        ui->main_gridLayout->addWidget(m_cssChartViewList[i], 3, i);
        m_aveChartList[i]->setTheme(QChart::ChartThemeDark);
        m_cssChartList[i]->setTheme(QChart::ChartThemeDark);

        groupBox1->setStyleSheet("border:none");
        groupBox2->setStyleSheet("border:none");

        m_groupBoxListList[i].append(groupBox1);
        m_groupBoxListList[i].append(groupBox2);
    }

    for (int i = 0; i <  m_aveChartViewList.size(); ++i) {
        ui->main_gridLayout->setColumnStretch(i,1);
    }

    ui->main_gridLayout->setRowStretch(0, m_labelRowNumb);
    ui->main_gridLayout->setRowStretch(1,20);
    ui->main_gridLayout->setRowStretch(2,1);
    ui->main_gridLayout->setRowStretch(3,14);

    initTableContextMenu();
}

void CSSChartForm::setLabels() {
    int width = 80;
    int height = 20;
    int y_space = 10;
    int x_space = 5;
    int x_start = 15;
    int y_start = 10;

    for (int i = 0; i < m_groupBoxListList.size(); ++i) {
        int x_pos = x_start;
        int y_pos = y_start;
        QGroupBox* aveGroupBox = m_groupBoxListList[i][0];
        QGroupBox* cssGroupBox = m_groupBoxListList[i][1];

        int lableIndex = 0;
        QLabel* dataFreqLabel = new QLabel(aveGroupBox);
        dataFreqLabel->setObjectName(QStringLiteral("dataFreq_Label"));
        dataFreqLabel->setGeometry(QRect(x_pos, y_pos, 50, height));
        dataFreqLabel->setText(QString("F: %1").arg(m_timeTypeList[i]));
        m_aveChartlabelListList[i].append(dataFreqLabel);
        ++lableIndex;

        QLabel* timeLabel = new QLabel(aveGroupBox);
        timeLabel->setObjectName(QStringLiteral("time_Label"));
        x_pos = x_pos + 50 + x_space;
        timeLabel->setGeometry(QRect(x_pos, y_pos, 120, height));
        timeLabel->setText("T:");
        m_aveChartlabelListList[i].append(timeLabel);
        ++lableIndex;

        QLabel* closeLabel = new QLabel(aveGroupBox);
        closeLabel->setObjectName(QStringLiteral("close_Label"));
        x_pos = x_pos + 120 + x_space;
        closeLabel->setGeometry(QRect(x_pos, y_pos, width, height));
        closeLabel->setText("C:");
        m_aveChartlabelListList[i].append(closeLabel);
        ++lableIndex;

//        bool isChangeLine = false;
        for (int j = 0; j < m_aveNumbList.size(); ++j) {
            QLabel* aveLabel = new QLabel(aveGroupBox);
            if (m_isEMAList[j]) {
                aveLabel->setObjectName(QString("EMA_%1_Label").arg(m_aveNumbList[j]));
                aveLabel->setText(QString("E%1: ").arg(m_aveNumbList[j]));
            } else {
                aveLabel->setObjectName(QString("MA_%1_Label").arg(m_aveNumbList[j]));
                aveLabel->setText(QString("A%1: ").arg(m_aveNumbList[j]));
            }
            x_pos = x_pos + width + x_space;

            if (lableIndex % m_labelColNumb == 0) {
                x_pos = x_start;
                y_pos += height + y_space;
            }
//            if (j >= (m_aveNumbList.size()+3)/2-2 && isChangeLine == false && m_timeTypeList.size() > 1) {
//                x_pos = x_start;
//                y_pos += height + y_space;
//                isChangeLine = true;
//            }
            aveLabel->setGeometry(QRect(x_pos, y_pos, width, height));
            m_aveChartlabelListList[i].append(aveLabel);
            ++lableIndex;
        }

        x_pos = x_start;
        y_pos = y_start;
        QLabel* mainCSSLabel = new QLabel(cssGroupBox);
        mainCSSLabel->setObjectName(QString("mainCSS_Label"));
        mainCSSLabel->setGeometry(QRect(x_pos, y_pos, width+40, height));
        mainCSSLabel->setText(QString("主%1: ").arg(m_mainAveNumb));
        m_cssChartlabelListList[i].append(mainCSSLabel);

        QLabel* subCSSLabel = new QLabel(cssGroupBox);
        subCSSLabel->setObjectName(QString("subCSS_Label"));
        x_pos = x_pos + width+40 + x_space;
        subCSSLabel->setGeometry(QRect(x_pos, y_pos, width+40, height));
        subCSSLabel->setText(QString("从%1: ").arg(m_subAveNumb));
        m_cssChartlabelListList[i].append(subCSSLabel);

        QLabel* energyCSSLabel = new QLabel(cssGroupBox);
        energyCSSLabel->setObjectName(QString("energyCSS_Label"));
        x_pos = x_pos + width+40 + x_space;
        energyCSSLabel->setGeometry(QRect(x_pos, y_pos, width+40, height));
        energyCSSLabel->setText(QString("势%1: ").arg(m_energyAveNumb));
        m_cssChartlabelListList[i].append(energyCSSLabel);
    }
}

void CSSChartForm::setColors() {
    for (int i = 0; i < m_timeTypeList.size();++i) {
        for (int j = 0; j < m_aveChartColorList.size(); ++j) {
            m_aveChartlabelListList[i][j]->setStyleSheet(QString("color: rgb(%1, %2, %3);")
                                                         .arg(m_aveChartColorList[j].red())
                                                         .arg(m_aveChartColorList[j].green())
                                                         .arg(m_aveChartColorList[j].blue()));
            if (j -2 >= 0) {
                m_aveLineSeriesList[i][j-2]->setColor(m_aveChartColorList[j]);
            }
        }

        for (int j = 0; j < 3; ++j) {
            m_cssChartlabelListList[i][j]->setStyleSheet(QString("color: rgb(%1, %2, %3)")
                                                         .arg(m_cssChartColorList[j].red())
                                                         .arg(m_cssChartColorList[j].green())
                                                         .arg(m_cssChartColorList[j].blue()));
        }

        m_aveChartLabelSeriesList[i]->setPen(QPen(QBrush(QColor(255,255,255)), 1));
        m_cssChartLabelSeriesList[i]->setPen(QPen(QBrush(QColor(255,255,255)), 1));

        for (int j = 0; j <2; ++j) {
            m_cssLineSeriesList[i][j]->setColor(m_cssChartColorList[j]);
        }

        for (int j=3; j < m_cssChartColorList.size()-2; ++j) {
            if (m_cssMarkValueList[j-3] == 240 || m_cssMarkValueList[j-3] == -240) {
                QPen dashPen(Qt::DashLine);
                dashPen.setColor(m_cssChartColorList[j]);
                m_cssMarkLineSeriesList[i][j-3]->setPen(dashPen);
            }else if (m_cssMarkValueList[j-3] == 265 || m_cssMarkValueList[j-3] == -265) {
                QPen dotPen(Qt::DotLine);
                dotPen.setColor(m_cssChartColorList[j]);
                m_cssMarkLineSeriesList[i][j-3]->setPen(dotPen);
            } else {
                m_cssMarkLineSeriesList[i][j-3]->setColor(m_cssChartColorList[j]);
            }

        }
        m_energySeriesListList[i][0]->barSets()[0]->setColor(m_cssChartColorList[m_cssChartColorList.size()-2]);
        m_energySeriesListList[i][0]->barSets()[0]->setBorderColor(m_cssChartColorList[m_cssChartColorList.size()-2]);
        m_energySeriesListList[i][1]->barSets()[0]->setColor(m_cssChartColorList[m_cssChartColorList.size()-1]);
        m_energySeriesListList[i][1]->barSets()[0]->setBorderColor(m_cssChartColorList[m_cssChartColorList.size()-1]);

        m_energySeriesListList[i][0]->setBarWidth(0);
        m_energySeriesListList[i][1]->setBarWidth(0);
    }
}

void CSSChartForm::setWindowTitleName()
{
    if (m_isPortfolio)
    {
        m_titleName = QString("%1_%2_%3_%4.xlsx").arg(m_portfolioName)
                                               .arg(m_timeTypeList.join("_"))
                                               .arg(m_realStartTime).arg(m_realEndTime);
    }
    else
    {
        if (m_hedgedCodeName.size() > 0)
        {
            m_titleName += QString("%1_%2_%3_%4_%5.xlsx").arg(m_selectCodeName).arg(m_hedgedCodeName)
                                                      .arg(m_timeTypeList.join("_"))
                                                      .arg(m_realStartTime).arg(m_realEndTime);
        }
        else
        {
            m_titleName += QString("%1_%2_%3_%4.xlsx").arg(m_selectCodeName)
                                                   .arg(m_timeTypeList.join("_"))
                                                   .arg(m_realStartTime).arg(m_realEndTime);
        }
    }
    this->setWindowTitle(m_titleName);
}

void CSSChartForm::connectMarkers()
{
    // Connect all markers to handler
    for (QChart* chart: m_aveChartList) {
        foreach (QLegendMarker* marker, chart->legend()->markers()) {
            // Disconnect possible existing connection to avoid multiple connections
            QObject::disconnect(marker, SIGNAL(clicked()),
                                this, SLOT(handleMarkerClicked()));
            QObject::connect(marker, SIGNAL(clicked()),
                             this, SLOT(handleMarkerClicked()));
        }
    }

    for (QChart* chart: m_cssChartList) {
        foreach (QLegendMarker* marker, chart->legend()->markers()) {
            // Disconnect possible existing connection to avoid multiple connections
            QObject::disconnect(marker, SIGNAL(clicked()),
                                this, SLOT(handleMarkerClicked()));
            QObject::connect(marker, SIGNAL(clicked()),
                             this, SLOT(handleMarkerClicked()));
        }
    }
}

void CSSChartForm::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);

    marker->series()->setVisible(!marker->series()->isVisible());

    marker->setVisible(true);
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
}

void CSSChartForm::setPropertyValue(int index, int dataID) {
    if (index >=0 && index < m_timeList[dataID].size()) {
        m_aveChartlabelListList[dataID][1]->setText(QString("T: %1").arg(m_timeList[dataID][index]));
        m_aveChartlabelListList[dataID][2]->setText(QString("C: %1").arg(m_aveList[dataID][0][index]));
        for (int i = 0; i < m_aveNumbList.size(); ++i) {
            if (m_isEMAList[i]) {
                m_aveChartlabelListList[dataID][i+3]->setText(QString("E%1: %2")
                                                        .arg(m_aveNumbList[i]).arg(m_aveList[dataID][i+1][index]));
            } else {
                m_aveChartlabelListList[dataID][i+3]->setText(QString("A%1: %2")
                                                        .arg(m_aveNumbList[i]).arg(m_aveList[dataID][i+1][index]));
            }
        }

        m_cssChartlabelListList[dataID][0]->setText(QString("主%1: %2")
                                                     .arg(m_mainAveNumb).arg(m_cssList[dataID][0][index]));
        m_cssChartlabelListList[dataID][1]->setText(QString("从%1: %2")
                                                     .arg(m_subAveNumb).arg(m_cssList[dataID][1][index]));
        m_cssChartlabelListList[dataID][2]->setText(QString("势%1: %2")
                                                     .arg(m_energyAveNumb).arg(m_cssList[dataID][2][index]));

        updateLabelSeries(index, dataID);
    }
}

void CSSChartForm::updateLabelSeries(int index, int dataID) {
    for (QPointF point: m_cssChartLabelSeriesList[dataID]->points()) {
        m_cssChartLabelSeriesList[dataID]->remove(point);
    }
    for (QPointF point:  m_aveChartLabelSeriesList[dataID]->points()) {
         m_aveChartLabelSeriesList[dataID]->remove(point);
    }

    m_cssChartLabelSeriesList[dataID]->append(index, m_maxCSS);
    m_cssChartLabelSeriesList[dataID]->append(index, m_minCSS);

    m_aveChartLabelSeriesList[dataID]->append(index, m_aveChartRange[dataID][0]);
    m_aveChartLabelSeriesList[dataID]->append(index, m_aveChartRange[dataID][1]);

    m_oldLabelIndexList[dataID] = index;
    update();
}

void CSSChartForm::mouseMoveEvenFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    int currIndex = -1;
    int dataID = -1;
    for (int i = 0; i < m_timeTypeList.size(); ++i) {
        if (watched == m_aveChartViewList[i]) {
            QPointF curAveChartChartPoint = m_aveChartList[i]->mapToValue (curPoint);
            currIndex = qFloor(curAveChartChartPoint.x());
            dataID = i;
            break;
        }
        if (watched == m_cssChartViewList[i]) {
            QPointF curCSSChartChartPoint = m_cssChartList[i]->mapToValue (curPoint);
            currIndex = qFloor(curCSSChartChartPoint.x());
            dataID = i;
            break;
        }
    }

    if (dataID != -1 && m_isKeyMoveList[dataID] == false) {
        setPropertyValue(currIndex, dataID);
    }else if (m_isKeyMoveList[dataID] == true) {
        m_isKeyMoveList[dataID] = false;
    }
}

void CSSChartForm::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {
    QMouseEvent *mouseEvent = (QMouseEvent *)event;
    QPoint curPoint = mouseEvent->pos ();
    QPointF transPoint;

    for (int i = 0; i < m_timeTypeList.size(); ++i) {
        m_currTimeIndexList[i] = -1;
        m_mouseInitPosList[i] = QCursor::pos();
        m_keyMoveCountList[i] = 0;
        double deltaInGlobalPointAndChartPoint = 0;
        if (watched == m_aveChartViewList[i]) {
            QPointF aveChartPoint = m_aveChartList[i]->mapToValue (curPoint);
            m_currTimeIndexList[i] = qFloor(aveChartPoint.x());
            if (m_currTimeIndexList[i] >= 0 && m_currTimeIndexList[i] < m_timeList[i].size()) {
                transPoint = m_aveChartList[i]->mapToPosition( (QPointF(m_currTimeIndexList[i], m_aveList[i][0][m_currTimeIndexList[i]])));
                deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
            }
        }

        if (watched == m_cssChartViewList[i]) {
            QPointF cssChartPoint = m_cssChartList[i]->mapToValue (curPoint);
            m_currTimeIndexList[i] = qFloor(cssChartPoint.x());
            if (m_currTimeIndexList[i] >= 0 && m_currTimeIndexList[i] < m_timeList[i].size()) {
                transPoint = m_cssChartList[i]->mapToPosition( (QPointF(m_currTimeIndexList[i], m_cssList[i][0][m_currTimeIndexList[i]])));
                deltaInGlobalPointAndChartPoint = transPoint.x() - curPoint.x();
            }
        }

        if (m_currTimeIndexList[i] >= 0 && m_currTimeIndexList[i] < m_timeList[i].size()) {
            m_isKeyMoveList[i] = true;
            setPropertyValue(m_currTimeIndexList[i], i);
            m_mouseInitPosList[i].setX(m_mouseInitPosList[i].x() + deltaInGlobalPointAndChartPoint);
            QCursor::setPos(m_mouseInitPosList[i]);
            m_currFoucusChartID = i;
            qDebug() << "m_currFoucusChartID: " << i;
            break;
        }
    }
}

void CSSChartForm::KeyReleaseFunc(QEvent *event) {
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

void CSSChartForm::moveMouse(int step) {
    double pointXDistance = getPointXDistance();

    if (step != 0) {
        m_keyMoveCountList[m_currFoucusChartID] += step;
        m_currTimeIndexList[m_currFoucusChartID] += step;
        float move_distance = pointXDistance * m_keyMoveCountList[m_currFoucusChartID];

        m_isKeyMoveList[m_currFoucusChartID] = true;
        setPropertyValue(m_currTimeIndexList[m_currFoucusChartID], m_currFoucusChartID);

        if (move_distance >= 1 || move_distance <=-1 || move_distance == 0) {
            QCursor::setPos(m_mouseInitPosList[m_currFoucusChartID].x() + move_distance, m_mouseInitPosList[m_currFoucusChartID].y());
        }
    }
}

double CSSChartForm::getPointXDistance() {
    int testIndex = 0;
    QPointF pointa = m_aveChartList[m_currFoucusChartID]->mapToPosition(
                     QPointF(testIndex, m_aveList[m_currFoucusChartID][0][testIndex]));
    QPointF pointb = m_aveChartList[m_currFoucusChartID]->mapToPosition(
                      QPointF(testIndex+1, m_aveList[m_currFoucusChartID][0][testIndex+1]));
    double distance = pointb.x() - pointa.x();
    return distance;
}

QList<QMyChartView*> CSSChartForm::getChartViewList() {
    QList<QMyChartView*> result;
    for (auto series: m_aveChartViewList) {
        result.append(series);
    }
    for (auto series: m_cssChartViewList) {
        result.append(series);
    }
    return result;
}

QString CSSChartForm::getExcelFileName(QStringList keyValueList, QString fileDir) {
    QString fileName = fileDir;
    fileName += m_titleName;
//    if (m_isPortfolio)
//    {
//        fileName += QString("%1_%2_%3_%4.xlsx").arg(m_portfolioName)
//                                               .arg(m_timeTypeList.join("_"))
//                                               .arg(m_startDate).arg(m_endDate);
//    }
//    else
//    {
//        if (m_hedgedCodeName.size() > 0)
//        {
//            fileName += QString("%1_%2_%3_%4.xlsx").arg(m_selectCodeName)
//                                                   .arg(m_timeTypeList.join("_"))
//                                                   .arg(m_startDate).arg(m_endDate);
//        }
//        else
//        {
//            fileName += QString("%1_%2_%3_%4_%5.xlsx").arg(m_selectCodeName).arg(m_hedgedCodeName)
//                                                      .arg(m_timeTypeList.join("_"))
//                                                      .arg(m_startDate).arg(m_endDate);
//        }
//    }
    keyValueList;
    return fileName;
}

void CSSChartForm::getChoosenInfo_slot(QStringList choosenKeyValueList, QString fileDir, bool bOpenDesFile) {
    QString fileName = getExcelFileName(choosenKeyValueList, fileDir);

    qDebug() << "fileName: " << fileName;
    int sumResult = 1;
    /**/
    for (int i = 0; i < m_timeTypeList.size(); ++i)
    {
        QString sheetName = m_timeTypeList[i];
        QList<QStringList> currSheetData;

        currSheetData.append(m_timeListStore[i]);
        currSheetData[0].insert(0, "时间");

        for (int j = 0; j < m_aveListStore[i].size(); ++j) {
            QStringList stringListData;
            for (double data : m_aveListStore[i][j]) {
                stringListData.append(QString("%1").arg(data));
            }
            if(j == 0) {
                stringListData.insert(0, "收盘价");
            } else {
                if (m_isEMAList[j-1] == true) {
                    stringListData.insert(0, QString("EMA_%1").arg(m_aveNumbList[j-1]));
                } else {
                    stringListData.insert(0, QString("MA_%1").arg(m_aveNumbList[j-1]));
                }
            }
            currSheetData.append(stringListData);
        }

        for (int j = 0; j < m_cssListStore[i].size(); ++j) {
            QStringList stringListData;
            for (double data : m_cssListStore[i][j]) {
                stringListData.append(QString("%1").arg(data));
            }
            switch (j) {
            case 0:
                stringListData.insert(0, "主指标");
                break;
            case 1:
                stringListData.insert(0, "从指标");
                break;
            case 2:
                stringListData.insert(0, "势能指标");
                break;
            }
            currSheetData.append(stringListData);
        }

        if (m_isPortfolio)
        {
            QStringList stringListData;
            for (double data : m_indexDataListStore[i]) {
                stringListData.append(QString("%1").arg(data));
            }
            stringListData.insert(0, m_hedgeIndexCode);
            currSheetData.append(stringListData);

            stringListData.clear();
            for (double data : m_hedgedDataListStore[i]) {
                stringListData.append(QString("%1").arg(data));
            }
            stringListData.insert(0, QString("对冲值"));
            currSheetData.append(stringListData);

        }

        int result = writeMatrixData(fileName, currSheetData, sheetName, true, false);
        sumResult *= result;
    }

    if (sumResult == 1) {
        if (true == bOpenDesFile) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        } else {
            QMessageBox::information(NULL, "信息", QString("文件 %1 写入成功").arg(fileName));
        }
    } else {
        QMessageBox::critical(NULL, "错误", QString("文件 %1 写入失败").arg(fileName));
    }

}

