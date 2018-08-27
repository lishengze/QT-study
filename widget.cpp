#include <QScatterSeries>
#include <QDialog>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QDate>
#include <QTime>
#include <QList>
#include <QPointF>
#include <QDebug>
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QPersistentModelIndex>
#include <QAbstractItemModel>
#include <QVector>
#include <QTimer>
#include <QFileDialog>
#include <QDesktopServices>
#include <QCheckBox>

#include "secode_func.h"
#include "excel_func.h"
#include "widget_func.h"
#include "io_func.h"
#include "toolfunc.h"
#include "process_data_func.h"
#include "widget.h"
#include "ui_widget.h"
#include "QChartView"
#include "indexchart.h"

#include "chartform.h"
#include "announcementform.h"
#include "generateportfolioform.h"
#include "xlsxdocument.h"
#include "futurechart.h"
#include "database.h"
#include "csschartformone.h"
#include "csschartform.h"

QT_CHARTS_USE_NAMESPACE
#pragma execution_character_set("utf-8")

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initCommonData();
    initWidegt();
}

Widget::~Widget()
{
    delete ui;
    for (int i = 0; i < m_chartViews.size (); ++i) {
        if (NULL != m_chartViews[i]) {
            delete m_chartViews[i];
            m_chartViews[i] = NULL;
        }
    }
}

void Widget::initCommonData() {
    m_announcementView = NULL;
    m_dbhostList << "192.168.211.162"
                 << "192.168.211.165";
    m_currDBHost = m_dbhostList[0];
    m_futureIndexCode = "000300";
    setFutureContractList();
    m_isBuySalePortfolio = false;

    initFileDir();
}

void Widget::initFileDir() {
    m_nativeFileName = "./spreadDirInfo.xlsx";
    QFile tmpFile(m_nativeFileName);
    if (!tmpFile.exists()) {
//        qDebug() << QString("%1 does not exit!").arg(m_nativeFileName);
        QXlsx::Document xlsx;
        m_strategyFileDir = QString("//192.168.211.182/it程序设计/strategy/");
        m_buySalePortfolioFileDir = QString("//192.168.211.182/it程序设计/买入卖出组合/");
        xlsx.write("A1", m_strategyFileDir);
        xlsx.write("A2",m_buySalePortfolioFileDir);
        xlsx.saveAs(m_nativeFileName);
    } else {
        QXlsx::Document xlsx(m_nativeFileName);
        xlsx.selectSheet("Sheet1");
        QXlsx::CellRange range = xlsx.dimension();

//        qDebug() <<"fileName: " << m_nativeFileName << "rowCount: "
//                 << range.rowCount() << ", colCount" << range.columnCount();

        if (range.rowCount() == 0) {
            m_strategyFileDir = QString("//192.168.211.182/it程序设计/strategy/");
            m_buySalePortfolioFileDir = QString("//192.168.211.182/it程序设计/买入卖出组合/");
            xlsx.write("A1", m_strategyFileDir);
            xlsx.write("A2",m_buySalePortfolioFileDir);
            xlsx.save();
        } else {
//            qDebug() << QString("%1 exits!").arg(m_nativeFileName);
            m_strategyFileDir = xlsx.cellAt(1, 1)->value().toString();
            m_buySalePortfolioFileDir = xlsx.cellAt(2, 1)->value().toString();
//            qDebug() << "m_strategyFileDir: " << m_strategyFileDir;
//            qDebug() << "m_buySalePortfolioFileDir: " << m_buySalePortfolioFileDir;
        }
    }
}

void Widget::initWidegt() {
    initCalendar();
    initHedgeComboBox();
    initDataFrequency();
    initEnergyDataFreq();
    initMacdTime();
    setFutureContractCombox();
    initDatasourceCombox();
    initSpreadSpinbox();
    initCSSParamComBox();

    initSelectIndexComboBox();
    initHedgedIndexComboBox();
    initEnergyIndexCodeComboBox();
    initIndexAveNumb();

    setFutureContractList();
    setStrategyTable();
    setBuySalePortfolioTable();

    initProgramWorkInfoTableView();
    initTableContextMenu();
}

void Widget::initCalendar () {
    ui->chooseStartDate->setCalendarPopup (true);
    ui->chooseEndDate->setCalendarPopup (true);

    ui->chooseStartDate->setDate (QDate::currentDate().addDays(-30));
    ui->chooseEndDate->setDate (QDate::currentDate());

    QDate today = QDate::currentDate();
    QDate sevenDayAgo = today.addDays(-7);
    ui->AnnouncementStartDate->setDate(sevenDayAgo);
    ui->AnnouncementEndDate->setDate(today);

    ui->chooseIndexStartDate->setCalendarPopup (true);
    ui->chooseIndexEndDate->setCalendarPopup (true);

    ui->chooseIndexStartDate->setDate (QDate::currentDate().addDays(-30));
    ui->chooseIndexEndDate->setDate (QDate::currentDate());

    ui->energyStart_dateEdit->setCalendarPopup (true);
    ui->energyEnd_dateEdit->setCalendarPopup (true);

//    ui->energyStart_dateEdit->setDate (QDate::currentDate().addDays(-365*5));
//    ui->energyEnd_dateEdit->setDate (QDate::currentDate());

    ui->energyStart_dateEdit->setDate (QDate(2011,1,1));
    ui->energyEnd_dateEdit->setDate (QDate(2012,1,1));
}

