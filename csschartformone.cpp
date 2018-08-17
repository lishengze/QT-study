#include "csschartformone.h"
#include "ui_csschartformone.h"
#include "process_data_func.h"
#include "io_func.h"
#include <QMouseEvent>
#include <QtMath>
#include <QLegendMarker>
#include <QChartView>
#include <QBarSet>

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
    m_chartXaxisTickCount = 5;
    m_keyMoveCount = 0;
    m_currTimeIndex = 0;
    m_isKeyMove = false;
    m_mouseInitPos = QPoint(-1, -1);
    m_oldPointDistance = -1;
    m_cssMarkValueList << -200 << 200 << -158 << 158 << -130 << 130 << -80 << 80;
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
    this->setWindowTitle(QString("%1").arg(m_singleCodeName));
    ui->title_Label->setText(QString("%1: [%2, %3] 指标图")
                             .arg(m_singleCodeName).arg(m_startDate).arg(m_endDate));
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

void CSSChartFormOne::initChartView() {
    QCategoryAxis* timeAxisX = getTimeAxisX(m_timeList, m_chartXaxisTickCount);
    QValueAxis* aveAxisY = new QValueAxis;
    QList<double> aveRange =getChartYvalueRange(m_aveList);
    aveAxisY->setRange(aveRange[0], aveRange[1]);

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

    m_aveChart = new QChart();
    for (auto lineSeries:m_aveLineSeries) {
        m_aveChart->addSeries(lineSeries);
    }

    m_aveChartView = new QMyChartView(m_aveChart);
    m_aveChartView->setRenderHint(QPainter::Antialiasing);
    m_aveChartView->installEventFilter(this);
    m_aveChartView->setMouseTracking(true);

    m_aveChart->setAnimationOptions(QChart::NoAnimation);
    m_aveChart->addAxis(aveAxisY, Qt::AlignLeft);
    for (auto lineSeries:m_aveLineSeries) {
        lineSeries->attachAxis(aveAxisY);
    }

    QValueAxis* cssAxisY = new QValueAxis;
    cssAxisY->setRange(m_minCSS, m_maxCSS);

    QList<int> aveList;
    aveList << m_mainAveNumb << m_subAveNumb;
    for (int i = 0; i < 2; ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = aveList[i]; j < m_cssList[i].size(); ++j) {
            currSeries->append(j, m_cssList[i][j]);
        }
        currSeries->setUseOpenGL(true);
        m_cssLineSeries.append(currSeries);
    }
    m_cssLineSeries[0]->setName("主指标");
    m_cssLineSeries[1]->setName("从指标");

    m_energySeries = new QStackedBarSeries;
    QBarSet *energySet = new QBarSet("势能指标");
    for (int i = 0; i < m_cssList[2].size(); ++i) {
        energySet->append(m_cssList[2][i]);
    }
    m_energySeries->append(energySet);
    m_energySeries->setName("势能指标");

    for (int i = 0; i < m_cssMarkValueList.size(); ++i) {
        QLineSeries* currSeries = new QLineSeries;
        for (int j = 0; j < m_timeList.size(); ++j) {
            currSeries->append(j, m_cssMarkValueList[i]);
        }
        currSeries->setUseOpenGL(true);
        m_cssMarkLineSeries.append(currSeries);
        currSeries->setName(QString("%1").arg(m_cssMarkValueList[i]));
    }

    m_cssChart = new QChart();
    for (auto lineSeries:m_cssLineSeries) {
        m_cssChart->addSeries(lineSeries);
    }
    m_cssChart->addSeries(m_energySeries);
    for (auto lineSeries:m_cssMarkLineSeries) {
        m_cssChart->addSeries(lineSeries);
    }

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
    m_energySeries->attachAxis(timeAxisX);
    m_energySeries->attachAxis(cssAxisY);
}

void CSSChartFormOne::sendCSSData_slot(QList<QString> timeList, QList<QList<double>> aveList,
                                       QList<QList<double>> cssList) {

    qDebug() << "CSSChartFormOne::sendCSSData_slot";

    m_timeList = timeList;
    m_aveList = aveList;
    m_cssList = cssList;

//    qDebug() << "m_timeList.size: " << m_timeList.size();
//    for (int i =0; i < m_aveList.size(); ++i) {
//        qDebug() << QString("m_aveList[%1].size: %2").arg(i).arg(m_aveList[i].size());
//    }
//    for (int i =0; i < m_cssList.size(); ++i) {
//        qDebug() << QString("m_cssList[%1].size: %2").arg(i).arg(m_cssList[i].size());
//    }
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

void CSSChartFormOne::closeEvent(QCloseEvent *event) {

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
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
    foreach (QLegendMarker* marker, m_cssChart->legend()->markers()) {
        QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(handleMarkerClicked()));
    }
}

void CSSChartFormOne::handleMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());
    Q_ASSERT(marker);
//    qDebug() << "handleMarkerClicked";

    switch (marker->type())
    {
        case QLegendMarker::LegendMarkerTypeXY:
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
}

