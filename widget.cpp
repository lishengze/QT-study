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
#include "datastruct.h"

#include "time_func.h"
#include "secode_func.h"
#include "excel_func.h"
#include "widget_func.h"
#include "io_func.h"
#include "toolfunc.h"
#include "process_data_func.h"
#include "widget.h"
#include "ui_widget.h"
#include "QChartView"

#include "chartform.h"
#include "announcementform.h"
#include "generateportfolioform.h"
#include "xlsxdocument.h"
#include "futurechart.h"
#include "database.h"
#include "csschartform.h"
#include "time_func.h"

QT_CHARTS_USE_NAMESPACE
#pragma execution_character_set("utf-8")

Widget::Widget(QWidget *parent) : QWidget(parent),
                                  ui(new Ui::Widget)
{
    ui->setupUi(this);
    initCommonData();
    initWidegt();

    if (!m_isDev)
    {
        setDBTableNameMap();
    }
}

Widget::~Widget()
{
    delete ui;
    for (int i = 0; i < m_chartViews.size(); ++i)
    {
        if (NULL != m_chartViews[i])
        {
            delete m_chartViews[i];
            m_chartViews[i] = NULL;
        }
    }

    if (NULL != m_workProgressDialog)
    {
        delete m_workProgressDialog;
        m_workProgressDialog = NULL;
    }

    m_histDataThread.quit();
    m_histDataThread.wait();
}

void Widget::initCommonData()
{
    m_workProgressDialog = NULL;
    m_announcementView   = NULL;
    m_futureIndexCode    = "000300";
    m_isDev              = true;
    m_hedgedType         = 1;
    m_hedgedString       = "点差对冲";

    m_dbhostList << "192.168.211.162"
                 << "192.168.211.165"
                 << "127.0.0.1";
    m_currDBHost         = m_dbhostList[0];
    initFileDir();
    registSignalParamsType();
}

void Widget::registSignalParamsType()
{
    qRegisterMetaType<QMap<QString, QList<QStringList>>>("QMap<QString, QList<QStringList>>");
    qRegisterMetaType<QList<QList<double>>>("QList<QList<double>>");
    qRegisterMetaType<QMap<QString, QStringList>>("QMap<QString,QStringList>");
    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<QMap<QString, double>>("QMap<QString, double>");
    qRegisterMetaType<QVector<int>>("QVector<int>");    
}

void Widget::initWidegt()
{
    initDateTimeWidget();
    initHedgedWidget();
    initHedgeTypeWidget();
    initEnergyWidget();
    initAnnounceWidget();
    initDatasourceWidget();
    initProgramWorkInfoWidget();
    initTableContextMenuWidget();
    setPortfolioTableWidget();
    setBuySalePortfolioTableWidget();
}

void Widget::initFileDir()
{
    QString workDir = QCoreApplication::applicationDirPath() + "/";
    m_infoFileName = workDir + "/info.xlsx";
    QFile tmpFile(m_infoFileName);
    if (!tmpFile.exists())
    {
        QXlsx::Document xlsx;
        xlsx.selectSheet("dir");
        m_oriPortfolioFileDir = workDir;
        m_hedgePortfolioFileDir = workDir;
        xlsx.write("A1", m_oriPortfolioFileDir);
        xlsx.write("A2", m_hedgePortfolioFileDir);
        xlsx.saveAs(m_infoFileName);
    }
    else
    {
        QXlsx::Document xlsx(m_infoFileName);
        xlsx.selectSheet("dir");
        QXlsx::CellRange range = xlsx.dimension();

        if (range.rowCount() == 0)
        {
            m_oriPortfolioFileDir = workDir;
            m_hedgePortfolioFileDir = workDir;
            xlsx.selectSheet("dir");
            xlsx.write("A1", m_oriPortfolioFileDir);
            xlsx.write("A2", m_hedgePortfolioFileDir);
            xlsx.save();
        }
        else
        {
            m_oriPortfolioFileDir = xlsx.cellAt(1, 1)->value().toString();
            m_hedgePortfolioFileDir = xlsx.cellAt(2, 1)->value().toString();
        }
    }
}

void Widget::initDateTimeWidget()
{
    ui->startDate->setCalendarPopup(true);
    ui->endDate->setCalendarPopup(true);
    ui->weightDate->setCalendarPopup(true);

    ui->startDate->setDate(QDate::currentDate().addDays(-10));
    ui->endDate->setDate(QDate::currentDate());
    ui->startDate->setMinimumDate(QDate(2007, 1, 1));
    ui->weightDate->setDate(getLastTradingDate());

    ui->energyUpdate_spinBox->setValue(10);
    ui->energyUpdate_spinBox->setMinimum(10);

    if (isStockAMTrading())
    {
        ui->spreadUpdate_spinBox->setValue(4);
    }
    else
    {
       ui->spreadUpdate_spinBox->setValue(6);
    }

    ui->spreadUpdate_spinBox->setMinimum(4);
}

void Widget::initHedgeTypeWidget()
{
    ui->spreadUpdate_spinBox->setValue(2);
    ui->spreadUpdate_spinBox->setMinimum(1);

    ui->HedgeEarning_comboBox->addItem(QString("个股权重不变"), "-2");
    ui->HedgeEarning_comboBox->addItem(QString("个股数量不变"), "-3");
    ui->HedgeEarning_comboBox->addItem(QString(""), "-4");
    ui->HedgeEarning_comboBox->setCurrentText("个股数量不变");
    
    ui->SingleEarning_comboBox->addItem(QString("个股权重不变"), "0");
    ui->SingleEarning_comboBox->addItem(QString("个股数量不变"), "-1");    
    ui->SingleEarning_comboBox->addItem(QString(""), "-4");
    ui->SingleEarning_comboBox->setCurrentText("");

    m_hedgedType = -3;
    m_hedgedString = "净值对冲, 个股数量不变";  
    ui->HedgeEarning_radioButton->setChecked(true);
    ui->SpreadEarning_radioButton->setChecked(false);
    ui->SingleEarning_radioButton->setChecked(false);   
}

void Widget::initHedgedWidget()
{
    ui->hedgeTarget_comboBox->setCurrentText(QString("沪深300"));
    ui->hedgeTarget_comboBox->addItem(QString("沪深300"), "000300");
    ui->hedgeTarget_comboBox->addItem(QString("上证50"), "000016");
    ui->hedgeTarget_comboBox->addItem(QString("中证100"), "399903");
    ui->hedgeTarget_comboBox->addItem(QString("中证200"), "000904");
    ui->hedgeTarget_comboBox->addItem(QString("中证500"), "000905");
    ui->hedgeTarget_comboBox->addItem(QString("中证800"), "000906");
    ui->hedgeTarget_comboBox->addItem(QString("中证1000"), "000852");
    // ui->hedgeCount_spinBox->setValue(2);
    // ui->hedgeCount_spinBox->setRange(-10, 100);

    QStringList timeFre;
    timeFre << "5m"
            << "10m"
            << "15m"
            << "30m"
            << "60m"
            << "120m"
            << "day";

    ui->dataFrequency->addItems(timeFre);
    ui->dataFrequency->setCurrentText("day");

    ui->EMA1TimeSpinBox->setValue(12);
    ui->EMA2TimeSpinBox->setValue(26);
    ui->DIFFTimeSpinBox->setValue(9);
}