void Widget::initHedgeComboBox() {
    ui->hedgeTarget_comboBox->setCurrentText (QString("沪深300"));
    ui->hedgeTarget_comboBox->addItem (QString("沪深300"),  "000300");
    ui->hedgeTarget_comboBox->addItem (QString("上证50"),   "000016");
    ui->hedgeTarget_comboBox->addItem (QString("中证100"),  "399903");
    ui->hedgeTarget_comboBox->addItem (QString("中证200"),  "000904");
    ui->hedgeTarget_comboBox->addItem (QString("中证500"),  "000905");
    ui->hedgeTarget_comboBox->addItem (QString("中证800"),  "000906");
    ui->hedgeTarget_comboBox->addItem (QString("中证1000"), "000852");
    ui->hedgeCount_spinBox->setValue (2);
}

void Widget::initDataFrequency() {
    QStringList timeFre;
    timeFre << "5m" << "10m" << "15m" << "30m" << "60m" << "120m" << "day" << "week" << "month";
    ui->index_dataFrequency->addItems (timeFre);
    ui->index_dataFrequency->setCurrentText ("15m");

    ui->dataFrequency->addItems (timeFre);
    ui->dataFrequency->setCurrentText ("day");

//    QStringList energyTimeFre;
//    energyTimeFre << "10m" << "15m" << "30m" << "60m" << "120m" << "day" << "week" << "month";
//    ui->energyDataFrequency_comboBox->addItems(energyTimeFre);
//    ui->energyDataFrequency_comboBox->setCurrentText ("day");
}

void Widget::initEnergyDataFreq() {
    m_energyDataFreqListWidget = new QListWidget(this);
    QStringList energyTimeFre;
    energyTimeFre << "10m" << "15m" << "30m" << "60m" << "120m" << "day" << "week" << "month";
    for (auto reqValue: energyTimeFre) {
        QListWidgetItem *pItem = new QListWidgetItem(m_energyDataFreqListWidget);
        pItem->setData(Qt::UserRole, reqValue);

        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(reqValue);
//        if(reqValue == "day" || reqValue == "120m" || reqValue == "60m") {
//            pCheckBox->setChecked(true);
//        }

        if(reqValue == "day") {
            pCheckBox->setChecked(true);
        }

        m_energyDataFreqListWidget->addItem(pItem);
        m_energyDataFreqListWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->energyDataFrequency_comboBox->setModel(m_energyDataFreqListWidget->model());
    ui->energyDataFrequency_comboBox->setView(m_energyDataFreqListWidget);
}

void Widget::initSelectIndexComboBox() {
    QStringList indexList;
    indexList << "000849" << "000908" << "000909" << "000910" << "000911" << "000912"
              << "000913" << "000915" << "000917" << "000951" << "000952" ;
    ui->targetIndex_QComboBox->addItems (indexList);
    ui->targetIndex_QComboBox->setCurrentText ("000913");
}

void Widget::initHedgedIndexComboBox() {
    QStringList indexList;
    indexList << "000300" << "000849" << "000908" << "000909" << "000910" << "000911"
              << "000912" << "000913" << "000915" << "000917" << "000951" << "000952";
    ui->hedgeIndex_comboBox->addItems (indexList);
    ui->hedgeIndex_comboBox->setCurrentText ("000300");
}

void Widget::initIndexAveNumb() {
    ui->indexAveNunb_spinBox->setMaximum(100000);
    ui->indexAveNunb_spinBox->setMinimum(-100000);
    ui->indexAveNunb_spinBox->setValue(32);
}

void Widget::initMacdTime () {
    ui->EMA1TimeSpinBox->setValue (12);
    ui->EMA2TimeSpinBox->setValue (26);
    ui->DIFFTimeSpinBox->setValue (9);
}

void Widget::setFutureContractCombox() {
    ui->futureList_ComboBox->clear();
    ui->futureList_ComboBox->addItems(m_futureList);
    ui->futureList_ComboBox->setCurrentIndex(0);
}

void Widget::setFutureContractList() {
    Database database_obj(this, "0", m_currDBHost);
    m_futureList = database_obj.getAllData(m_futureIndexCode, "Future_Info");
//    qDebug() << m_futureList;
}

void Widget::initDatasourceCombox() {
    ui->dataSource_ComboBox->addItems(m_dbhostList);
    ui->dataSource_ComboBox->setCurrentIndex(0);
}

void Widget::initSpreadSpinbox() {
    ui->maxSpread_SpinBox->setSingleStep(0.01);
    ui->minSpread_SpinBox->setSingleStep(0.01);

    ui->maxSpread_SpinBox->setRange(-100000, 100000);
    ui->minSpread_SpinBox->setRange(-100000, 100000);

    ui->maxSpread_SpinBox->setValue(100);
    ui->minSpread_SpinBox->setValue(-100);
}

void Widget::initEnergyIndexCodeComboBox() {
    QStringList indexList;
    indexList << "上证综指_000001" << "上证A指_000002" << "上证B指_000003" << "上证380_000009" << "上证180_000010"
              << "深证成指_399001" << "深证综指_399106" << "深证A指_399107" << "深证B指_399108"
              << "创业板50_399673" << "创业板指_399006" << "创业300_399102" << "创业板综_399102"
              << "中小板指_399005" << "中小300_399008"<< "中小板综_399101"
              << "上证50_000016" << "中证100_399903"  << "中证200_0904" << "中证500_000905" << "中证800_000906"
              << "沪深300_000300" << "300非银_000849"
              << "沪深300能源_000908" << "沪深300材料_000909" << "沪深300工业_000910" << "沪深300可选_000911"
              << "沪深300消费_000912" << "沪深300医药_000913" << "沪深300信息_000915" << "沪深300公用_000917"
              << "300银行_000951" << "300地产_000952";
    ui->energyIndexCode_comboBox->addItems (indexList);
    ui->energyIndexCode_comboBox->setCurrentText ("000300");
}

void Widget::initCSSParamComBox() {
    m_aveParamListWidget = new QListWidget(this);
    QList<int> aveNumbList;
//    aveNumbList <<12 << -25 << -55 << 107 << 143
//                <<232 << 319 << 452 << 619 << 729;

    aveNumbList << 13 << -25 << -55 << 108 << 144
                << 233 << 320 << 453 << 620 << 730;
    for (int aveValue: aveNumbList) {
        QListWidgetItem *pItem = new QListWidgetItem(m_aveParamListWidget);

        QSpinBox *pSpinBox = new QSpinBox(this);
        pSpinBox->setMinimum(-10000);
        pSpinBox->setMaximum(10000);
        pSpinBox->setValue(aveValue);

        m_aveParamListWidget->addItem(pItem);
        m_aveParamListWidget->setItemWidget(pItem, pSpinBox);
    }
    ui->energyAveParam_comboBox->setModel(m_aveParamListWidget->model());
    ui->energyAveParam_comboBox->setView(m_aveParamListWidget);
//    ui->energyAveParam_comboBox->setCurrentText("正值代表MA, 负值代表EMA");

    m_cssParamListWidget = new QListWidget(this);
    QMap<QString, int> aveValue;
    aveValue.insert("主指标平均数: ", 32);
    aveValue.insert("从指标平均数: ", int(32/2.6));
    aveValue.insert("势能指标平均数: ", 84);
    for (QMap<QString, int>::iterator it = aveValue.begin(); it != aveValue.end(); ++it) {
        QListWidgetItem *pItem = new QListWidgetItem(m_cssParamListWidget);
        pItem->setData(Qt::UserRole, it.key());

        QSpinBox *pSpinBox = new QSpinBox(this);
        pSpinBox->setMinimum(1);
        pSpinBox->setMaximum(10000);
        pSpinBox->setValue(it.value());
        pSpinBox->setPrefix(it.key());

        m_cssParamListWidget->addItem(pItem);
        m_cssParamListWidget->setItemWidget(pItem, pSpinBox);
    }

    ui->energyCssParam_comboBox->setModel(m_cssParamListWidget->model());
    ui->energyCssParam_comboBox->setView(m_cssParamListWidget);
}

void Widget::initProgramWorkInfoTableView () {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QString("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QString("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QString("备注")));
    ui->programInfo_tableView->setModel (initModel);
    ui->programInfo_tableView->setColumnWidth (0, 250);
    ui->programInfo_tableView->setColumnWidth (1, 600);
    ui->programInfo_tableView->setColumnWidth (2, 150);
    ui->programInfo_tableView->setShowGrid (false);
}

