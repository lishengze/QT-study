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

#include "excel_func.h"
#include "widget_func.h"
#include "io_func.h"
#include "toolfunc.h"
#include "widget.h"
#include "ui_widget.h"
#include "QChartView"

#include "chartform.h"
#include "announcementform.h"
#include "generateportfolioform.h"
#include "xlsxdocument.h"
#include "futurechart.h"
#include "database.h"

QT_CHARTS_USE_NAMESPACE

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    initCommonData();
    initWidegt();
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
        qDebug() << QString("%1 does not exit!").arg(m_nativeFileName);
        QXlsx::Document xlsx;
        m_strategyFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/strategy/");
        m_buySalePortfolioFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/买入卖出组合/");
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
            m_strategyFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/strategy/");
            m_buySalePortfolioFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/买入卖出组合/");
            xlsx.write("A1", m_strategyFileDir);
            xlsx.write("A2",m_buySalePortfolioFileDir);
            xlsx.save();
        } else {
            qDebug() << QString("%1 exits!").arg(m_nativeFileName);
            m_strategyFileDir = xlsx.cellAt(1, 1)->value().toString();
            m_buySalePortfolioFileDir = xlsx.cellAt(2, 1)->value().toString();
            qDebug() << "m_strategyFileDir: " << m_strategyFileDir;
            qDebug() << "m_buySalePortfolioFileDir: " << m_buySalePortfolioFileDir;
        }
    }
}

void Widget::initWidegt() {
    initCalendar();
    initHedgeComboBox();
    initDataFrequency();
    initMacdTime();
    setFutureContractCombox();
    initDatasourceCombox();
    initSpreadSpinbox();

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
}

void Widget::initHedgeComboBox() {
    ui->hedgeTarget_comboBox->setCurrentText (QString::fromLocal8Bit("沪深300"));
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("沪深300"),  "000300");
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("上证50"),   "000016");
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("中证100"),  "399903");
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("中证200"),  "000904");
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("中证500"),  "000905");
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("中证800"),  "000906");
    ui->hedgeTarget_comboBox->addItem (QString::fromLocal8Bit("中证1000"), "000852");
    ui->hedgeCount_spinBox->setValue (2);
}

void Widget::initMacdTime () {
    ui->EMA1TimeSpinBox->setValue (12);
    ui->EMA2TimeSpinBox->setValue (26);
    ui->DIFFTimeSpinBox->setValue (9);
}

void Widget::initDataFrequency () {
    QStringList timeFre;
    timeFre << "1m" << "5m" << "10m" << "15m" << "30m" << "60m" << "120m" << "day" << "week" << "month";
    ui->dataFrequency->addItems (timeFre);
    ui->dataFrequency->setCurrentText ("day");
}

void Widget::setFutureContractCombox() {
    ui->futureList_ComboBox->clear();
    ui->futureList_ComboBox->addItems(m_futureList);
    ui->futureList_ComboBox->setCurrentIndex(0);
}

void Widget::setFutureContractList() {
    Database database_obj(this, "0", m_currDBHost);
    m_futureList = database_obj.getAllData(m_futureIndexCode, "Future_Info");
    qDebug() << m_futureList;
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
}

void Widget::initProgramWorkInfoTableView () {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QString::fromLocal8Bit("备注")));
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

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("策略组合")));
    ui->strategy_table->setModel (standardItemModel);
    ui->strategy_table->setColumnWidth (0, 300);
    ui->strategy_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->strategy_table->setShowGrid (false);
}

void Widget::setBuySalePortfolioTable() {
    m_buySalePortfolioFileInfoList = getExcelFileInfo(m_buySalePortfolioFileDir);
    m_portfolioAmount = getPortfolioAmountMap(m_buySalePortfolioFileInfoList);
    printMap(m_portfolioAmount, "m_portfolioAmount: ");

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

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("买卖组合")));
    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("金额")));
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
    QAction *refreshTable = new QAction(QString::fromLocal8Bit("刷新"),ui->strategy_table);
    QAction *deleteFile = new QAction(QString::fromLocal8Bit("删除"),ui->strategy_table);

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
    QAction *refreshTable = new QAction(QString::fromLocal8Bit("刷新"),ui->buySalePortfolio_table);
    QAction *deleteFile = new QAction(QString::fromLocal8Bit("删除"),ui->buySalePortfolio_table);

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
                updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
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
                updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
            }
        }
    }
    setBuySalePortfolioTable();
}

void Widget::on_chooseStartDate_editingFinished()
{
    QDate startDate = ui->chooseStartDate->date ();
    QString strDate = startDate.toString ("yyyyMMdd");
    qDebug() << "strDate: " << strDate;
}