void Widget::initEnergyWidget()
{
    m_energyDataFreqListWidget = new QListWidget(this);
    m_timeTypeList << "day"
                   << "120m"
                   << "60m"
                   << "30m"
                   << "15m"
                   << "10m";
    for (auto reqValue : m_timeTypeList)
    {
        QListWidgetItem *pItem = new QListWidgetItem(m_energyDataFreqListWidget);
        pItem->setData(Qt::UserRole, reqValue);

        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(reqValue);

        if (reqValue == "10m")
        {
            pCheckBox->setChecked(true);
        }

        m_energyDataFreqListWidget->addItem(pItem);
        m_energyDataFreqListWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->energyDataFrequency_comboBox->setModel(m_energyDataFreqListWidget->model());
    ui->energyDataFrequency_comboBox->setView(m_energyDataFreqListWidget);

    ui->energySelectCode_lineEdit->insert("SH000300");

    initCSSParamComBox();
}

void Widget::initFutureWidget()
{
    Database database_obj(this, "0", m_currDBHost);
    m_futureList = database_obj.getAllData(m_futureIndexCode, "Future_Info");

    ui->futureList_ComboBox->clear();
    ui->futureList_ComboBox->addItems(m_futureList);
    ui->futureList_ComboBox->setCurrentIndex(0);

    ui->maxSpread_SpinBox->setSingleStep(0.01);
    ui->minSpread_SpinBox->setSingleStep(0.01);

    ui->maxSpread_SpinBox->setRange(-100000, 100000);
    ui->minSpread_SpinBox->setRange(-100000, 100000);

    ui->maxSpread_SpinBox->setValue(100);
    ui->minSpread_SpinBox->setValue(-100);
}

void Widget::initAnnounceWidget()
{
    ui->AnnouncementStartDate->setDate(QDate::currentDate().addDays(-7));
    ui->AnnouncementEndDate->setDate(QDate::currentDate().addDays(0));
}

void Widget::initDatasourceWidget()
{
    ui->dataSource_ComboBox->addItems(m_dbhostList);
    ui->dataSource_ComboBox->setCurrentIndex(0);
}

void Widget::initProgramWorkInfoWidget()
{
    QStandardItemModel *initModel = new QStandardItemModel();
    initModel->setHorizontalHeaderItem(0, new QStandardItem(QString("时间")));
    initModel->setHorizontalHeaderItem(1, new QStandardItem(QString("消息")));
    initModel->setHorizontalHeaderItem(2, new QStandardItem(QString("备注")));
    ui->programInfo_tableView->setModel(initModel);
    ui->programInfo_tableView->setColumnWidth(0, 250);
    ui->programInfo_tableView->setColumnWidth(1, 600);
    ui->programInfo_tableView->setColumnWidth(2, 150);
    ui->programInfo_tableView->setShowGrid(false);
}

void Widget::initTableContextMenuWidget()
{
    ui->strategy_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->buySalePortfolio_table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->strategy_table, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_strategyTable_contextMenu(QPoint)));

    connect(ui->buySalePortfolio_table, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_portfolioTable_contextMenu(QPoint)));
}

void Widget::setPortfolioTableWidget()
{
    m_oriPortfolioFileInfoList = getExcelFileInfo(m_oriPortfolioFileDir, false);

    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(ui->strategy_table->model());
    if (NULL == standardItemModel)
    {
        standardItemModel = new QStandardItemModel();
    }
    else
    {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_oriPortfolioFileInfoList.size(); ++i)
    {
        standardItemModel->setItem(i, 0, new QStandardItem(m_oriPortfolioFileInfoList[i].fileName()));
    }

    standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString("组合组合")));
    ui->strategy_table->setModel(standardItemModel);
    ui->strategy_table->setColumnWidth(0, 300);
    ui->strategy_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->strategy_table->setShowGrid(false);
}

void Widget::setBuySalePortfolioTableWidget()
{
    m_hedgePortfolioFileInfoList = getExcelFileInfo(m_hedgePortfolioFileDir, false);

    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(ui->buySalePortfolio_table->model());
    if (NULL == standardItemModel)
    {
        standardItemModel = new QStandardItemModel();
    }
    else
    {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_hedgePortfolioFileInfoList.size(); ++i)
    {
        standardItemModel->setItem(i, 0, new QStandardItem(m_hedgePortfolioFileInfoList[i].fileName()));
    }

    standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString("对冲组合")));
    ui->buySalePortfolio_table->setModel(standardItemModel);
    ui->buySalePortfolio_table->setColumnWidth(0, 400);
    ui->buySalePortfolio_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->buySalePortfolio_table->setShowGrid(false);
}