void Widget::setStrategyTable() {
    m_strategyFileInfoList = getExcelFileInfo(m_strategyFileDir);

    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
    } else {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_strategyFileInfoList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(m_strategyFileInfoList[i].fileName()));
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("策略组合")));
    ui->strategy_table->setModel (standardItemModel);
    ui->strategy_table->setColumnWidth (0, 300);
    ui->strategy_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->strategy_table->setShowGrid (false);
}

void Widget::setBuySalePortfolioTable() {
    m_buySalePortfolioFileInfoList = getExcelFileInfo(m_buySalePortfolioFileDir);
    m_portfolioAmount = getPortfolioAmountMap(m_buySalePortfolioFileInfoList);
//    printMap(m_portfolioAmount, "m_portfolioAmount: ");

    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
    } else {
        standardItemModel->clear();
    }

    int index = 0;
    for (QMap<QString, QString>::iterator it = m_portfolioAmount.begin();
         it != m_portfolioAmount.end(); ++it) {
        standardItemModel->setItem(index, 0, new QStandardItem(it.key()));
        standardItemModel->setItem(index++, 1, new QStandardItem(it.value()));
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("买卖组合")));
    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString("金额")));
    ui->buySalePortfolio_table->setModel (standardItemModel);
    ui->buySalePortfolio_table->setColumnWidth (0, 400);
    ui->buySalePortfolio_table->setColumnWidth (1, 100);
    ui->buySalePortfolio_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->buySalePortfolio_table->setShowGrid (false);
}

void Widget::initTableContextMenu() {
    ui->strategy_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->buySalePortfolio_table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->strategy_table,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_strategyTable_contextMenu(QPoint)));

    connect(ui->buySalePortfolio_table,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_portfolioTable_contextMenu(QPoint)));
}

