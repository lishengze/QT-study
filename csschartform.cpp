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
using std::max;

#include <QtMath>

#pragma execution_character_set("utf-8")

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
    initCommonData();
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

        m_keyMoveCountList.append(0);
        m_currTimeIndexList.append(0);
        m_isKeyMoveList.append(false);
        m_mouseInitPosList.append(QPoint(-1, -1));
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
                                                       m_energyCssRate1, m_energyCssRate2,
                                                       m_maxCSS, m_minCSS, i);
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

    m_cssChartColorList.append(QColor(253,216,53));   // -200
    m_cssChartColorList.append(QColor(117,117,117));  // -158
    m_cssChartColorList.append(QColor(229,115,115));  // -130
    m_cssChartColorList.append(QColor(211,47,47));  // -80
    m_cssChartColorList.append(QColor(0,172,193));  // 80
    m_cssChartColorList.append(QColor(0,172,193));  // 130
    m_cssChartColorList.append(QColor(117,117,117));  // 158
    m_cssChartColorList.append(QColor(253,216,53));  // 200
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
    qDebug() << "CSSChartFormOne::sendCSSData_slot: " << dataID;
    int startPos = getStartIndex(m_startDate, timeList);

    m_timeList[dataID] = getSubList(timeList, startPos, timeList.size());
    for (int i = 0; i < aveList.size(); ++i) {
        m_aveList[dataID].append(getSubList(aveList[i], startPos, aveList[i].size()));
    }
    for (int i = 0; i < cssList.size(); ++i) {
        m_cssList[dataID].append(getSubList(cssList[i], startPos, cssList[i].size()));
    }

    setChart(dataID);
    initTheme();

    m_currDataIDList.append(dataID);
    if (m_currDataIDList.size() == m_timeTypeList.size()) {
        setChartView();
        setLabels();
        setColors();
        connectMarkers();
    }
}

void CSSChartForm::setChart(int dataID) {
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
       /**/
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
    ui->main_gridLayout->setRowStretch(0,2);
    ui->main_gridLayout->setRowStretch(1,20);
    ui->main_gridLayout->setRowStretch(2,1);
    ui->main_gridLayout->setRowStretch(3,14);
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

        QLabel* dataFreqLabel = new QLabel(aveGroupBox);
        dataFreqLabel->setObjectName(QStringLiteral("dataFreq_Label"));
        dataFreqLabel->setGeometry(QRect(x_pos, y_pos, 50, height));
        dataFreqLabel->setText(QString("F: %1").arg(m_timeTypeList[i]));
        m_aveChartlabelListList[i].append(dataFreqLabel);

        QLabel* timeLabel = new QLabel(aveGroupBox);
        timeLabel->setObjectName(QStringLiteral("time_Label"));
        x_pos = x_pos + 50 + x_space;
        timeLabel->setGeometry(QRect(x_pos, y_pos, 120, height));
        timeLabel->setText("T:");
        m_aveChartlabelListList[i].append(timeLabel);

        QLabel* closeLabel = new QLabel(aveGroupBox);
        closeLabel->setObjectName(QStringLiteral("close_Label"));
        x_pos = x_pos + 120 + x_space;
        closeLabel->setGeometry(QRect(x_pos, y_pos, width, height));
        closeLabel->setText("C:");
        m_aveChartlabelListList[i].append(closeLabel);

        bool isChangeLine = false;
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
            if (j >= (m_aveNumbList.size()+3)/2-2 && isChangeLine == false) {
                x_pos = x_start;
                y_pos += height + y_space;
                isChangeLine = true;
            }
            aveLabel->setGeometry(QRect(x_pos, y_pos, width, height));
            m_aveChartlabelListList[i].append(aveLabel);
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

        for (int j = 0; j <2; ++j) {
            m_cssLineSeriesList[i][j]->setColor(m_cssChartColorList[j]);
        }

        for (int j=3; j < m_cssChartColorList.size()-2; ++j) {
            m_cssMarkLineSeriesList[i][j-3]->setColor(m_cssChartColorList[j]);
        }
        m_energySeriesListList[i][0]->barSets()[0]->setColor(m_cssChartColorList[m_cssChartColorList.size()-2]);
        m_energySeriesListList[i][0]->barSets()[0]->setBorderColor(m_cssChartColorList[m_cssChartColorList.size()-2]);
        m_energySeriesListList[i][1]->barSets()[0]->setColor(m_cssChartColorList[m_cssChartColorList.size()-1]);
        m_energySeriesListList[i][1]->barSets()[0]->setBorderColor(m_cssChartColorList[m_cssChartColorList.size()-1]);
    }
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
    }
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