void Widget::initCSSParamComBox()
{
    ui->pictureNumb_spinBox->setValue(3);

    m_aveParamListWidget = new QListWidget(this);
    QList<int> aveNumbList;

    aveNumbList << 13 << -25 << -55 << 108 << 144
                << 233 << 320 << 453 << 620 << 730;
    for (int aveValue : aveNumbList)
    {
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

    m_cssParamListWidget = new QListWidget(this);
    QMap<QString, double> aveValue;
    aveValue.insert("主指标平均数: ", 32);
    aveValue.insert("从指标平均数: ", int(32 / 2.6));
    aveValue.insert("势能指标平均数: ", 84);
    for (QMap<QString, double>::iterator it = aveValue.begin(); it != aveValue.end(); ++it)
    {
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

void Widget::initWorkProgressDialog()
{
    QString m_dbhost = ui->dataSource_ComboBox->currentText();
    m_workProgressDialog = new WorkProgressDialog();
    m_histDataWorker = new HistoryData(m_dbhost, m_timeTypeList);

    connect(&m_histDataThread, SIGNAL(finished()),
            m_histDataWorker, SLOT(deleteLater()));

    connect(this, SIGNAL(getTableList_signal()),
            m_histDataWorker, SLOT(getTableList_slot()));

    connect(m_histDataWorker, SIGNAL(sendTableList_signal(QString, QStringList)),
            this, SLOT(sendTableList_slot(QString, QStringList)));

    m_histDataWorker->moveToThread(&m_histDataThread);
    m_histDataThread.start();
}

void Widget::setDBTableNameMap()
{
    initWorkProgressDialog();

    m_workProgressDialog->setWorkInfo(QString("开始读取数据库中已存储的股票与指数信息, 用于验证输入股票或指数代码是否有效[只执行一次]"));
    m_workProgressDialog->setRange(m_timeTypeList.size(), 0);
    m_workProgressDialog->updateWorkState(0);
    m_workProgressDialog->show();
    m_workProgressDialog->setWindowFlags(Qt::WindowStaysOnTopHint);

    emit getTableList_signal();
}

void Widget::sendTableList_slot(QString timeType, QStringList tableList)
{
    QString databaseName = QString("MarketData_%1").arg(timeType);
    m_databaseTableNameMap[timeType] = tableList;

    m_workProgressDialog->updateWorkState(m_databaseTableNameMap.size());

    QString info = QString("开始读取数据库中已存储的股票与指数信息, 用于验证输入股票或指数代码是否有效[只执行一次] \n") + QString("%1 数据库中股票和指数数目为: %2")
                                                                                                                              .arg(timeType)
                                                                                                                              .arg(m_databaseTableNameMap[timeType].size());

    m_workProgressDialog->setWorkInfo(info);
    if (m_timeTypeList.size() == m_databaseTableNameMap.size())
    {
        m_workProgressDialog->close();
    }
}

void Widget::show_strategyTable_contextMenu(QPoint pos)
{
    QMenu *menu = new QMenu(ui->strategy_table);
    QAction *refreshTable = new QAction(QString("刷新"), ui->strategy_table);
    QAction *deleteFile = new QAction(QString("删除"), ui->strategy_table);

    connect(refreshTable, SIGNAL(triggered()), 
            this, SLOT(refresh_strategy_table()));
    connect(deleteFile, SIGNAL(triggered()), this, SLOT(delete_strategy_file()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    menu->move(cursor().pos());
    menu->show();
    pos;
}

void Widget::show_portfolioTable_contextMenu(QPoint pos)
{
    QMenu *menu = new QMenu(ui->buySalePortfolio_table);
    QAction *refreshTable = new QAction(QString("刷新"), ui->buySalePortfolio_table);
    QAction *deleteFile = new QAction(QString("删除"), ui->buySalePortfolio_table);

    connect(refreshTable, SIGNAL(triggered()), this, SLOT(refresh_portfolio_table()));
    connect(deleteFile, SIGNAL(triggered()), this, SLOT(delete_portfolio_file()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    menu->move(cursor().pos());
    menu->show();
    pos;
}

void Widget::refresh_strategy_table()
{
    setPortfolioTableWidget();
}

void Widget::refresh_portfolio_table()
{
    setBuySalePortfolioTableWidget();
}

void Widget::delete_strategy_file()
{
    QMap<int, int> selectMap = getSelectRows(ui->strategy_table);
    QList<QString> selectData = getSelectTableData(ui->strategy_table, selectMap);

    for (QString stringData : selectData)
    {
        QString fileName = m_oriPortfolioFileDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists())
        {
            if (tmpFile.remove())
            {
                updateProgramInfo(ui->programInfo_tableView, QString("成功删除 %1").arg(stringData));
            }
            else
            {
                updateProgramInfo(ui->programInfo_tableView, QString("删除 %1 失败").arg(stringData));
            }
        }
    }
    setPortfolioTableWidget();
}

void Widget::delete_portfolio_file()
{
    QMap<int, int> selectMap = getSelectRows(ui->buySalePortfolio_table);
    QList<QString> selectData = getSelectTableData(ui->buySalePortfolio_table, selectMap);

    for (QString stringData : selectData)
    {
        QString fileName = m_hedgePortfolioFileDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists())
        {
            if (tmpFile.remove())
            {
                updateProgramInfo(ui->programInfo_tableView, QString("成功删除 %1").arg(stringData));
            }
            else
            {
                updateProgramInfo(ui->programInfo_tableView, QString("删除 %1 失败").arg(stringData));
            }
        }
    }
    setBuySalePortfolioTableWidget();
}

QList<int> Widget::getMACDParams()
{
    int EVA1Time = ui->EMA1TimeSpinBox->value();
    int EVA2Time = ui->EMA2TimeSpinBox->value();
    int DIFFTime = ui->DIFFTimeSpinBox->value();
    QList<int> macdTime;
    macdTime << EVA1Time << EVA2Time << DIFFTime;
    return macdTime;
}

QList<int> Widget::getCSSParams()
{
    QList<int> result;
    for (int i = 0; i < m_cssParamListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_cssParamListWidget->item(i);
        QSpinBox *pSpinBox = (QSpinBox *)m_cssParamListWidget->itemWidget(pItem);
        result.append(pSpinBox->value());
    }
    return result;
}

void Widget::checkBuySalePortfolio()
{
    if (m_oriPortfolio.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择原始组合"));
        return;
    }

    if (m_hedgedPortfolio.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择对冲组合"));
        return;
    }

    if (m_oriPortfolio.find("Error") != m_oriPortfolio.end())
    {
        QMessageBox::critical(NULL, "Error", QString("%1 中没有正确的原始组合信息").arg(m_oriPortfolioName));
        return;
    }

    if (m_hedgedPortfolio.find("Error") != m_hedgedPortfolio.end())
    {
        QMessageBox::critical(NULL, "Error", QString("%1 中没有正确的对冲组合信息").arg(m_hedgedPortfolioName));
        return;
    }
}

void Widget::getAveParams(QList<int> &aveNumbList, QList<bool> &isEMAList)
{
    for (int i = 0; i < m_aveParamListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_aveParamListWidget->item(i);
        QSpinBox *pSpinBox = (QSpinBox *)m_aveParamListWidget->itemWidget(pItem);
        int currValue = pSpinBox->value();
        if (currValue > 0)
        {
            aveNumbList.append(currValue);
            isEMAList.append(false);
        }
        else if (currValue < 0)
        {
            aveNumbList.append(0 - currValue);
            isEMAList.append(true);
        }
    }
}

QStringList Widget::getEnergyDataFreq()
{
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

bool Widget::checkCodeInDatabase(QString codeName, QString dbhost, QStringList timeTypeList)
{
    bool result = true;

    if (!m_isDev)
    {
        if (codeName != "")
        {
            for (auto timeType : timeTypeList)
            {
                if (m_databaseTableNameMap[timeType].indexOf(codeName) < 0)
                {
                    qDebug() << QString("TimeType: %1, CodeName: %2").arg(timeType).arg(codeName);
                    QMessageBox::critical(NULL, "Error", QString("选择的代码: %1 不在数据库中").arg(codeName));
                    result = false;
                }
            }
        }
    }

    return result;
}

void Widget::closeEvent(QCloseEvent *event)
{
    event;
    for (int i = 0; i < m_chartViews.size(); ++i)
    {
        if (NULL != m_chartViews[i])
        {
            m_chartViews[i]->close();
            delete m_chartViews[i];
            m_chartViews[i] = NULL;            
        }
    }
}

void Widget::on_SpreadEarning_spinBox_editingFinished()
{
    m_hedgedType = ui->SpreadEarning_spinBox->value();
    m_hedgedString = QString("点差对冲, 笔数: %1").arg(m_hedgedType);

    ui->SpreadEarning_radioButton->setChecked(true);
    ui->SingleEarning_radioButton->setChecked(false);
    ui->HedgeEarning_radioButton->setChecked(false);
}

void Widget::on_HedgeEarning_comboBox_activated(const QString &arg1)
{
    if (arg1 != "")
    {
        m_hedgedType = ui->HedgeEarning_comboBox->currentData().toInt();
        qDebug() << arg1 << m_hedgedType;

        if (m_hedgedType == -2)
        {
            m_hedgedString = "个股权重不变, 净值对冲";
        }
        else
        {
            m_hedgedString = "个股数量不变, 净值对冲";
        }    
        ui->SpreadEarning_radioButton->setChecked(false);
        ui->SingleEarning_radioButton->setChecked(false);
        ui->HedgeEarning_radioButton->setChecked(true);        
    }
}

void Widget::on_SingleEarning_comboBox_activated(const QString &arg1)
{
    if (arg1 != "")
    {
        m_hedgedType = ui->SingleEarning_comboBox->currentData().toInt();
        qDebug() << arg1 << m_hedgedType;

        if (m_hedgedType == 0)
        {
            m_hedgedString = "个股权重不变, 单纯净值";
        }
        else
        {
            m_hedgedString = "个股数量不变, 单纯净值";
        }
        ui->SpreadEarning_radioButton->setChecked(false);
        ui->SingleEarning_radioButton->setChecked(true);
        ui->HedgeEarning_radioButton->setChecked(false);          
    }
}

void Widget::on_weightDate_editingFinished()
{
    m_weightDate = ui->weightDate->date().toString("yyyyMMdd");
}

void Widget::on_strategy_table_clicked(const QModelIndex &index)
{
    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(ui->strategy_table->model());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpName = m_oriPortfolioFileDir + fileName;

    m_oriPortfolioName = (fileName.split("."))[0];
    m_oriPortfolio = readExcelMapDouble(cmpName);

    if (m_oriPortfolio.find("Error") != m_oriPortfolio.end())
    {
        updateProgramInfo(ui->programInfo_tableView, QString("读取组合: %1, 出错").arg(fileName));
    }
    else
    {
        updateProgramInfo(ui->programInfo_tableView, QString("%1 中股票数目为: %2")
                                                         .arg(m_oriPortfolioName)
                                                         .arg(m_oriPortfolio.size()));
    }
}

void Widget::on_buySalePortfolio_table_clicked(const QModelIndex &index)
{
    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(ui->buySalePortfolio_table->model());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpName = m_hedgePortfolioFileDir + fileName;

    m_hedgedPortfolioName = (fileName.split("."))[0];
    updateProgramInfo(ui->programInfo_tableView, QString("读取对冲组合: %1")
                                                     .arg(m_hedgedPortfolioName));

    m_hedgedPortfolio = readExcelMapDouble(cmpName);

    if (m_hedgedPortfolio.find("Error") != m_hedgedPortfolio.end())
    {
        updateProgramInfo(ui->programInfo_tableView, QString("读取组合: %1, 出错").arg(fileName));
    }
    else
    {
        updateProgramInfo(ui->programInfo_tableView, QString(QString("对冲组合股票数目为: %1"))
                                                         .arg(m_hedgedPortfolio.size()));
    }
}

void Widget::on_strategy_table_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(ui->strategy_table->model());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpName = m_oriPortfolioFileDir + fileName;
    QFile tmpFile(cmpName);
    if (tmpFile.exists())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpName));
    }
    else
    {
        QMessageBox::critical(NULL, "Error", QString("当前文件已经被删除,请刷新当前表格"));
    }
}

void Widget::on_buySalePortfolio_table_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel *standardItemModel = dynamic_cast<QStandardItemModel *>(ui->buySalePortfolio_table->model());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpName = m_hedgePortfolioFileDir + fileName;

    QFile tmpFile(cmpName);
    if (tmpFile.exists())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpName));
    }
    else
    {
        QMessageBox::critical(NULL, "Error", QString("当前文件已经被删除,请刷新当前表格"));
    }
}

