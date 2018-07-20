#include "indexchart.h"
#include "ui_indexchart.h"

IndexChart::IndexChart(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IndexChart)
{
    ui->setupUi(this);
}

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
    m_aveNumb(aveNumb), m_css12Rate(css12Rate),
    m_cssRate1(cssRate1), m_cssRate2(cssRate2),
    m_maxCSS(maxCSS), m_minCSS(minCSS),
    QMainWindow(parent), ui(new Ui::IndexChart) {

    ui->setupUi(this);
    registSignalParamsType();
    initHistDataWorker();

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

void IndexChart::initHistDataWorker() {
    m_histDataWorker = new HistoryData(m_chartViewID, m_dbhost, m_databaseName,
                                       m_selectIndex, m_hedgedIndex,
                                       m_startDate, m_endDate,
                                       m_aveNumb, m_css12Rate,
                                       m_cssRate1, m_cssRate2,
                                       m_maxCSS, m_minCSS);

    connect(this, SIGNAL(getIndexHistData_signal()),
            m_histDataWorker, SLOT(getIndexHistData_slot()));

    connect(m_histDataWorker, SIGNAL(sendHistIndexData_signal(QList<QStringList>)),
            this, SLOT(sendHistIndexData_slot()));

    connect(m_histDataWorker, SIGNAL(sendHistIndexError_signal(QString)),
            this, SLOT(sendHistIndexError_slot(QString)));

    m_histDataWorker->moveToThread(&m_histDataThread);
    connect(&m_histDataThread, SIGNAL(finished()),
            m_histDataWorker, SLOT(deleteLater()));

    m_histDataThread.start();
}

void IndexChart::initMonitorWorker() {

}

void IndexChart::registSignalParamsType() {
    qRegisterMetaType<QList<QStringList>>("(QList<QStringList>");
}

void IndexChart::sendHistIndexData_slot(QList<QStringList> result) {

}

void IndexChart::sendHistIndexError_slot(QString) {

}

void IndexChart::sendRealTimeIndexData_slot(QString time, double earningValue) {

}