void Widget::show_strategyTable_contextMenu(QPoint pos) {
    QMenu *menu = new QMenu(ui->strategy_table);
    QAction *refreshTable = new QAction(QString("刷新"),ui->strategy_table);
    QAction *deleteFile = new QAction(QString("删除"),ui->strategy_table);

    connect (refreshTable,SIGNAL(triggered()),this,SLOT(refresh_strategy_table()));
    connect (deleteFile, SIGNAL(triggered()),this,SLOT(delete_strategy_file()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    menu->move (cursor ().pos ());
    menu->show ();
    pos;
}

void Widget::show_portfolioTable_contextMenu(QPoint pos) {
    QMenu *menu = new QMenu(ui->buySalePortfolio_table);
    QAction *refreshTable = new QAction(QString("刷新"),ui->buySalePortfolio_table);
    QAction *deleteFile = new QAction(QString("删除"),ui->buySalePortfolio_table);

    connect (refreshTable,SIGNAL(triggered()),this,SLOT(refresh_portfolio_table()));
    connect (deleteFile, SIGNAL(triggered()),this,SLOT(delete_portfolio_file()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    menu->move (cursor ().pos ());
    menu->show ();
    pos;
}

void Widget::refresh_strategy_table() {
    setStrategyTable();
}

void Widget::refresh_portfolio_table() {
    setBuySalePortfolioTable();
}

void Widget::delete_strategy_file() {
    QMap<int, int> selectMap = getSelectRows(ui->strategy_table);
//    qDebug() << "selectMap: " << selectMap;
    QList<QString> selectData = getSelectTableData(ui->strategy_table, selectMap);
//    qDebug() << "selectData: " << selectData;

    for (QString stringData : selectData) {
        QString fileName = m_strategyFileDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_tableView, QString("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_tableView, QString("删除 %1 失败").arg(stringData));
            }
        }
    }
    setStrategyTable();
}

void Widget::delete_portfolio_file() {
    QMap<int, int> selectMap = getSelectRows(ui->buySalePortfolio_table);
//    qDebug() << "selectMap: " << selectMap;
    QList<QString> selectData = getSelectTableData(ui->buySalePortfolio_table, selectMap);
//    qDebug() << "selectData: " << selectData;

    for (QString stringData : selectData) {
        QString fileName = m_buySalePortfolioFileDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_tableView, QString("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_tableView, QString("删除 %1 失败").arg(stringData));
            }
        }
    }
    setBuySalePortfolioTable();
}

void Widget::on_chooseStartDate_editingFinished()
{
    QDate startDate = ui->chooseStartDate->date ();
    QString strDate = startDate.toString ("yyyyMMdd");
//    qDebug() << "strDate: " << strDate;
}

void Widget::on_strategy_table_clicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_strategyFileDir + fileName;

    m_currStrategyName = (fileName.split("."))[0];

    m_strategyMap = readExcelMapInt(cmpFileName);
    printMap(m_strategyMap, "m_strategyMap");

    if (m_strategyMap.find("Error") != m_strategyMap.end()) {
        updateProgramInfo (ui->programInfo_tableView, QString(QString("读取策略: %1, 出错")).arg(fileName));
    } else {
        updateProgramInfo (ui->programInfo_tableView, QString(QString("策略中股票数目为: %1"))
                                                      .arg(m_strategyMap.size ()));
    }
}

void Widget::on_buySalePortfolio_table_clicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_buySalePortfolioFileDir + fileName;

    m_currBuySalePortfolioName = (fileName.split("."))[0];
//    qDebug() << "m_currBuySalePortfolioName: " << m_currBuySalePortfolioName;

    updateProgramInfo (ui->programInfo_tableView, QString(QString("读取买卖策略: %1"))
                                                  .arg(m_currBuySalePortfolioName));

    m_buyStrategyMap = readExcelMapInt(cmpFileName, "buy");
    m_saleStrategyMap = readExcelMapInt(cmpFileName, "sale");
    if (m_buyStrategyMap.find("Error") != m_buyStrategyMap.end()) {
        updateProgramInfo(ui->programInfo_tableView, QString("%1 没有正确的买入组合信息").arg(fileName));
    }else if (m_saleStrategyMap.find("Error") != m_saleStrategyMap.end()) {
        updateProgramInfo(ui->programInfo_tableView, QString("%1 没有正确的卖出组合信息").arg(fileName));
    }

    updateProgramInfo (ui->programInfo_tableView, QString(QString("策略中股票数目为: %1"))
                                                  .arg(m_buyStrategyMap.size () + m_saleStrategyMap.size()));

}

void Widget::on_strategy_table_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_strategyFileDir + fileName;
    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
    } else {
        QMessageBox::critical(NULL, "Error", QString("当前文件已经被删除,请刷新当前表格"));
    }
}

void Widget::on_buySalePortfolio_table_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_buySalePortfolioFileDir + fileName;

    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
    } else {
        QMessageBox::critical(NULL, "Error", QString("当前文件已经被删除,请刷新当前表格"));
    }
}