void Widget::on_showFutureSpread_Button_clicked()
{
    int maxSpreadValue = ui->maxSpread_SpinBox->value();
    int minSpreadValue = ui->minSpread_SpinBox->value();
    QString curFuture = ui->futureList_ComboBox->currentText();
    QString dbhost = ui->dataSource_ComboBox->currentText();
    QWidget *futureChartView = new FutureChart(m_chartViews.size(), dbhost, ui->programInfo_tableView,
                                               curFuture, maxSpreadValue, minSpreadValue);

    connect(futureChartView, SIGNAL(windowClose_signal(int, QString)),
            this,      SLOT(windowClose_slot(int, QString)));

    connect(futureChartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
            this,      SLOT(updateProgramInfo_slot(QString, bool)));  

    futureChartView->show();
    m_chartViews.append(futureChartView);

    updateProgramInfo(ui->programInfo_tableView, QString("期货: %1, 最大基差: %2, 最小基差: %3")
                                                     .arg(curFuture)
                                                     .arg(maxSpreadValue)
                                                     .arg(minSpreadValue));

    updateProgramInfo(ui->programInfo_tableView, QString("当前数据源为: %1").arg(dbhost));
}

void Widget::on_Annoucnement_Button_clicked()
{
    QString startDate = ui->AnnouncementStartDate->date().toString("yyyyMMdd");
    QString endDate = ui->AnnouncementEndDate->date().toString("yyyyMMdd");
    QString dbhost = ui->dataSource_ComboBox->currentText();
    m_announcementView = new AnnouncementForm(QString("%1").arg(m_chartViews.count()), dbhost,
                                              startDate, endDate);
    m_announcementView->show();
    m_chartViews.append(m_announcementView);
}

void Widget::on_chooseStrategyDir_Button_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(NULL, "caption", ".");
    if (dirName != "")
    {
        m_oriPortfolioFileDir = dirName + "/";
        if (writeFileInfo(m_infoFileName, "A1", m_oriPortfolioFileDir, "dir") == 1)
        {
            setPortfolioTableWidget();
            updateProgramInfo(ui->programInfo_tableView, QString(QString("设置新的组合文件夹为: %1"))
                                                             .arg(m_oriPortfolioFileDir));
        }
        else
        {
            QMessageBox::critical(NULL, "Error", QString("设置新的组合文件夹出错"));
        }
    }
}

void Widget::on_choosePortfolioDir_Button_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(NULL, "caption", ".");
    if (dirName != "")
    {
        m_hedgePortfolioFileDir = dirName + "/";
        if (writeFileInfo(m_infoFileName, "A2", m_hedgePortfolioFileDir) == 1)
        {
            setBuySalePortfolioTableWidget();
            updateProgramInfo(ui->programInfo_tableView, QString(QString("设置新的买卖组合文件夹为: %1"))
                                                             .arg(m_hedgePortfolioFileDir));
        }
        else
        {
            QMessageBox::critical(NULL, "Error", QString("设置新的买卖文件夹出错"));
        }
    }
}

void Widget::on_dataSource_ComboBox_currentIndexChanged(const QString &arg1)
{
    m_currDBHost = arg1;
    initFutureWidget();
}

