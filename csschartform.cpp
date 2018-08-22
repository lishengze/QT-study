#include "csschartform.h"
#include "ui_csschartform.h"
#include "process_data_func.h"
#include "io_func.h"
#include <QMouseEvent>
#include <QtMath>
#include <QLegendMarker>
#include <QChartView>
#include <QBarSet>
#include <QColor>
#include <algorithm>
#include "time_func.h"
#include "compute_func.h"

#include <QtMath>

//CSSChartForm::CSSChartForm(QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::CSSChartForm)
//{
//    ui->setupUi(this);
//}

CSSChartForm::CSSChartForm(int chartID, QString dbhost, QStringList timeTypeList,
                                QString startDate, QString endDate, QString codeName,
                                QList<int> aveNumbList, QList<bool> isEMAList,
                                int mainAveNumb, int subAveNumb, int energyAveNumb,
                                double css12Rate, double mainCssRate1, double mainCssRate2,
                                double energyCssRate1, double energyCssRate2,
                                double maxCSS, double minCSS,
                                QWidget *parent):
                                m_dbhost(dbhost), m_timeTypeList(timeTypeList),
                                m_startDate(startDate), m_endDate(endDate), m_singleCodeName(codeName),
                                m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
                                m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
                                m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
                                m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
                                m_maxCSS(maxCSS), m_minCSS(minCSS),
                                BaseChart(chartID, parent),ui(new Ui::CSSChartForm)
{
    ui->setupUi(this);
    initLayout();
    initCommonData();
    initHistoryData();
//    registSignalParamsType();
//    startGetData();
}

CSSChartForm::~CSSChartForm()
{
    delete ui;
}

void CSSChartForm::initLayout() {
    m_mainGridLayout = new QGridLayout(this);
    for (int i = 0; i < m_timeTypeList.size(); ++i) {
        QGridLayout* gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString("gridLayout_%1").arg(i));
        m_gridLayoutList.append(gridLayout);
    }

    if (m_gridLayoutList.size() > 3) {
        int rowNumb = 2;
        int rows = qCeil(double(m_gridLayoutList.size()) / double(rowNumb));
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < rowNumb; ++j) {
                if (i * rowNumb + j >= m_gridLayoutList.size()) {
                    break;
                } else {
                    m_mainGridLayout->addLayout(m_gridLayoutList[i*rowNumb + j], i, j, 1, 1);
                }
            }
        }
    } else {
        for (int i = 0; i < m_gridLayoutList.size(); ++i) {
            m_mainGridLayout->addLayout(m_gridLayoutList[i], 0,i,1,1);
        }
    }
}

void CSSChartForm::initCommonData() {
    m_chartXaxisTickCount = 5;
    m_cssMarkValueList << -200  << -158  << -130 << -80 << 80 << 130 << 158<< 200<< 300;
}

void CSSChartForm::initListData() {
    for (auto timeType:m_timeTypeList) {
        m_databaseNameList.append(QString("MarketData_%1").arg(timeType));
        QList<QList<double>> emptyAVEData;
        QList<QList<double>> emptyCSSData;
        QList<QString> emptyTimeData;
        m_aveList.append(emptyAVEData);
        m_cssList.append(emptyCSSData);
        m_timeList.append(emptyTimeData);

        QList<QLineSeries*> emptyLineSeries;
        m_aveLineSeriesList.append(emptyLineSeries);
        m_cssLineSeriesList.append(emptyLineSeries);
        m_cssMarkLineSeriesList.append(emptyLineSeries);

        QList<QStackedBarSeries*> emptyStackedBarSeries;
        m_energySeriesListList.append(emptyStackedBarSeries);

        QMyChartView* emptyMyChartView;
        m_aveChartViewList.append(emptyMyChartView);
        m_cssChartViewList.append(emptyMyChartView);

        QChart* emptyChart;
        m_aveChartList.append(emptyChart);
        m_cssChartList.append(emptyChart);
    }
}