void Widget::on_historyHedgeIndexChart_clicked()
{
    if (m_strategyMap.size () == 0) {
        QMessageBox::critical(NULL, "Error", QString("还未选择策略"));
        return;
    } else if (m_strategyMap.find("Error") != m_strategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString(QString("读取策略: %1, 出错")).arg(m_currStrategyName));
        m_strategyMap.clear();
    }else {
        m_isBuySalePortfolio = false;
        QString startDate = ui->chooseStartDate->date ().toString ("yyyyMMdd");
        QString endDate = ui->chooseEndDate->date ().toString ("yyyyMMdd");
        QString timeType = ui->dataFrequency->currentText();
        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QString dbhost = ui->dataSource_ComboBox->currentText();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;

        QString hedgeIndexCode = ui->hedgeTarget_comboBox->currentData ().toString ();
        int hedgeIndexCount = ui->hedgeCount_spinBox->value ();

        if (startDate.toInt () >= endDate.toInt ()) {
            QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
            return;
        } else {
            QWidget* chartView = new ChartForm(0, ui->programInfo_tableView,
                                              m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                              hedgeIndexCode, hedgeIndexCount, macdTime,
                                              m_strategyMap, m_currStrategyName,
                                              EmpytQStringIntMap(), EmpytQStringIntMap(),
                                              false, startDate, endDate, timeType);
            chartView->show ();
            connect(chartView, SIGNAL(windowClose_signal(int)),
                    this, SLOT(windowClose_slot(int)));
            m_chartViews.append (chartView);
            updateProgramInfo (ui->programInfo_tableView, QString(QString("策略名称: %1, 策略中股票数目为: %2"))
                                                          .arg(m_currStrategyName).arg(m_strategyMap.size ()));

            updateProgramInfo(ui->programInfo_tableView, QString("当前数据源为: %1").arg(dbhost));

            updateProgramInfo (ui->programInfo_tableView, QString(QString("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6"))
                                                          .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

            updateProgramInfo (ui->programInfo_tableView, QString(QString("对冲目标: %1, 对冲笔数: %2"))
                                                          .arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));

            updateProgramInfo (ui->programInfo_tableView, QString("开始读取数据"));
        }
    }
}

void Widget::on_realTimeHedgeIndexChart_clicked()
{
    if (m_strategyMap.size () == 0) {
        QMessageBox::critical(NULL, "Error", QString("还未选择策略"));
        return;
    } else if (m_strategyMap.find("Error") != m_strategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString(QString("读取策略: %1, 出错")).arg(m_currStrategyName));
        m_strategyMap.clear();
    } else {
        m_isBuySalePortfolio = false;
        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QString dbhost = ui->dataSource_ComboBox->currentText();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;

        QString hedgeIndexCode = ui->hedgeTarget_comboBox->currentData ().toString ();
        int hedgeIndexCount = ui->hedgeCount_spinBox->value ();

        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView,
                                          m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                          hedgeIndexCode, hedgeIndexCount, macdTime,
                                          m_strategyMap, m_currStrategyName);
        chartView->show ();
        connect(chartView, SIGNAL(windowClose_signal(int)),
                this, SLOT(windowClose_slot(int)));
        m_chartViews.append (chartView);

        updateProgramInfo (ui->programInfo_tableView, QString(QString("策略名称: %1, 策略中股票数目为: %2"))
                                                      .arg(m_currStrategyName).arg(m_strategyMap.size ()));

        updateProgramInfo(ui->programInfo_tableView, QString("当前数据源为: %1").arg(dbhost));

        updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3")
                                                      .arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

        updateProgramInfo (ui->programInfo_tableView, QString(QString("对冲目标: %1, 对冲笔数: %2"))
                                                      .arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));

        updateProgramInfo (ui->programInfo_tableView, QString("开始读取数据"));
    }
}

void Widget::on_historyBuySaleChart_clicked()
{
    if(m_buyStrategyMap.size() == 0 && m_saleStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString("还未选择买卖组合策略"));
    } else if (m_buyStrategyMap.find("Error") != m_buyStrategyMap.end() ||
               m_saleStrategyMap.find("Error")!= m_saleStrategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString("%1 中没有正确的买卖组合信息")
                                             .arg(m_currBuySalePortfolioName));
    }else {
        m_isBuySalePortfolio = true;
        QString startDate = ui->chooseStartDate->date ().toString ("yyyyMMdd");
        QString endDate = ui->chooseEndDate->date ().toString ("yyyyMMdd");
        QString timeType = ui->dataFrequency->currentText();

        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QString dbhost = ui->dataSource_ComboBox->currentText();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;
//        qDebug() << "m_currBuySalePortfolioName: " << m_currBuySalePortfolioName;

        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView,
                                           m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                           m_currBuySalePortfolioName, -1, macdTime,
                                           EmpytQStringIntMap(), m_currBuySalePortfolioName,
                                           m_buyStrategyMap, m_saleStrategyMap,
                                           false, startDate, endDate, timeType);
        chartView->show();
        connect(chartView, SIGNAL(windowClose_signal(int)),
                this, SLOT(windowClose_slot(int)));
        m_chartViews.append(chartView);

        updateProgramInfo (ui->programInfo_tableView, QString(QString("策略名称: %1"))
                                                      .arg(m_currBuySalePortfolioName));

        updateProgramInfo(ui->programInfo_tableView, QString("当前数据源为: %1").arg(dbhost));

        updateProgramInfo (ui->programInfo_tableView, QString(QString("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6"))
                                                      .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

        updateProgramInfo (ui->programInfo_tableView, QString("开始读取数据"));
    }

}

void Widget::on_realTimeBuySaleChart_clicked()
{
    if(m_buyStrategyMap.size() == 0 && m_saleStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString("还未选择买卖组合策略"));
    } else if (m_buyStrategyMap.find("Error") != m_buyStrategyMap.end() ||
               m_saleStrategyMap.find("Error")!= m_saleStrategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString("%1 中没有正确的买卖组合信息")
                                             .arg(m_currBuySalePortfolioName));
    } else {
        m_isBuySalePortfolio = true;
        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QString dbhost = ui->dataSource_ComboBox->currentText();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;
//        qDebug() << "m_currBuySalePortfolioName: " << m_currBuySalePortfolioName;

        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView,
                                           m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                           m_currBuySalePortfolioName, -1, macdTime,
                                           EmpytQStringIntMap(), m_currBuySalePortfolioName,
                                           m_buyStrategyMap, m_saleStrategyMap, true);
        chartView->show();

        connect(chartView, SIGNAL(windowClose_signal(int)),
                this, SLOT(windowClose_slot(int)));
        m_chartViews.append(chartView);

        updateProgramInfo (ui->programInfo_tableView, QString(QString("策略名称: %1."))
                                                      .arg(m_currBuySalePortfolioName));

        updateProgramInfo(ui->programInfo_tableView, QString("当前数据源为: %1").arg(dbhost));

        updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3")
                                                      .arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

        updateProgramInfo (ui->programInfo_tableView, QString("开始读取数据"));
    }
}