void Widget::on_indexEnergyChart_clicked()
{
    DatabaseParam dbParam;
    HedgedParam hedgeParam;
    CSSParam cssParam;

    dbParam.m_dbhost                = ui->dataSource_ComboBox->currentText();
    hedgeParam.m_startDate          = ui->startDate->date().toString("yyyyMMdd");
    hedgeParam.m_endDate            = ui->endDate->date().toString("yyyyMMdd");
    hedgeParam.m_oriCode            = ui->energySelectCode_lineEdit->text();
    hedgeParam.m_hedgedCode         = ui->energyHedgedCode_lineEdit->text();       
    hedgeParam.m_isCSSChart         = true; 
    hedgeParam.m_isIndex            = true;   
    hedgeParam.m_energyUpdateMSecs  = ui->energyUpdate_spinBox->value() * 1000;

    if (ui->endDate->date() == QDateTime::currentDateTime().date())
    {
        hedgeParam.m_isRealTime   = true;
    }  
    QStringList timeTypeList      = getEnergyDataFreq();     

    QList<int> cssAveNumbList     = getCSSParams();
    QList<int> aveNumbList;
    QList<bool> isEMAList;
    getAveParams(aveNumbList, isEMAList);
    cssParam.m_aveNumbList      = aveNumbList;
    cssParam.m_isEMAList        = isEMAList;
    cssParam.m_mainAveNumb      = cssAveNumbList[0];
    cssParam.m_subAveNumb       = cssAveNumbList[1];
    cssParam.m_energyAveNumb    = cssAveNumbList[2];

    if (timeTypeList.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择时间频率"));
        return;
    }

    if (hedgeParam.m_startDate.toInt() >= hedgeParam.m_endDate.toInt())
    {
        QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
        return;
    }

    QStringList strAveNumbList;
    for (int i = 0; i < aveNumbList.size(); ++i)
    {
        if (isEMAList[i] == false)
        {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]));
        }
        else
        {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i] * -1));
        }
    }

    QString timeType;
    if (timeTypeList.size() == 1)
    {
        timeType = timeTypeList[0];
    }
    else
    {
        timeType = timeTypeList.join(", ");
    }

    QString codeInfo;
    if (hedgeParam.m_hedgedCode.size() == 0)
    {
        codeInfo = QString("目标代码: %1, \n").arg(hedgeParam.m_oriCode);
    }
    else
    {
        codeInfo = QString("目标代码: %1, 对冲代码: %2 \n").arg(hedgeParam.m_oriCode).arg(hedgeParam.m_hedgedCode);
    }

    QString info = QString("开始显示指标图: \n") 
                 + codeInfo 
                 + QString("起始时间: %1, 终止时间: %2 \n").arg(hedgeParam.m_startDate).arg(hedgeParam.m_endDate) 
                 + QString("数据频率: %1 \n主指标平均值: %2 \n从指标平均值: %3 \n势能指标平均值: %4\n")
                 .arg(timeType).arg(cssParam.m_mainAveNumb).arg(cssParam.m_subAveNumb).arg(cssParam.m_energyAveNumb) 
                 + QString("均线参数: %1").arg(strAveNumbList.join(","));

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
    {
        updateProgramInfo(ui->programInfo_tableView, info);
        int maxColNumb = ui->pictureNumb_spinBox->value();

        for (int i = 0; i < timeTypeList.size();)
        {
            int endPos = min(timeTypeList.size(), i + maxColNumb);
            hedgeParam.m_timeTypeList = getSubList(timeTypeList, i, endPos);
            QWidget *chartView = new CSSChartForm(m_chartViews.size(), dbParam, hedgeParam, cssParam);                                                    

            connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                    this,      SLOT(windowClose_slot(int, QString)));

            connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                    this,      SLOT(updateProgramInfo_slot(QString, bool)));  

            m_chartViews.append(chartView);

            chartView->show();

            i = endPos;
        }
    }
}

void Widget::on_portfolioIndexHistSpreadChart_clicked()
{
    if (m_oriPortfolio.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择组合"));
        return;
    }
    else if (m_oriPortfolio.find("Error") != m_oriPortfolio.end())
    {
        QMessageBox::critical(NULL, "Error", QString(QString("读取组合: %1, 出错")).arg(m_oriPortfolioName));
        m_oriPortfolio.clear();
    }
    else
    {
        DatabaseParam dbParam;
        HedgedParam hedgeParam;

        dbParam.m_dbhost            = ui->dataSource_ComboBox->currentText();
        hedgeParam.m_isPortfolio    = true;
        hedgeParam.m_weightDate     = getLastTradingDate(ui->weightDate->date()).toString("yyyyMMdd");
        hedgeParam.m_startDate      = ui->startDate->date().toString("yyyyMMdd");
        hedgeParam.m_endDate        = ui->endDate->date().toString("yyyyMMdd");
        hedgeParam.m_timeType       = ui->dataFrequency->currentText();
        hedgeParam.m_hedgedType     = m_hedgedType;
        hedgeParam.m_oriPortfolio   = m_oriPortfolio;
        hedgeParam.m_portfolioName  = m_oriPortfolioName;
        hedgeParam.m_macdTime       = getMACDParams();
        hedgeParam.m_isPortfolio    = true;
        if (hedgeParam.m_hedgedType != 0)
        {
            hedgeParam.m_hedgedCode = ui->hedgeTarget_comboBox->currentData().toString();
        }        
        hedgeParam.setCodeList();

        if (hedgeParam.m_startDate.toInt() >= hedgeParam.m_endDate.toInt())
        {
            QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
            return;
        }

        QString info = QString("当前数据源为: %1\n组合名称: %2, 组合中股票数目为: %3\n")
                       .arg(dbParam.m_dbhost).arg(m_oriPortfolioName).arg(m_oriPortfolio.size()) 
                     + QString("起始时间: %1 \n终止时间: %2 \n权重日期: %3\n")
                        .arg(hedgeParam.m_startDate).arg(hedgeParam.m_endDate).arg(hedgeParam.m_weightDate)
                     + QString("数据频率: %1\n").arg(hedgeParam.m_timeType) 
                     + QString("对冲目标: %1 \n").arg(hedgeParam.m_hedgedCode)
                     + QString("对冲类型: %1 \n").arg(m_hedgedString) 
                     + QString("T1: %1, T2: %2, T3: %3")
                       .arg(hedgeParam.m_macdTime[0]).arg(hedgeParam.m_macdTime[1]).arg(hedgeParam.m_macdTime[2]);

        if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
        {
            updateProgramInfo(ui->programInfo_tableView, info);

            QWidget *chartView = new ChartForm(m_chartViews.size(), dbParam, hedgeParam);                 
            chartView->show();

            connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                    this,      SLOT(windowClose_slot(int, QString)));

            connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                    this,      SLOT(updateProgramInfo_slot(QString, bool)));  
            m_chartViews.append(chartView);
        }
    }
}