void Widget::on_strategy_table_clicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_strategyFileDir + fileName;
//    qDebug() << "cmpFileName: " << cmpFileName;

    m_currStrategyName = (fileName.split("."))[0];
//    qDebug() << "m_currStrategyName: " << m_currStrategyName;

    m_strategyMap = readExcelMapInt(cmpFileName);
//    qDebug() << "m_strategyMap: " << m_strategyMap;

    if (m_strategyMap.find("Error") != m_strategyMap.end()) {
        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("读取策略: %1, 出错")).arg(fileName));
    } else {
        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("策略中股票数目为: %1"))
                                                      .arg(m_strategyMap.size ()));
    }
}

void Widget::on_buySalePortfolio_table_clicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_buySalePortfolioFileDir + fileName;

    m_currBuySalePortfolioName = (fileName.split("."))[0];
    qDebug() << "m_currBuySalePortfolioName: " << m_currBuySalePortfolioName;

    updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("读取买卖策略: %1"))
                                                  .arg(m_currBuySalePortfolioName));

    m_buyStrategyMap = readExcelMapInt(cmpFileName, "buy");
    m_saleStrategyMap = readExcelMapInt(cmpFileName, "sale");
    printMap(m_buyStrategyMap, "m_buyStrategyMap: ");
    printMap(m_saleStrategyMap, "m_saleStrategyMap: ");

    if (m_buyStrategyMap.find("Error") != m_buyStrategyMap.end()) {
        updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("%1 没有正确的买入组合信息").arg(fileName));
    }else if (m_saleStrategyMap.find("Error") != m_saleStrategyMap.end()) {
        updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("%1 没有正确的卖出组合信息").arg(fileName));
    }

    updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("策略中股票数目为: %1"))
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
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件已经被删除,请刷新当前表格"));
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
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件已经被删除,请刷新当前表格"));
    }
}

void Widget::on_historyHedgeIndexChart_clicked()
{
    if (m_strategyMap.size () == 0) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("还未选择策略"));
        return;
    } else if (m_strategyMap.find("Error") != m_strategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString(QString::fromLocal8Bit("读取策略: %1, 出错")).arg(m_currStrategyName));
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
            QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("起始时间晚于截止时间"));
            return;
        } else {
            QWidget* charView = new ChartForm(0, ui->programInfo_tableView,
                                              m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                              hedgeIndexCode, hedgeIndexCount, macdTime,
                                              m_strategyMap, m_currStrategyName,
                                              EmpytQStringIntMap(), EmpytQStringIntMap(),
                                              false, startDate, endDate, timeType);
            charView->show ();
            m_chartViews.append (charView);
            updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("策略名称: %1, 策略中股票数目为: %2"))
                                                          .arg(m_currStrategyName).arg(m_strategyMap.size ()));

            updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("当前数据源为: %1").arg(dbhost));

            updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6"))
                                                          .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

            updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("对冲目标: %1, 对冲笔数: %2"))
                                                          .arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));

            updateProgramInfo (ui->programInfo_tableView, QString::fromLocal8Bit("开始读取数据"));
        }
    }
}

void Widget::on_realTimeHedgeIndexChart_clicked()
{
    if (m_strategyMap.size () == 0) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("还未选择策略"));
        return;
    } else if (m_strategyMap.find("Error") != m_strategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString(QString::fromLocal8Bit("读取策略: %1, 出错")).arg(m_currStrategyName));
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

        QWidget* charView = new ChartForm(0, ui->programInfo_tableView,
                                          m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                          hedgeIndexCode, hedgeIndexCount, macdTime,
                                          m_strategyMap, m_currStrategyName);
        charView->show ();
        m_chartViews.append (charView);

        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("策略名称: %1, 策略中股票数目为: %2"))
                                                      .arg(m_currStrategyName).arg(m_strategyMap.size ()));

        updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("当前数据源为: %1").arg(dbhost));

        updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3")
                                                      .arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("对冲目标: %1, 对冲笔数: %2"))
                                                      .arg(ui->hedgeTarget_comboBox->currentText ()).arg(hedgeIndexCount));

        updateProgramInfo (ui->programInfo_tableView, QString::fromLocal8Bit("开始读取数据"));
    }
}