void Widget::on_showFutureSpread_Button_clicked()
{
    int maxSpreadValue = ui->maxSpread_SpinBox->value();
    int minSpreadValue = ui->minSpread_SpinBox->value();
    QString curFuture = ui->futureList_ComboBox->currentText();
    QString dbhost = ui->dataSource_ComboBox->currentText();
    QWidget* futureChartView = new FutureChart(m_chartViews.count(), dbhost, ui->programInfo_tableView,
                                                curFuture, maxSpreadValue, minSpreadValue);
    connect(futureChartView, SIGNAL(windowClose_signal(int)),
            this, SLOT(windowClose_slot(int)));

    futureChartView->show();
    m_chartViews.append(futureChartView);

    updateProgramInfo (ui->programInfo_tableView, QString("期货: %1, 最大基差: %2, 最小基差: %3")
                                                  .arg(curFuture).arg(maxSpreadValue).arg(minSpreadValue));

    updateProgramInfo(ui->programInfo_tableView, QString("当前数据源为: %1").arg(dbhost));
}

void Widget::on_Annoucnement_Button_clicked()
{
    QString startDate = ui->AnnouncementStartDate->date ().toString ("yyyyMMdd");
    QString endDate = ui->AnnouncementEndDate->date ().toString ("yyyyMMdd");
    QString dbhost = ui->dataSource_ComboBox->currentText();
    m_announcementView = new AnnouncementForm(QString("%1").arg(m_chartViews.count()), dbhost,
                                              startDate, endDate);
    m_announcementView->show();
    m_chartViews.append(m_announcementView);
}

void Widget::on_chooseStrategyDir_Button_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(NULL, "caption", ".");
    if (dirName != "") {
        m_strategyFileDir = dirName + "/";
        if (writeFileInfo(m_nativeFileName, "A1", m_strategyFileDir) == 1) {
            setStrategyTable();
            updateProgramInfo(ui->programInfo_tableView, QString(QString("设置新的策略文件夹为: %1"))
                                                         .arg(m_strategyFileDir));
        } else {
            QMessageBox::critical(NULL, "Error", QString("设置新的策略文件夹出错"));
        }
    }
}

void Widget::on_choosePortfolioDir_Button_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(NULL, "caption", ".");
    if (dirName != "") {
        m_buySalePortfolioFileDir = dirName + "/";
        if (writeFileInfo(m_nativeFileName, "A2", m_buySalePortfolioFileDir) == 1) {
            setBuySalePortfolioTable();
            updateProgramInfo(ui->programInfo_tableView, QString(QString("设置新的买卖组合文件夹为: %1"))
                                                         .arg(m_buySalePortfolioFileDir));
        } else {
            QMessageBox::critical(NULL, "Error", QString("设置新的买卖文件夹出错"));
        }
    }
}

void Widget::windowClose_slot(int windowID) {
    if (windowID >= 0 && windowID < m_chartViews.size()) {
        if (m_chartViews[windowID] != NULL) {
            qDebug() << "windowID: " << windowID;
            delete m_chartViews[windowID];
            m_chartViews[windowID] = NULL;
        }
    }
}

void Widget::closeEvent(QCloseEvent *event) {
    event;
    for(int i = 0; i < m_chartViews.size(); ++i) {
        if (NULL != m_chartViews[i]) {
            m_chartViews[i] -> close();
        }
    }
}

void Widget::on_dataSource_ComboBox_currentIndexChanged(const QString &arg1)
{
    m_currDBHost = arg1;
    setFutureContractList();
    setFutureContractCombox();
}

void Widget::on_showIndexHedgedPic_pushButton_clicked()
{
    QString startDate = ui->chooseIndexStartDate->date().toString("yyyyMMdd");
    QString endDate = ui->chooseIndexEndDate->date().toString("yyyyMMdd");
    QString dbhost = ui->dataSource_ComboBox->currentText();
    QString timeType = ui->index_dataFrequency->currentText();
    QString databaseName = QString("MarketData_%1").arg(timeType);
    QString selectIndex = ui->targetIndex_QComboBox->currentText();
    QString hedgedIndex = ui->hedgeIndex_comboBox->currentText();
    int aveNumb = ui->indexAveNunb_spinBox->value();

    if (startDate.toInt () > endDate.toInt ()) {
        QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
        return;
    }

    if (selectIndex == hedgedIndex) {
        QMessageBox::critical(NULL, "Error", QString("目标指数与对冲指数相同都为: %1").arg(selectIndex));
        return;
    }
    QWidget* chartView = new IndexChart(m_chartViews.count(), dbhost, databaseName,
                                selectIndex, hedgedIndex, startDate, endDate,
                                aveNumb);
    m_chartViews.append (chartView);
    chartView->setWindowTitle("指数对冲图");
    chartView->show();

    connect(chartView, SIGNAL(windowClose_signal(int)),
            this, SLOT(windowClose_slot(int)));

    QString info = QString("开始显示指数对冲图: \n目标指数为: %1, 对冲指数为: %2\n起始时间: %3, 终止时间: %4\n数据频率: %5, 平均值: %6")
                           .arg(selectIndex).arg(hedgedIndex)
                           .arg(startDate).arg(endDate)
                           .arg(timeType).arg(aveNumb);
    updateProgramInfo (ui->programInfo_tableView, info);
}