void Widget::on_portfolioIndexRealSpreadChart_clicked()
{
    if (m_oriPortfolio.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择组合"));
        return;
    }
    else if (m_oriPortfolio.find("Error") != m_oriPortfolio.end())
    {
        QMessageBox::critical(NULL, "Error", QString(QString("读取组合: %1, 出错")).arg(m_oriPortfolioName));
        m_oriPortfolio.clear();
    }
    else
    {
        DatabaseParam dbParam;
        HedgedParam hedgeParam;

        dbParam.m_dbhost                = ui->dataSource_ComboBox->currentText();
        hedgeParam.m_weightDate         = getLastTradingDate(ui->weightDate->date()).toString("yyyyMMdd");        
        hedgeParam.m_hedgedType         = m_hedgedType;
        hedgeParam.m_oriPortfolio       = m_oriPortfolio;
        hedgeParam.m_portfolioName      = m_oriPortfolioName;
        hedgeParam.m_macdTime           = getMACDParams();
        hedgeParam.m_isRealTime         = true;
        hedgeParam.m_isPortfolio        = true;
        hedgeParam.m_spreadUpdateMSecs  = ui->spreadUpdate_spinBox->value() * 1000;
        if (hedgeParam.m_hedgedType != 0)
        {
            hedgeParam.m_hedgedCode     = ui->hedgeTarget_comboBox->currentData().toString();
        }

        hedgeParam.setCodeList();

        QString info = QString("当前数据源为: %1\n组合名称: %2, 组合中股票数目为: %3\n")
                       .arg(dbParam.m_dbhost).arg(m_oriPortfolioName).arg(m_oriPortfolio.size()) 
                     + QString("对冲目标: %1\n")
                       .arg(hedgeParam.m_hedgedCode)
                     + QString("对冲类型: %1 \n").arg(m_hedgedString) 
                     + QString("T1: %1, T2: %2, T3: %3")
                       .arg(hedgeParam.m_macdTime[0]).arg(hedgeParam.m_macdTime[1]).arg(hedgeParam.m_macdTime[2]);

        if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
        {
            updateProgramInfo(ui->programInfo_tableView, info);

            QWidget *chartView = new ChartForm(m_chartViews.size(), dbParam, hedgeParam);                                               
            chartView->show();

            connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                    this,      SLOT(windowClose_slot(int, QString)));

            connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                    this,      SLOT(updateProgramInfo_slot(QString, bool)));  

            m_chartViews.append(chartView);
        }
    }
}

void Widget::on_portfolioIndexEnergyChart_clicked()
{
    if (m_oriPortfolio.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择组合"));
        return;
    }
    else if (m_oriPortfolio.find("Error") != m_oriPortfolio.end())
    {
        QMessageBox::critical(NULL, "Error", QString(QString("读取组合: %1, 出错")).arg(m_oriPortfolioName));
        m_oriPortfolio.clear();
        return;
    }

    DatabaseParam dbParam;
    HedgedParam hedgeParam;
    CSSParam cssParam;


    dbParam.m_dbhost                = ui->dataSource_ComboBox->currentText();
    hedgeParam.m_startDate          = ui->startDate->date().toString("yyyyMMdd");
    hedgeParam.m_endDate            = ui->endDate->date().toString("yyyyMMdd");
    hedgeParam.m_weightDate         = getLastTradingDate(ui->weightDate->date()).toString("yyyyMMdd");
    hedgeParam.m_oriPortfolio       = m_oriPortfolio;
    hedgeParam.m_portfolioName      = m_oriPortfolioName; 
    hedgeParam.m_hedgedType         = m_hedgedType;
    hedgeParam.m_energyUpdateMSecs  = ui->energyUpdate_spinBox->value() * 1000;
    if (hedgeParam.m_hedgedType != 0)
    {
        hedgeParam.m_hedgedCode = ui->hedgeTarget_comboBox->currentData().toString();
    }    
    hedgeParam.setCodeList();

    QStringList timeTypeList    = getEnergyDataFreq();
    
    QList<int> cssAveNumbList   = getCSSParams();
    QList<int> aveNumbList;
    QList<bool> isEMAList;
    getAveParams(aveNumbList, isEMAList);

    cssParam.m_aveNumbList      = aveNumbList;
    cssParam.m_isEMAList        = isEMAList;
    cssParam.m_mainAveNumb      = cssAveNumbList[0];
    cssParam.m_subAveNumb       = cssAveNumbList[1];
    cssParam.m_energyAveNumb    = cssAveNumbList[2];

    hedgeParam.m_isCSSChart     = true;
    hedgeParam.m_isPortfolio    = true;        
    if (ui->endDate->date() == QDateTime::currentDateTime().date())
    {
        hedgeParam.m_isRealTime = true;
    }        

    if (timeTypeList.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择时间频率"));
        return;
    }
    if (hedgeParam.m_startDate.toInt() >= hedgeParam.m_endDate.toInt())
    {
        QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
        return;
    }

    QStringList strAveNumbList;
    for (int i = 0; i < aveNumbList.size(); ++i)
    {
        if (isEMAList[i] == false)
        {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]));
        }
        else
        {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i] * -1));
        }
    }

    QString timeType;
    if (timeTypeList.size() == 1)
    {
        timeType = timeTypeList[0];
    }
    else
    {
        timeType = timeTypeList.join(", ");
    }

    QString hedgedInfo = "";
    if (m_hedgedType != 0)
    {
        hedgedInfo = QString("对冲目标: %1\n").arg(hedgeParam.m_hedgedCode);
    }
    hedgedInfo += QString("对冲方式: %1\n").arg(m_hedgedString);

    QString info = QString("当前的数据源为: %1 \n").arg(dbParam.m_dbhost) 
                    + QString("组合名称: %1, \n组合中股票数目为: %2 \n").arg(m_oriPortfolioName).arg(m_oriPortfolio.size()) 
                    + hedgedInfo 
                    + QString("起始时间: %1 \n终止时间: %2 \n权重日期: %3\n")
                      .arg(hedgeParam.m_startDate).arg(hedgeParam.m_endDate).arg(hedgeParam.m_weightDate)
                    + QString("数据频率: %1 \n主指标平均值: %2 \n从指标平均值: %3 \n势能指标平均值: %4\n")
                    .arg(timeType).arg(cssParam.m_mainAveNumb).arg(cssParam.m_subAveNumb).arg(cssParam.m_energyAveNumb) 
                    + QString("均线参数: %1\n").arg(strAveNumbList.join(","));

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
    {
        updateProgramInfo(ui->programInfo_tableView, info);
        int maxColNumb = ui->pictureNumb_spinBox->value();

        for (int i = 0; i < timeTypeList.size();)
        {
            int endPos = min(timeTypeList.size(), i + maxColNumb);
            hedgeParam.m_timeTypeList = getSubList(timeTypeList, i, endPos);
            
            // QWidget *chartView = new CSSChartForm(m_chartViews.size(), dbParam.m_dbhost, currTimeTypeList,
            //                                       startDate, endDate,
            //                                       m_oriPortfolio, EmpytQStringDoubleMap(),
            //                                       m_oriPortfolioName,
            //                                       hedgeParam.m_hedgedCode, hedgeParam.m_hedgedType,
            //                                       aveNumbList, isEMAList,
            //                                       mainAveNumb, subAveNumb, energyAveNumb,
            //                                       css12Rate, mainCssRate1, mainCssRate2,
            //                                       energyCssRate1, energyCssRate2,
            //                                       maxCSS, minCSS);

            QWidget *chartView = new CSSChartForm(m_chartViews.size(), dbParam, hedgeParam, cssParam);

            connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                    this,      SLOT(windowClose_slot(int, QString)));

            connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                    this,      SLOT(updateProgramInfo_slot(QString, bool)));  

            m_chartViews.append(chartView);

            chartView->show();
            i = endPos;
        }
    }    
}