void Widget::on_historyBuySaleChart_clicked()
{
    if(m_buyStrategyMap.size() == 0 && m_saleStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("还未选择买卖组合策略"));
    } else if (m_buyStrategyMap.find("Error") != m_buyStrategyMap.end() ||
               m_saleStrategyMap.find("Error")!= m_saleStrategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("%1 中没有正确的买卖组合信息")
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
        qDebug() << "m_currBuySalePortfolioName: " << m_currBuySalePortfolioName;

        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView,
                                           m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                           "", -1, macdTime,
                                           EmpytQStringIntMap(), m_currBuySalePortfolioName,
                                           m_buyStrategyMap, m_saleStrategyMap,
                                           false, startDate, endDate, timeType);
        chartView->show();
        m_chartViews.append(chartView);

        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("策略名称: %1"))
                                                      .arg(m_currBuySalePortfolioName));

        updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("当前数据源为: %1").arg(dbhost));

        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("起始时间: %1, 终止时间: %2, 时间频率: %3, T1: %4, T2: %5, T3: %6"))
                                                      .arg(startDate).arg(endDate).arg(timeType).arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

        updateProgramInfo (ui->programInfo_tableView, QString::fromLocal8Bit("开始读取数据"));
    }

}

void Widget::on_realTimeBuySaleChart_clicked()
{
    if(m_buyStrategyMap.size() == 0 && m_saleStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("还未选择买卖组合策略"));
    } else if (m_buyStrategyMap.find("Error") != m_buyStrategyMap.end() ||
               m_saleStrategyMap.find("Error")!= m_saleStrategyMap.end()) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("%1 中没有正确的买卖组合信息")
                                             .arg(m_currBuySalePortfolioName));
    } else {
        m_isBuySalePortfolio = true;
        int EVA1Time = ui->EMA1TimeSpinBox->value ();
        int EVA2Time = ui->EMA2TimeSpinBox->value ();
        int DIFFTime = ui->DIFFTimeSpinBox->value ();
        QString dbhost = ui->dataSource_ComboBox->currentText();
        QList<int> macdTime;
        macdTime << EVA1Time << EVA2Time << DIFFTime;
        qDebug() << "m_currBuySalePortfolioName: " << m_currBuySalePortfolioName;

        QWidget* chartView = new ChartForm(0, ui->programInfo_tableView,
                                           m_chartViews.count(), dbhost, m_isBuySalePortfolio,
                                           "", -1, macdTime,
                                           EmpytQStringIntMap(), m_currBuySalePortfolioName,
                                           m_buyStrategyMap, m_saleStrategyMap, true);
        chartView->show();
        m_chartViews.append(chartView);

        updateProgramInfo (ui->programInfo_tableView, QString(QString::fromLocal8Bit("策略名称: %1."))
                                                      .arg(m_currBuySalePortfolioName));

        updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("当前数据源为: %1").arg(dbhost));

        updateProgramInfo (ui->programInfo_tableView, QString("T1: %1, T2: %2, T3: %3")
                                                      .arg(EVA1Time).arg(EVA2Time).arg(DIFFTime));

        updateProgramInfo (ui->programInfo_tableView, QString::fromLocal8Bit("开始读取数据"));
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

    futureChartView->show();
    m_chartViews.append(futureChartView);

    updateProgramInfo (ui->programInfo_tableView, QString::fromLocal8Bit("期货: %1, 最大基差: %2, 最小基差: %3")
                                                  .arg(curFuture).arg(maxSpreadValue).arg(minSpreadValue));

    updateProgramInfo(ui->programInfo_tableView, QString::fromLocal8Bit("当前数据源为: %1").arg(dbhost));
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
            updateProgramInfo(ui->programInfo_tableView, QString(QString::fromLocal8Bit("设置新的策略文件夹为: %1"))
                                                         .arg(m_strategyFileDir));
        } else {
            QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("设置新的策略文件夹出错"));
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
            updateProgramInfo(ui->programInfo_tableView, QString(QString::fromLocal8Bit("设置新的买卖组合文件夹为: %1"))
                                                         .arg(m_buySalePortfolioFileDir));
        } else {
            QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("设置新的买卖文件夹出错"));
        }
    }
}

void Widget::receiveChartCLoseSignal(int chartViewID) {
    qDebug() << "Widget::receiveChartCLoseSignal chartViewID: " << chartViewID;
}

void Widget::closeEvent(QCloseEvent *event) {
    event;
    for(int i = 0; i < m_chartViews.size(); ++i) {
        if (NULL != m_chartViews[i]) {
            m_chartViews[i] -> close();
        }
    }
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

void Widget::on_dataSource_ComboBox_currentIndexChanged(const QString &arg1)
{
    qDebug() << "arg1: " << arg1;
    m_currDBHost = arg1;
    setFutureContractList();
    setFutureContractCombox();
}