QList<int> Widget::getCSSParams() {
    QList<int> result;
    for (int i = 0; i < m_cssParamListWidget->count(); ++i) {
        QListWidgetItem *pItem = m_cssParamListWidget->item(i);
        QSpinBox *pSpinBox = (QSpinBox*)m_cssParamListWidget->itemWidget(pItem);
        result.append(pSpinBox->value());
    }
    return result;
}

void Widget::getAveParams(QList<int>& aveNumbList, QList<bool>& isEMAList) {
    for (int i = 0; i < m_aveParamListWidget->count(); ++i) {
        QListWidgetItem *pItem = m_aveParamListWidget->item(i);
        QSpinBox *pSpinBox = (QSpinBox*)m_aveParamListWidget->itemWidget(pItem);
        int currValue = pSpinBox->value();
        if (currValue > 0) {
            aveNumbList.append(currValue);
            isEMAList.append(false);
        } else if(currValue < 0) {
            aveNumbList.append(0-currValue);
            isEMAList.append(true);
        } else {

        }
    }
}

QStringList Widget::getEnergyDataFreq() {
    QStringList result;
    for (int i = 0; i < m_energyDataFreqListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_energyDataFreqListWidget->item(i);
        QWidget *pWidget = m_energyDataFreqListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            result.append(strText);
        }
    }
    return result;
}

bool Widget::checkCodeInDatabase(QString codeName, QString dbhost, QStringList timeTypeList) {
    bool result = true;
    for (auto timeType : timeTypeList) {
        QString databaseName = QString("MarketData_%1").arg(timeType);
        if (!isCodeInDatabase(codeName, databaseName, dbhost)) {
            QMessageBox::critical(NULL, "Error", QString("选择的代码: %1, 不在数据库中").arg(codeName));
            result = false;
        }
    }
    return result;
}

/**/
void Widget::on_showAVEEnergy_pushButton_clicked()
{
    QString startDate = ui->energyStart_dateEdit->date().toString("yyyyMMdd");
    QString endDate = ui->energyEnd_dateEdit->date().toString("yyyyMMdd");
    QString dbhost = ui->dataSource_ComboBox->currentText();
    QString indexCodeName = getCompleteIndexCode(ui->energyIndexCode_comboBox->currentText());
    indexCodeName = getCompleteIndexCode(indexCodeName.split("_")[1]);
    QString codeName = indexCodeName;
    QString stockCodeName = ui->energyStockCode_lineEdit->text();
    if (stockCodeName != "") {
        stockCodeName = getCompleteSecode(stockCodeName);
        codeName = stockCodeName;
    }
    QList<int> cssAveNumbList = getCSSParams();
    QList<int> aveNumbList;
    QList<bool> isEMAList;
    getAveParams(aveNumbList, isEMAList);

    int mainAveNumb = cssAveNumbList[0];
    int subAveNumb = cssAveNumbList[1];
    int energyAveNumb = cssAveNumbList[2];
    double css12Rate = 0.014;
    double mainCssRate1 = 1.0/3.0;
    double mainCssRate2 = 2.0/3.0;
    double energyCssRate1 = 11.0/13.0;
    double energyCssRate2 = 2.0/13.0;
    double maxCSS = 300;
    double minCSS = -300;

    QString timeType;
    QStringList timeTypeList = getEnergyDataFreq();
    qDebug() << timeTypeList;
    if (timeTypeList.size() == 1) {
        timeType = timeTypeList[0];
    } else {
        timeType = timeTypeList.join(", ");
    }

    if (timeTypeList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString("还未选择时间频率"));
        return;
    }

    if (startDate.toInt () > endDate.toInt ()) {
        QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
        return;
    }