void Widget::on_portfolioHedgedHistSpreadChart_clicked()
{
    checkBuySalePortfolio();

    DatabaseParam dbParam;
    HedgedParam hedgeParam;

    dbParam.m_dbhost                = ui->dataSource_ComboBox->currentText();
    hedgeParam.m_oriPortfolio       = m_oriPortfolio;
    hedgeParam.m_hedgedPortfolio    = m_hedgedPortfolio;
    hedgeParam.m_portfolioName      = m_oriPortfolioName + ", " + m_hedgedPortfolioName;
    hedgeParam.m_startDate          = ui->startDate->date().toString("yyyyMMdd");
    hedgeParam.m_endDate            = ui->endDate->date().toString("yyyyMMdd");
    hedgeParam.m_weightDate         = getLastTradingDate(ui->weightDate->date()).toString("yyyyMMdd");
    hedgeParam.m_hedgedType         = m_hedgedType;
    hedgeParam.m_timeType           = ui->dataFrequency->currentText();    
    hedgeParam.m_isPortfolioHedge   = true;
    hedgeParam.m_isPortfolio        = true;
    hedgeParam.m_macdTime           = getMACDParams();    
    hedgeParam.setCodeList();

    if (hedgeParam.m_hedgedType != MKVALUE_WEIGHT_HEDGE && hedgeParam.m_hedgedType != CLVALUE_WEIGHT_HEDGE)
    {
        QMessageBox::critical(NULL, "Error", QString("未选择正确的净值对冲方式"));
        return;        
    }

    QString info = QString("数据源为: %1 \n").arg(dbParam.m_dbhost) 
                 + QString("原始组合名称: %1,  组合中股票数目为: %2 \n").arg(m_oriPortfolioName).arg(m_oriPortfolio.size()) 
                 + QString("对冲组合名称: %1,  组合中股票数目为: %2\n").arg(m_hedgedPortfolioName).arg(m_hedgedPortfolio.size()) 
                 + QString("对冲类型: %1 \n").arg(m_hedgedString) 
                 + QString("起始时间: %1 \n终止时间: %2 \n权重时间: %3")
                   .arg(hedgeParam.m_startDate).arg(hedgeParam.m_endDate).arg(hedgeParam.m_weightDate)
                 + QString("数据频率: %1 \n").arg(hedgeParam.m_timeType )
                 + QString("T1: %1, T2: %2, T3: %3 \n")
                   .arg(hedgeParam.m_macdTime[0]).arg(hedgeParam.m_macdTime[1]).arg(hedgeParam.m_macdTime[2]);

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
    {
        updateProgramInfo(ui->programInfo_tableView, info);
        QWidget *chartView = new ChartForm(m_chartViews.size(), dbParam, hedgeParam);
        chartView->show();

        connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                this,      SLOT(windowClose_slot(int, QString)));

        connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                this,      SLOT(updateProgramInfo_slot(QString, bool)));  

        m_chartViews.append(chartView);
    }
}

void Widget::on_portfolioHedgedRealSpreadChart_clicked()
{
    checkBuySalePortfolio();
    DatabaseParam dbParam;
    HedgedParam hedgeParam;

    dbParam.m_dbhost                = ui->dataSource_ComboBox->currentText();
    hedgeParam.m_weightDate         = getLastTradingDate(ui->weightDate->date()).toString("yyyyMMdd");
    hedgeParam.m_oriPortfolio       = m_oriPortfolio;
    hedgeParam.m_hedgedPortfolio    = m_hedgedPortfolio;
    hedgeParam.m_portfolioName      = m_oriPortfolioName + ", " + m_hedgedPortfolioName;
    hedgeParam.m_timeType           = ui->dataFrequency->currentText(); 
    hedgeParam.m_hedgedType         = m_hedgedType;   
    hedgeParam.m_isPortfolioHedge   = true;
    hedgeParam.m_isPortfolio        = true;
    hedgeParam.m_isRealTime         = true;
    hedgeParam.m_macdTime           = getMACDParams();
    hedgeParam.m_spreadUpdateMSecs  = ui->spreadUpdate_spinBox->value() * 1000;
    hedgeParam.setCodeList();

    if (hedgeParam.m_hedgedType != MKVALUE_WEIGHT_HEDGE && hedgeParam.m_hedgedType != CLVALUE_WEIGHT_HEDGE)
    {
        QMessageBox::critical(NULL, "Error", QString("未选择正确的净值对冲方式"));
        return;        
    }

    QString info = QString("数据源为: %1 \n").arg(dbParam.m_dbhost) 
                 + QString("原始组合名称: %1,  组合中股票数目为: %2 \n")
                   .arg(m_oriPortfolioName).arg(m_oriPortfolio.size()) 
                 + QString("对冲组合名称: %1,  组合中股票数目为: %2\n")
                   .arg(m_hedgedPortfolioName).arg(m_hedgedPortfolio.size()) 
                 + QString("权重日期: %1 \n").arg(hedgeParam.m_weightDate)
                 + QString("对冲类型: %1 \n").arg(m_hedgedString) 
                 + QString("T1: %1, T2: %2, T3: %3 \n")
                   .arg(hedgeParam.m_macdTime[0]).arg(hedgeParam.m_macdTime[1]).arg(hedgeParam.m_macdTime[2]);

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
    {
        updateProgramInfo(ui->programInfo_tableView, info);

        QWidget *chartView = new ChartForm(m_chartViews.size(), dbParam, hedgeParam);                                           
        chartView->show();

        connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                this,      SLOT(windowClose_slot(int, QString)));

        connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                this,      SLOT(updateProgramInfo_slot(QString, bool)));        

        m_chartViews.append(chartView);
    }
}