void CSSChartForm::initHistoryData() {
    for (int i = 0; i < m_timeTypeList.size(); ++i) {
        QString timeType = m_timeTypeList[i];
        QString databaseName = m_databaseNameList[i];
        int maxPreTimeNumb = Max(m_aveNumbList, 0, m_aveNumbList.size());
        QString newStartDate = getPreDate(m_startDate, timeType, maxPreTimeNumb);
        qDebug() << QString("maxPreTimeNumb: %1, newStartDate: %2").arg(maxPreTimeNumb).arg(newStartDate);
        HistoryData* histdataWorker = new HistoryData(m_dbhost, databaseName, newStartDate, m_endDate,
                                                       m_singleCodeName, m_aveNumbList, m_isEMAList,
                                                       m_mainAveNumb, m_subAveNumb, m_energyAveNumb,
                                                       m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                                       m_energyCssRate1, m_energyCssRate2, i,
                                                       m_maxCSS, m_minCSS);
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

void CSSChartForm::registSignalParamsType() {
    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
}

void CSSChartForm::sendCSSData_slot(QList<QString> timeList, QList<QList<double>> aveList,
                                    QList<QList<double>> cssList, int dataID) {
    QMutexLocker locker(&m_mutex);
    qDebug() << "CSSChartFormOne::sendCSSData_slot";
    int startPos = getStartIndex(m_startDate, timeList);

    qDebug() << QString("startPos: %1").arg(startPos);
    m_timeList[dataID] = getSubList(timeList, startPos, timeList.size());
    for (int i = 0; i < aveList.size(); ++i) {
        m_aveList[dataID].append(getSubList(aveList[i], startPos, aveList[i].size()));
    }
    for (int i = 0; i < cssList.size(); ++i) {
        m_cssList[dataID].append(getSubList(cssList[i], startPos, cssList[i].size()));
    }
}

void CSSChartForm::setChartView(int dataID) {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList[dataID], m_chartXaxisTickCount);
    QValueAxis* aveAxisY = new QValueAxis;
    QList<double> aveRange =getChartYvalueRange(m_aveList[dataID]);
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

    QChart* aveChart = new QChart();
    for (auto lineSeries:m_aveLineSeriesList[dataID]) {
        aveChart->addSeries(lineSeries);
    }

    QMyChartView* aveChartView = new QMyChartView(aveChart);
    aveChartView->setRenderHint(QPainter::Antialiasing);
    aveChartView->installEventFilter(this);
    aveChartView->setMouseTracking(true);

    aveChart->setAnimationOptions(QChart::NoAnimation);
    aveChart->addAxis(aveAxisY, Qt::AlignLeft);
    for (auto lineSeries:m_aveLineSeriesList[dataID]) {
        lineSeries->attachAxis(aveAxisY);
    }

    m_aveChartViewList[dataID] = aveChartView;
    m_aveChartList[dataID] = aveChart;

    QValueAxis* cssAxisY = new QValueAxis;
    cssAxisY->setRange(m_minCSS, m_maxCSS);
    cssAxisY->setGridLineVisible(false);
    cssAxisY->setLineVisible(true);

    QList<int> aveList;
    aveList << m_mainAveNumb << m_subAveNumb;
    for (int i = 0; i < 2; ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = aveList[i]; j < m_cssList[i].size(); ++j) {
            currSeries->append(j, m_cssList[dataID][i][j]);
        }
        currSeries->setUseOpenGL(true);
        m_cssLineSeriesList[dataID].append(currSeries);
    }
    m_cssLineSeriesList[dataID][0]->setName("主指标");
    m_cssLineSeriesList[dataID][1]->setName("从指标");

    QBarSet *energySetUp = new QBarSet("势能指标大于0");
    QBarSet *energySetDown = new QBarSet("势能指标小于0");
    for (int i = 0; i < m_cssList[2].size(); ++i) {
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
        for (int j = 0; j < m_timeList.size(); ++j) {
            currSeries->append(j, m_cssMarkValueList[i]);
        }
        currSeries->setUseOpenGL(true);
        m_cssMarkLineSeriesList[dataID].append(currSeries);
        currSeries->setName(QString("%1").arg(m_cssMarkValueList[i]));
    }

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
    m_cssChartViewList[dataID] = cssChartView;
    m_cssChartList[dataID] = cssChart;
}