//    if (!checkCodeInDatabase(codeName, dbhost, timeTypeList)) {
//        return;
//    }

    QStringList strAveNumbList;
    for (int i = 0; i < aveNumbList.size(); ++i) {
        if (isEMAList[i] == false) {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]));
        } else {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]*-1));
        }
    }
    QString info = QString("开始显示指标图: \n代码为: %1, \n起始时间: %2, 终止时间: %3\n")
                   .arg(codeName).arg(startDate).arg(endDate)
                 + QString("数据频率: %1 \n主指标平均值: %2 \n从指标平均值: %3 \n势能指标平均值: %4\n")
                   .arg(timeType).arg(mainAveNumb).arg(subAveNumb).arg(energyAveNumb)
                 + QString("均线参数: %1").arg(strAveNumbList.join(","));

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes|QMessageBox::No)) {
        updateProgramInfo (ui->programInfo_tableView, info);
        QWidget* chartView;
        if (timeTypeList.size() == 1) {
            chartView = new CSSChartFormOne(m_chartViews.size(), dbhost, timeType,
                                            startDate, endDate, codeName,
                                            aveNumbList, isEMAList,
                                            mainAveNumb, subAveNumb, energyAveNumb,
                                            css12Rate, mainCssRate1, mainCssRate2,
                                            energyCssRate1, energyCssRate2, maxCSS, minCSS);

        } else {
            chartView = new CSSChartForm(m_chartViews.size(), dbhost, timeTypeList,
                                        startDate, endDate, codeName,
                                        aveNumbList, isEMAList,
                                        mainAveNumb, subAveNumb, energyAveNumb,
                                        css12Rate, mainCssRate1, mainCssRate2,
                                        energyCssRate1, energyCssRate2, maxCSS, minCSS);
        }

        chartView->setWindowTitle(QString("%1,[%2, %3], %4 指标图")
                                 .arg(codeName).arg(startDate).arg(endDate).arg(timeType));

        connect(chartView, SIGNAL(windowClose_signal(int)),
                this, SLOT(windowClose_slot(int)));

        m_chartViews.append (chartView);

        chartView->show();

//        int maxColNumb = 3;
//        for (int i = 0; i < timeTypeList.size();) {
//            int endPos = min(timeTypeList.size(), i+maxColNumb);
//            QStringList currTimeTypeList = getSubList(timeTypeList, i, endPos);
//            QWidget* chartView = new CSSChartForm(m_chartViews.size(), dbhost, currTimeTypeList,
//                                        startDate, endDate, codeName,
//                                        aveNumbList, isEMAList,
//                                        mainAveNumb, subAveNumb, energyAveNumb,
//                                        css12Rate, mainCssRate1, mainCssRate2,
//                                        energyCssRate1, energyCssRate2, maxCSS, minCSS);

//            chartView->setWindowTitle(QString("%1,[%2, %3], %4 指标图")
//                                     .arg(codeName).arg(startDate).arg(endDate).arg(currTimeTypeList.join(", ")));

//            connect(chartView, SIGNAL(windowClose_signal(int)),
//                    this, SLOT(windowClose_slot(int)));

//            m_chartViews.append (chartView);

//            chartView->show();

//            i = endPos;
//        }

    }
}


/*
 * 临时多窗口方案
void Widget::on_showAVEEnergy_pushButton_clicked()
{
    QString startDate = ui->energyStart_dateEdit->date().toString("yyyyMMdd");
    QString endDate = ui->energyEnd_dateEdit->date().toString("yyyyMMdd");
    QString dbhost = ui->dataSource_ComboBox->currentText();
    QString indexCodeName = getCompleteIndexCode(ui->energyIndexCode_comboBox->currentText());
    QString codeName = indexCodeName;
    QString stockCodeName = ui->energyStockCode_lineEdit->text();
    if (stockCodeName != "") {
        stockCodeName = getCompleteSecode(stockCodeName);
        codeName = stockCodeName;
    }
    QList<int> cssAveNumbList = getCSSParams();
    QList<int> aveNumbList;
    QList<bool> isEMAList;
    getAveParams(aveNumbList, isEMAList);

    int mainAveNumb = cssAveNumbList[0];
    int subAveNumb = cssAveNumbList[1];
    int energyAveNumb = cssAveNumbList[2];
    double css12Rate = 0.014;
    double mainCssRate1 = 1.0/3.0;
    double mainCssRate2 = 2.0/3.0;
    double energyCssRate1 = 11.0/13.0;
    double energyCssRate2 = 2.0/13.0;
    double maxCSS = 300;
    double minCSS = -300;

    QString timeType;
    QStringList timeTypeList = getEnergyDataFreq();
    qDebug() << timeTypeList;
    if (timeTypeList.size() == 1) {
        timeType = timeTypeList[0];
    } else {
        timeType = timeTypeList.join(", ");
    }

    if (timeTypeList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString("还未选择时间频率"));
        return;
    }

    if (startDate.toInt () > endDate.toInt ()) {
        QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
        return;
    }

    if (!checkCodeInDatabase(codeName, dbhost, timeTypeList)) {
        return;
    }

    QStringList strAveNumbList;
    for (int i = 0; i < aveNumbList.size(); ++i) {
        if (isEMAList[i] == false) {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]));
        } else {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]*-1));
        }
    }
    QString info = QString("开始显示指标图: \n代码为: %1, \n起始时间: %2, 终止时间: %3\n")
                   .arg(codeName).arg(startDate).arg(endDate)
                 + QString("数据频率: %1 \n主指标平均值: %2 \n从指标平均值: %3 \n势能指标平均值: %4\n")
                   .arg(timeType).arg(mainAveNumb).arg(subAveNumb).arg(energyAveNumb)
                 + QString("均线参数: %1").arg(strAveNumbList.join(","));

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes|QMessageBox::No)) {
        updateProgramInfo (ui->programInfo_tableView, info);

        for (auto timeType:timeTypeList) {
            QWidget* chartView = new CSSChartFormOne(m_chartViews.size(), dbhost, timeType,
                                            startDate, endDate, codeName,
                                            aveNumbList, isEMAList,
                                            mainAveNumb, subAveNumb, energyAveNumb,
                                            css12Rate, mainCssRate1, mainCssRate2,
                                            energyCssRate1, energyCssRate2, maxCSS, minCSS);

            chartView->setWindowTitle(QString("%1, %2, [%3, %4] 指标图")
                                     .arg(timeType).arg(codeName).arg(startDate).arg(endDate));

            connect(chartView, SIGNAL(windowClose_signal(int)),
                    this, SLOT(windowClose_slot(int)));

            m_chartViews.append (chartView);

            chartView->show();
        }

    }

}
*/