void Widget::on_portfolioHedgedEnergyChart_clicked()
{
    checkBuySalePortfolio();

    DatabaseParam dbParam;
    HedgedParam hedgeParam;
    CSSParam cssParam;

    dbParam.m_dbhost                = ui->dataSource_ComboBox->currentText();
    hedgeParam.m_startDate          = ui->startDate->date().toString("yyyyMMdd");
    hedgeParam.m_endDate            = ui->endDate->date().toString("yyyyMMdd");
    hedgeParam.m_weightDate         = getLastTradingDate(ui->weightDate->date()).toString("yyyyMMdd");
    hedgeParam.m_oriPortfolio       = m_oriPortfolio;
    hedgeParam.m_hedgedPortfolio    = m_hedgedPortfolio;
    hedgeParam.m_portfolioName      = m_oriPortfolioName + ", " +m_hedgedPortfolioName;            
    hedgeParam.m_hedgedType         = m_hedgedType;
    hedgeParam.m_energyUpdateMSecs  = ui->energyUpdate_spinBox->value() * 1000;
    hedgeParam.m_isCSSChart         = true; 
    hedgeParam.m_isPortfolio        = true;        
    hedgeParam.m_isPortfolioHedge   = true;

    if (ui->endDate->date() == QDateTime::currentDateTime().date())
    {
        hedgeParam.m_isRealTime   = true;
    }   
    hedgeParam.setCodeList();

    QStringList timeTypeList      = getEnergyDataFreq();

    QList<int> cssAveNumbList     = getCSSParams();
    QList<int> aveNumbList;
    QList<bool> isEMAList;
    getAveParams(aveNumbList, isEMAList);
    cssParam.m_aveNumbList      = aveNumbList;
    cssParam.m_isEMAList        = isEMAList;
    cssParam.m_mainAveNumb      = cssAveNumbList[0];
    cssParam.m_subAveNumb       = cssAveNumbList[1];
    cssParam.m_energyAveNumb    = cssAveNumbList[2];

    if (hedgeParam.m_hedgedType != MKVALUE_WEIGHT_HEDGE && hedgeParam.m_hedgedType != CLVALUE_WEIGHT_HEDGE)
    {
        QMessageBox::critical(NULL, "Error", QString("未选择正确的净值对冲方式"));
        return;        
    }

    if (timeTypeList.size() == 0)
    {
        QMessageBox::critical(NULL, "Error", QString("还未选择时间频率"));
        return;
    }

    if (hedgeParam.m_startDate.toInt() >= hedgeParam.m_endDate.toInt())
    {
        QMessageBox::critical(NULL, "Error", QString("起始时间晚于截止时间"));
        return;
    }

    QStringList strAveNumbList;
    for (int i = 0; i < aveNumbList.size(); ++i)
    {
        if (isEMAList[i] == false)
        {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i]));
        }
        else
        {
            strAveNumbList.append(QString("%1").arg(aveNumbList[i] * -1));
        }
    }

    QString timeType;
    if (timeTypeList.size() == 1)
    {
        timeType = timeTypeList[0];
    }
    else
    {
        timeType = timeTypeList.join(", ");
    }

    QString info = QString("数据源为: %1 \n").arg(dbParam.m_dbhost) 
                 + QString("原始组合名称: %1,  组合中股票数目为: %2 \n").arg(m_oriPortfolioName).arg(m_oriPortfolio.size()) 
                 + QString("对冲组合名称: %1,  组合中股票数目为: %2\n").arg(m_hedgedPortfolioName).arg(m_hedgedPortfolio.size()) 
                 + QString("对冲类型: %1 \n").arg(m_hedgedString) 
                 + QString("起始时间: %1 \n终止时间: %2 \n权重时间: %3\n")
                  .arg(hedgeParam.m_startDate).arg(hedgeParam.m_endDate).arg(hedgeParam.m_weightDate)
                 + QString("数据频率: %1 \n主指标平均值: %2 \n从指标平均值: %3 \n势能指标平均值: %4\n")
                  .arg(timeType).arg(cssParam.m_mainAveNumb).arg(cssParam.m_subAveNumb).arg(cssParam.m_energyAveNumb) 
                 + QString("均线参数: %1\n").arg(strAveNumbList.join(","));

    if (QMessageBox::Yes == QMessageBox::warning(this, "确认信息", info, QMessageBox::Yes | QMessageBox::No))
    {
        updateProgramInfo(ui->programInfo_tableView, info);
        int maxColNumb = ui->pictureNumb_spinBox->value();

        for (int i = 0; i < timeTypeList.size();)
        {
            int endPos = min(timeTypeList.size(), i + maxColNumb);
            hedgeParam.m_timeTypeList = getSubList(timeTypeList, i, endPos);

            QWidget *chartView = new CSSChartForm(m_chartViews.size(), dbParam, hedgeParam, cssParam);                                                  

            connect(chartView, SIGNAL(windowClose_signal(int, QString)),
                    this,      SLOT(windowClose_slot(int, QString)));

            connect(chartView, SIGNAL(updateProgramInfo_signal(QString, bool)),
                    this,      SLOT(updateProgramInfo_slot(QString, bool)));  

            m_chartViews.append(chartView);

            chartView->show();
            i = endPos;
        }
    }
}

void Widget::updateProgramInfo_slot(QString info, bool isWarning)
{
   updateProgramInfo(ui->programInfo_tableView, info);
   if (isWarning)
   {
       QMessageBox::warning(this, "警告", info);
   }
}

void Widget::windowClose_slot(int windowID, QString windowName)
{
    if (windowID >= 0 && windowID < m_chartViews.size())
    {
        if (m_chartViews[windowID] != NULL)
        {
            delete m_chartViews[windowID];
            m_chartViews[windowID] = NULL;
        }
        QString info = QString("-- %1 closed --").arg(windowName);
        qDebug() << info;
        updateProgramInfo(ui->programInfo_tableView, info);
    }
}

void Widget::on_SpreadEarning_spinBox_valueChanged(const QString &arg1)
{
    m_hedgedType = ui->SpreadEarning_spinBox->value();
    m_hedgedString = QString("点差对冲, 笔数: %1").arg(m_hedgedType);

    ui->SpreadEarning_radioButton->setChecked(true);
    ui->SingleEarning_radioButton->setChecked(false);
    ui->HedgeEarning_radioButton->setChecked(false);
    arg1;
}

void Widget::on_SingleEarning_radioButton_clicked(bool checked)
{
    if (checked)
    {
        m_hedgedType = ui->SingleEarning_comboBox->currentData().toInt();

        if (m_hedgedType == 0)
        {
            m_hedgedString = "个股权重不变, 单纯净值";
        }
        else
        {
            m_hedgedString = "个股数量不变, 单纯净值";
        }
        ui->SpreadEarning_radioButton->setChecked(false);
        ui->SingleEarning_radioButton->setChecked(true);
        ui->HedgeEarning_radioButton->setChecked(false);            
    }
}

void Widget::on_HedgeEarning_radioButton_clicked(bool checked)
{
    if (checked)
    {
        m_hedgedType = ui->HedgeEarning_comboBox->currentData().toInt();

        if (m_hedgedType == -2)
        {
            m_hedgedString = "个股权重不变, 净值对冲";
        }
        else
        {
            m_hedgedString = "个股数量不变, 净值对冲";
        }    
        ui->SpreadEarning_radioButton->setChecked(false);
        ui->SingleEarning_radioButton->setChecked(false);
        ui->HedgeEarning_radioButton->setChecked(true);           
    }
}

void Widget::on_SpreadEarning_radioButton_clicked(bool checked)
{
    if (checked)
    {
        m_hedgedType = ui->SpreadEarning_spinBox->value();
        m_hedgedString = QString("点差对冲, 笔数: %1").arg(m_hedgedType);

        ui->SpreadEarning_radioButton->setChecked(true);
        ui->SingleEarning_radioButton->setChecked(false);
        ui->HedgeEarning_radioButton->setChecked(false);
    }

}
