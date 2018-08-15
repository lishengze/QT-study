#include "csschartformone.h"
#include "ui_csschartformone.h"
#include "process_data_func.h"

#pragma execution_character_set("utf-8")

//CSSChartFormOne::CSSChartFormOne(QWidget *parent) :
//    BaseChart(parent),ui(new Ui::CSSChartFormOne)
//{
//    ui->setupUi(this);
//}

CSSChartFormOne::CSSChartFormOne(QString dbhost, QString databaseName,
                                QString startDate, QString endDate, QString codeName,
                                QList<int> aveNumbList, QList<bool> isEMAList,
                                int mainAveNumb, int subAveNumb, int energyAveNumb,
                                double css12Rate, double mainCssRate1, double mainCssRate2,
                                double energyCssRate1, double energyCssRate2,
                                double maxCSS, double minCSS,
                                QWidget *parent):
                        m_dbhost(dbhost), m_databaseName(databaseName),
                        m_startDate(startDate), m_endDate(endDate), m_singleCodeName(codeName),
                        m_aveNumbList(aveNumbList), m_isEMAList(isEMAList),
                        m_mainAveNumb(mainAveNumb), m_subAveNumb(subAveNumb), m_energyAveNumb(energyAveNumb),
                        m_css12Rate(css12Rate), m_mainCssRate1(mainCssRate1), m_mainCssRate2(mainCssRate2),
                        m_energyCssRate1(energyCssRate1), m_energyCssRate2(energyCssRate2),
                        m_maxCSS(maxCSS), m_minCSS(minCSS),
                        BaseChart(parent),ui(new Ui::CSSChartFormOne)
{
    ui->setupUi(this);
    initHistoryData();
    startGetData();
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

void CSSChartFormOne::initHistoryData() {
    m_histdataWorker = new HistoryData(m_dbhost, m_databaseName, m_startDate, m_endDate,
                                       m_singleCodeName, m_aveNumbList, m_isEMAList,
                                       m_mainAveNumb, m_subAveNumb, m_energyAveNumb,
                                       m_css12Rate, m_mainCssRate1, m_mainCssRate2,
                                       m_energyCssRate1, m_energyCssRate2,
                                       m_maxCSS, m_minCSS);

    connect(&m_histdataThread, SIGNAL(finished()),
            m_histdataWorker, SLOT(deleteLater()));

    connect(this, SIGNAL(getCSSData_signal()),
            m_histdataWorker, SLOT(getCSSData_slot()));

    connect(m_histdataWorker, SIGNAL(sendCSSData_signal(QList<QString>, QList<QList<double>>,  QList<QList<double>>)),
            this, SLOT(sendCSSData_slot(QList<QString>, QList<QList<double>>,  QList<QList<double>>)));

    m_histdataWorker->moveToThread(&m_histdataThread);

    m_histdataThread.start();
}

void CSSChartFormOne::initLayout() {
//    this->setWindowTitle(QString("%1").arg(m_selectIndex));
//    ui->title_label->setText(QString("指数: %1 与指数: %2, %3频, 实时对冲图")
//                             .arg(m_selectIndex).arg(m_hedgedIndex).arg(m_timeType));
    initChartView();

//    initTableContextMenu();

    if (m_aveChartView != NULL) {
        ui->gridLayout->addWidget(m_aveChartView, 1, 0);
    }

    if (m_aveChartView != NULL) {
        ui->gridLayout->addWidget(m_cssChartView, 2, 0);
    }
    this->setMouseTracking(true);

    initTheme();
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

void CSSChartFormOne::initChartView() {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList, m_chartXaxisTickCount);
    QValueAxis* aveAxisY = new QValueAxis;
    QList<double> aveRange =getChartYvalueRange(m_aveList);
    aveAxisY->setRange(aveRange[0], aveRange[1]);

    QValueAxis* cssAxisY = new QValueAxis;
    QList<double> cssRange =getChartYvalueRange(m_cssList);
    cssAxisY->setRange(cssRange[0], cssRange[1]);

    for (int i = 0; i < m_aveNumbList.size(); ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_aveList[i].size(); ++j) {
            currSeries->append(j, m_aveList[i][j]);
        }
        currSeries->setName(QString("%1").arg(m_aveNumbList[i]));
        currSeries->setUseOpenGL(true);
        currSeries->attachAxis(timeAxisX);
        currSeries->attachAxis(aveAxisY);
        m_aveLineSeries.append(currSeries);
    }

    QList<int> aveList;
    aveList << m_mainAveNumb << m_subAveNumb << m_energyAveNumb;
    for (int i = 0; i < 3; ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_cssList[i].size(); ++j) {
            currSeries->append(j+aveList[j], m_cssList[i][j]);
        }
        currSeries->setUseOpenGL(true);
        currSeries->attachAxis(timeAxisX);
        currSeries->attachAxis(cssAxisY);
        m_cssLineSeries.append(currSeries);
    }
    m_cssLineSeries[0]->setName("主指标");
    m_cssLineSeries[0]->setName("从指标");
    m_cssLineSeries[0]->setName("势能");

    m_aveChart = new QChart();
    for (int i = 0; i < m_aveLineSeries.size(); ++i) {
        m_aveChart->addSeries(m_aveLineSeries[i]);
    }
    m_aveChart->setAnimationOptions(QChart::NoAnimation);
    m_aveChart->addAxis(timeAxisX, Qt::AlignBottom);
    m_aveChart->addAxis(aveAxisY, Qt::AlignLeft);

    m_aveChartView = new QMyChartView(m_aveChart);
    m_aveChartView->setRenderHint(QPainter::Antialiasing);
    m_aveChartView->installEventFilter(this);
    m_aveChartView->setMouseTracking(true);

    m_cssChart = new QChart();
    for (int i = 0; i < m_cssLineSeries.size(); ++i) {
        m_cssChart->addSeries(m_cssLineSeries[i]);
    }
    m_cssChart->setAnimationOptions(QChart::NoAnimation);
    m_cssChart->addAxis(timeAxisX, Qt::AlignBottom);
    m_cssChart->addAxis(cssAxisY, Qt::AlignLeft);

    m_cssChartView = new QMyChartView(m_cssChart);
    m_cssChartView->setRenderHint(QPainter::Antialiasing);
    m_cssChartView->installEventFilter(this);
    m_cssChartView->setMouseTracking(true);

}

void CSSChartFormOne::sendCSSData_slot(QList<QString> timeList, QList<QList<double>> aveList,
                                       QList<QList<double>> cssList) {

    m_timeList = timeList;
    m_aveList = aveList;
    m_cssList = cssList;

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

void CSSChartFormOne::updateAxis() {

}

void CSSChartFormOne::updateSeries() {

}

void CSSChartFormOne::updateMousePos() {

}

void CSSChartFormOne::setPropertyValue(int index) {

}

void CSSChartFormOne::mouseMoveEvenFunc(QObject *watched, QEvent *event) {

}

void CSSChartFormOne::mouseButtonReleaseFunc(QObject *watched, QEvent *event) {

}

void CSSChartFormOne::KeyReleaseFunc(QEvent *event) {

}

void CSSChartFormOne::moveMouse(int step) {

}

double CSSChartFormOne::getPointXDistance() {
    double result;
    return result;
}

bool CSSChartFormOne::eventFilter (QObject *watched, QEvent *event) {
    bool result;
    return result;
}

void CSSChartFormOne::closeEvent(QCloseEvent *event) {

}
