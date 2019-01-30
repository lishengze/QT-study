#include <QFile>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDate>
#include <QCheckBox>
#include "database.h"
#include "extractdataform.h"
#include "ui_extractdataform.h"
#include "xlsxdocument.h"
#include "io_func.h"
#include "excel_func.h"
#include "widget_func.h"
#include "secode_func.h"
#include "choosesecodewindow.h"
#pragma execution_character_set("utf-8")

ExtractDataForm::ExtractDataForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExtractDataForm)
{
    ui->setupUi(this);
    initCommonData();
    initWidget();
}

ExtractDataForm::~ExtractDataForm()
{
    delete ui;

    for (int i = 0; i < m_extractMarketDataObjList.size();++i) {
        if (NULL != m_extractMarketDataObjList[i]) {
            delete m_extractMarketDataObjList[i];
            m_extractMarketDataObjList[i] = NULL;
        }
    }

    for (int i = 0; i < m_extractWeightDataObjList.size();++i) {
        if (NULL != m_extractWeightDataObjList[i]) {
            delete m_extractWeightDataObjList[i];
            m_extractWeightDataObjList[i] = NULL;
        }
    }

    for (int i = 0; i < m_extractIndustryDataObjList.size();++i) {
        if (NULL != m_extractIndustryDataObjList[i]) {
            delete m_extractIndustryDataObjList[i];
            m_extractIndustryDataObjList[i] = NULL;
        }
    }

    for (int i = 0; i < m_chooseSecodeWindowList.size(); ++i) {
        if (NULL != m_chooseSecodeWindowList[i]) {
            delete m_chooseSecodeWindowList[i];
            m_chooseSecodeWindowList[i] = NULL;
        }
    }
}

void ExtractDataForm::initCommonData() {
    m_currOriFile = "";
    m_extractTimes = 0;
    m_keyValueList.clear();
    m_extractThreadCount = 16;

    initDataTypeMap();
    initKeyValueMap();
    initIndexCodeMap();
    initFundamentKeyMap();
    initIndustryList();

    initFileDir();
}

void ExtractDataForm::initFileDir() {
    m_nativeFileName = "./ExtractDataDir.xlsx";
    QFile tmpFile(m_nativeFileName);
    if (!tmpFile.exists()) {
        qDebug() << QString("%1 does not exit!").arg(m_nativeFileName);
        QXlsx::Document xlsx;
        m_oriFileDir = QString("//192.168.211.182/it程序设计/提取数据/");
        m_desFileDir = m_oriFileDir;
        xlsx.write("A1", m_oriFileDir);
        xlsx.write("A2", m_desFileDir);
        xlsx.saveAs(m_nativeFileName);
    } else {
        QXlsx::Document xlsx(m_nativeFileName);
        xlsx.selectSheet("1");
        QXlsx::CellRange range = xlsx.dimension();
        if (range.rowCount() == 0) {
            m_oriFileDir = QString("//192.168.211.182/it程序设计/提取数据/");
            m_desFileDir = m_oriFileDir;
            xlsx.write("A1", m_oriFileDir);
            xlsx.write("A2", m_desFileDir);
            xlsx.save();
        } else {
            m_oriFileDir = xlsx.cellAt(1, 1)->value().toString();
            m_desFileDir = xlsx.cellAt(2, 1)->value().toString();
        }
    }
    //    qDebug() << "m_oriFileDir: " << m_oriFileDir;
    //    qDebug() << "m_desFileDir: " << m_desFileDir;
}

void ExtractDataForm::initDataTypeMap() {

    //    m_dataTypeMap.insert(QString("1分"), "MarketData_1m");
    //    m_dataTypeMap.insert(QString("5分"), "MarketData_5m");
    m_dataTypeMap.insert(QString("10分"), "MarketData_10m");
    m_dataTypeMap.insert(QString("15分"), "MarketData_15m");
    m_dataTypeMap.insert(QString("30分"), "MarketData_30m");
    m_dataTypeMap.insert(QString("60分"), "MarketData_60m");
    m_dataTypeMap.insert(QString("120分"), "MarketData_120m");
    m_dataTypeMap.insert(QString("天"), "MarketData_day");
    m_dataTypeMap.insert(QString("周"), "MarketData_week");
    m_dataTypeMap.insert(QString("月"), "MarketData_month");
}

void ExtractDataForm::initKeyValueMap() {
    m_keyValueMap.insert(QString("开盘价"), "TOPEN");
    m_keyValueMap.insert(QString("收盘价"), "TCLOSE");
    m_keyValueMap.insert(QString("最高价"), "HIGH");
    m_keyValueMap.insert(QString("最低价"), "LOW");
    m_keyValueMap.insert(QString("成交量"), "VATRUNOVER");
    m_keyValueMap.insert(QString("成交额"), "VOTRUNOVER");
    m_keyValueMap.insert(QString("昨收"), "YCLOSE");
    m_keyValueMap.insert(QString("换手率"), "TURNOVER");
}

void ExtractDataForm::initIndexCodeMap() {
    m_indexCodeMap.insert(QString("上证50"), "SH000016");
    m_indexCodeMap.insert(QString("沪深300"), "SH000300");
    m_indexCodeMap.insert(QString("中证1000"), "SH000852");
    m_indexCodeMap.insert(QString("中证200"), "SH000904");
    m_indexCodeMap.insert(QString("中证500"), "SH000905");
    m_indexCodeMap.insert(QString("中证800"), "SH000906");
    m_indexCodeMap.insert(QString("中证100"), "SZ399903");
}

void ExtractDataForm::initFundamentKeyMap() {
    m_FundamentKeyMap.insert(QString("市盈增长率"), "PEG");
    m_FundamentKeyMap.insert(QString("市盈率"), "PE");
    m_FundamentKeyMap.insert(QString("市净率"), "PB");
    m_FundamentKeyMap.insert(QString("市销率"), "PS");
    m_FundamentKeyMap.insert(QString("每股现金流量净额"), "CFP");
    m_FundamentKeyMap.insert(QString("每股收益"), "EPS");
    m_FundamentKeyMap.insert(QString("总市值"), "MV");
    m_FundamentKeyMap.insert(QString("单季度.净资产收益率"), "ROE");
}

void ExtractDataForm::initIndustryList() {
    m_industryList.append(QString("中证一级行业"));
    m_industryList.append(QString("中证二级行业"));
    m_industryList.append(QString("中证三级行业"));
    m_industryList.append(QString("申万一级行业"));
    m_industryList.append(QString("申万二级行业"));
    m_industryList.append(QString("申万三级行业"));
    m_industryList.append(QString("万得一级行业"));
    m_industryList.append(QString("万得二级行业"));
    m_industryList.append(QString("万得三级行业"));
    m_industryList.append(QString("万得四级行业"));
    m_industryList.append(QString("全选"));
}

void ExtractDataForm::initWidget() {
    initDate();
    initDataSourceCombox();
    initDataTypeCombox();

    initMarketKeyComboBox();
    initIndexCodeComboBox();
    initIndustryComboBox();
    initFundamentKeyComboBox();

    initTableContextMenu();
    initProgramTable();
    setOriFileTable();
    setDesFileTable();
}

void ExtractDataForm::initDate() {
    QDate today = QDate::currentDate();
    QDate startDate = today.addDays(-365);
    QDate endDate = today.addDays(-1);
    ui->startDate_dateEdit->setDate(startDate);
    ui->endDate_dateEdit->setDate(endDate);

    ui->indexStartDate_dateEdit->setDate(today.addDays(-7));
    ui->indexEndDate_dateEdit->setDate(today);

    ui->industryStartDate_dateEdit->setDate(today.addDays(-7));
    ui->industryEndDate_dateEdit->setDate(endDate);

    ui->startDate_dateEdit->setCalendarPopup(true);
    ui->endDate_dateEdit->setCalendarPopup(true);

    ui->indexStartDate_dateEdit->setCalendarPopup(true);
    ui->indexEndDate_dateEdit->setCalendarPopup(true);

    ui->industryStartDate_dateEdit->setCalendarPopup(true);
    ui->industryEndDate_dateEdit->setCalendarPopup(true);
}

void ExtractDataForm::initMarketKeyComboBox() {
    m_marketKeyListWidget = new QListWidget(this);
    for (QMap<QString, QString>::iterator it = m_keyValueMap.begin();
         it != m_keyValueMap.end(); ++it) {
        QListWidgetItem *pItem = new QListWidgetItem(m_marketKeyListWidget);
        pItem->setData(Qt::UserRole, it.key());
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(it.key());
        m_marketKeyListWidget->addItem(pItem);
        m_marketKeyListWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->keyValue_comboBox->setModel(m_marketKeyListWidget->model());
    ui->keyValue_comboBox->setView(m_marketKeyListWidget);
}

void ExtractDataForm::initIndexCodeComboBox() {
    m_weightIndexListWidget = new QListWidget(this);
    for (QMap<QString, QString>::iterator it = m_indexCodeMap.begin();
         it != m_indexCodeMap.end(); ++it) {
        QListWidgetItem *pItem = new QListWidgetItem(m_weightIndexListWidget);
        m_weightIndexListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, it.key());
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(it.key());
        m_weightIndexListWidget->addItem(pItem);
        m_weightIndexListWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->indexCode_comboBox->setModel(m_weightIndexListWidget->model());
    ui->indexCode_comboBox->setView(m_weightIndexListWidget);
}

void ExtractDataForm::initIndustryComboBox() {
    m_setIndustryCombox = false;
    m_industryListWidget = new QListWidget(this);
    for (QString industryCode: m_industryList) {
        QListWidgetItem *pItem = new QListWidgetItem(m_industryListWidget);
        m_industryListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, industryCode);
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(industryCode);
        m_industryListWidget->addItem(pItem);
        m_industryListWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->industry_comboBox->setModel(m_industryListWidget->model());
    ui->industry_comboBox->setView(m_industryListWidget);
}

void ExtractDataForm::initFundamentKeyComboBox() {
    m_fundamentKeyWidget = new QListWidget(this);
    for (QMap<QString, QString>::iterator it = m_FundamentKeyMap.begin();
         it != m_FundamentKeyMap.end(); ++it) {
        QListWidgetItem *pItem = new QListWidgetItem(m_fundamentKeyWidget);
        pItem->setData(Qt::UserRole, it.key());
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(it.key());
        m_fundamentKeyWidget->addItem(pItem);
        m_fundamentKeyWidget->setItemWidget(pItem, pCheckBox);
    }
    ui->fundamentValue_comboBox->setModel(m_fundamentKeyWidget->model());
    ui->fundamentValue_comboBox->setView(m_fundamentKeyWidget);
}

void ExtractDataForm::initDataSourceCombox() {
    m_dbhostList << "192.168.211.162"
                 << "192.168.211.165";

    ui->chooseDataSource_comboBox->addItems(m_dbhostList);
    ui->chooseDataSource_comboBox->setCurrentIndex(0);
}

void ExtractDataForm::initDataTypeCombox() {
    QList<QString> dataTypeList = m_dataTypeMap.keys();
    for (int i = dataTypeList.size() - 1; i >= 0; --i) {
        ui->dataType_comboBox->addItem(dataTypeList[i]);
    }
}

void ExtractDataForm::initKeyValueCombox() {
    for (QMap<QString, QString>::iterator it = m_keyValueMap.begin(); it != m_keyValueMap.end(); ++it) {
        ui->keyValue_comboBox->addItem(it.key());
    }
}

void ExtractDataForm::initProgramTable() {
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->programInfo_Table->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
    } else {
        standardItemModel->clear();
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("时间")));
    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString("信息")));
    standardItemModel-> setHorizontalHeaderItem (2, new QStandardItem(QString("备注")));
    ui->programInfo_Table->setModel(standardItemModel);
    ui->programInfo_Table->setColumnWidth (0, 150);
    ui->programInfo_Table->setColumnWidth (1, 650);
    ui->programInfo_Table->setColumnWidth (2, 200);
    ui->programInfo_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->programInfo_Table->setShowGrid (false);
}

void ExtractDataForm::setOriFileTable() {
    m_oriFileList = getExcelFileInfo(m_oriFileDir);
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->oriFile_tableView->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
    } else {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_oriFileList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(m_oriFileList[i].fileName()));
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("源信息文件")));
    ui->oriFile_tableView->setModel(standardItemModel);
    ui->oriFile_tableView->setColumnWidth (0, 500);
    ui->oriFile_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->oriFile_tableView->setShowGrid (false);
}

void ExtractDataForm::setDesFileTable() {
    m_desFileList = getExcelFileInfo(m_desFileDir);
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->desFile_tableView->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
    } else {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_desFileList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(m_desFileList[i].fileName()));
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("生成文件")));
    ui->desFile_tableView->setModel(standardItemModel);
    ui->desFile_tableView->setColumnWidth (0, 500);
    ui->desFile_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->desFile_tableView->setShowGrid (false);
}

void ExtractDataForm::initTableContextMenu() {
    ui->oriFile_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->desFile_tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->oriFile_tableView,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_oriFileTable_contextMenu(QPoint)));

    connect(ui->desFile_tableView,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_desFileTable_contextMenu(QPoint)));
}

QList<QString> ExtractDataForm::extractTimeList()
{
    QString file_name = QFileDialog::getOpenFileName(NULL, QString("选择导入的EXCEL, 文件第一列为时间"),
                                                  ".", QString("Excel Files(*.xlsx)"));
    QList<QString> result;                 
    if (file_name != "") 
    {
        updateProgramInfo(ui->programInfo_Table, QString("选择了股票代码列表文件: %1").arg(m_currOriFile));
        QList<QString> oriTimeList = readExcelColData(file_name);
        for (int i = 0; i < oriTimeList.size(); ++i)
        {
            if (oriTimeList[i].indexOf("-") >= 0)
            {
                oriTimeList[i].remove(QChar('-'));
            }
        }
        result = oriTimeList;
        printList(oriTimeList, "TransTimeList");
    }    
    return result;
}

void ExtractDataForm::show_oriFileTable_contextMenu(QPoint pos) {
    QMenu *menu = new QMenu(ui->oriFile_tableView);
    QAction *refreshTable = new QAction(QString("刷新"),ui->oriFile_tableView);
    QAction *deleteFile = new QAction(QString("删除"),ui->oriFile_tableView);
    QAction *openDir = new QAction(QString("打开所在文件夹"),ui->oriFile_tableView);

    connect (refreshTable,SIGNAL(triggered()),this,SLOT(refresh_oriFileTable()));
    connect (deleteFile, SIGNAL(triggered()),this,SLOT(delete_oriFile()));
    connect (openDir, SIGNAL(triggered()),this,SLOT(open_oriDir()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    //    menu->addAction(openDir);
    menu->move (cursor ().pos ());
    menu->show ();
    pos;
}

void ExtractDataForm::show_desFileTable_contextMenu(QPoint pos) {
    QMenu *menu = new QMenu(ui->desFile_tableView);
    QAction *refreshTable = new QAction(QString("刷新"),ui->desFile_tableView);
    QAction *deleteFile = new QAction(QString("删除"),ui->desFile_tableView);
    QAction *openDir = new QAction(QString("打开所在文件夹"),ui->desFile_tableView);

    connect (refreshTable,SIGNAL(triggered()),this,SLOT(refresh_desFileTable()));
    connect (deleteFile, SIGNAL(triggered()),this,SLOT(delete_desFile()));
    connect (openDir, SIGNAL(triggered()),this,SLOT(open_desDir()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    //    menu->addAction(openDir);
    menu->move (cursor ().pos ());
    menu->show ();
    pos;
}

void ExtractDataForm::refresh_oriFileTable() {
    setOriFileTable();
}

void ExtractDataForm::refresh_desFileTable() {
    setDesFileTable();
}

void ExtractDataForm::delete_oriFile() {
    QMap<int, int> selectMap = getSelectRows(ui->oriFile_tableView);
    QList<QString> selectData = getSelectTableData(ui->oriFile_tableView, selectMap);
    //    qDebug() << "selectData: " << selectData;

    for (QString stringData : selectData) {
        QString fileName = m_oriFileDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_Table, QString("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString("删除 %1 失败").arg(stringData));
            }
        }
    }
    setOriFileTable();
}

void ExtractDataForm::delete_desFile() {
    QMap<int, int> selectMap = getSelectRows(ui->desFile_tableView);
    QList<QString> selectData = getSelectTableData(ui->desFile_tableView, selectMap);

    for (QString stringData : selectData) {
        QString fileName = m_desFileDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_Table, QString("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString("删除 %1 失败").arg(stringData));
            }
        }
    }
    setDesFileTable();
}

void ExtractDataForm::open_oriDir() {
    qDebug() <<"m_oriFileDir: " << m_oriFileDir ;
    QString dir = QString(m_oriFileDir.toStdString().c_str());
    if (m_oriFileDir.startsWith("//")) {
        dir = QString("file:%1").arg(QString(m_oriFileDir.toStdString().c_str()));
    }
    dir = QString("D:/课程/");
    qDebug() << "dir: " << dir;
    QDesktopServices::openUrl(QUrl(dir, QUrl::TolerantMode));
}

void ExtractDataForm::open_desDir() {
    qDebug() <<"m_desFileDir: " << m_desFileDir ;
    QString dir = QString(m_desFileDir.toStdString().c_str());
    if (m_desFileDir.startsWith("//")) {
        dir = QString("file:%1").arg(QString(m_desFileDir.toStdString().c_str()));
    }
    qDebug() << "dir: " << dir;
    QDesktopServices::openUrl(QUrl(m_desFileDir, QUrl::TolerantMode));
}

void ExtractDataForm::basicTest() {
    //        QString dbhost = "192.168.211.162";
    //            // QString dbhost = "localhost";
//    //         QString databaseName = "MarketData_10m";
    //        QString databaseName = "MarketData_day";
    //        QString startDate = "20150130";
    //        QString endDate = "20180417";

    //        int dbConnectID = m_extractTimes;
    //        QStringList keyValueList;
    //        keyValueList << "TCLOSE";
    //        QString secodeFileName = QString("D:/excel/沪深300成分股 20170130~20180417 10min收盘价.xlsx");
    //        QString desDirName = QString("D:/excel/cplus/");

    //        ExtractMarketData* currExtractMarketDataObj = new ExtractMarketData(dbhost, dbConnectID, databaseName,
    //                                                                            startDate, endDate,
    //                                                                            secodeFileName, desDirName,
    //                                                                            keyValueList, m_extractThreadCount);

    //        connect(currExtractMarketDataObj, SIGNAL(extractDataComplete_signal()),
    //                this, SLOT(extractDataComplete_slot()));

    //        currExtractMarketDataObj->startReadData();

    //        m_extractMarketDataObjList.append(currExtractMarketDataObj);
}

QStringList ExtractDataForm::getCurrKeyValueList() {
    QStringList result;
    m_keyValueList = getComboBoxKeyList(m_marketKeyListWidget);
    for (QString key:m_keyValueList) {
        result.append(m_keyValueMap[key]);
    }
    return result;
}

QStringList ExtractDataForm::getCurrIndexCodeList() {
    QStringList result;
    m_indexCodeList = getComboBoxKeyList(m_weightIndexListWidget);
    for (QString key:m_indexCodeList) {
        result.append(m_indexCodeMap[key]);
    }
    return result;
}

QStringList ExtractDataForm::getCurrIndustryList() {
    QStringList result = getComboBoxKeyList(m_industryListWidget);
    if (result.indexOf(QString("全选")) >= 0) {
        result = m_industryList;
    }
    return result;
}

QStringList ExtractDataForm::getCurrFundamentKeyList() {
    QStringList result;
    m_fundamentKeyList = getComboBoxKeyList(m_fundamentKeyWidget);
    for (QString key:m_fundamentKeyList) {
        result.append(m_FundamentKeyMap[key]);
    }
    return result;
}

QStringList ExtractDataForm::checkSecodeList(QStringList oriSecodeList, QString dbhost, QString dbName) {
    QStringList result;
    Database databaseObj(dbhost);
    QList<QString> tableList = databaseObj.getTableList(dbName);
    QStringList errorCodeList;
    for (QString secode:result) {
        if (tableList.indexOf(completeSecode(secode)) < 0) {
            errorCodeList.append(secode);
            result.removeAll(secode);
        }
    }

    if (errorCodeList.size() > 0) {
        QString info = QString("存在错误股票代码: \n%1,\n是否放弃当前源股票代码信息文件, 否则忽略错误股票代码")
                .arg(errorCodeList.join("\n"));
        result.append("Error");
        result.append(info);
    }
    return result;
}

void ExtractDataForm::on_extractMarketData_pushButton_clicked()
{
    ++m_extractTimes;
    QString databaseName = m_dataTypeMap[ui->dataType_comboBox->currentText()];
    QString dbhost = ui->chooseDataSource_comboBox->currentText();
    QString startDate = ui->startDate_dateEdit->date().toString("yyyyMMdd");
    QString endDate = ui->endDate_dateEdit->date().toString("yyyyMMdd");
    QStringList keyValueList = getCurrKeyValueList();


    QStringList checkRst = checkSecodeList(m_secodeList, dbhost, databaseName);
    if (checkRst.size() > 0 &&  checkRst[0] == "Error")  {
        if (QMessageBox::No == QMessageBox::information(NULL, QString("确认信息"),
                                                        checkRst[1], QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }

    qDebug() << databaseName << dbhost << startDate << endDate << keyValueList;

    if (keyValueList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString(QString("还未选择指标")));
    } else {
        QString info = QString("开始提取行情数据: \n数据类型: %1,\n提取的指标: %2,\n时间区间: [%3, %4], \n")
                                .arg(ui->dataType_comboBox->currentText()).arg(m_keyValueList.join(","))
                                .arg(startDate).arg(endDate);

        if (QMessageBox::Yes == QMessageBox::information(NULL, QString("确认信息"),
                                                         info, QMessageBox::Yes | QMessageBox::No)) 
        {
            updateProgramInfo(ui->programInfo_Table, info);
            ExtractMarketData* currExtractMarketDataObj = new ExtractMarketData(dbhost, databaseName,
                                                                                startDate, endDate,
                                                                                m_secodeList, m_desFileDir,
                                                                                keyValueList, m_extractThreadCount);

            connect(currExtractMarketDataObj, SIGNAL(extractMarketDataComplete_signal(QStringList)),
                    this, SLOT(extractDataComplete_slot(QStringList)));

            currExtractMarketDataObj->startReadData();

            m_extractMarketDataObjList.append(currExtractMarketDataObj);
        }
    }
}

void ExtractDataForm::on_extractWeightData_pushButton_clicked()
{
    ++m_extractTimes;
    int dbConnectId = m_extractTimes * m_extractThreadCount;
    QString dbhost = ui->chooseDataSource_comboBox->currentText();
    QString startDate = ui->indexStartDate_dateEdit->date().toString("yyyyMMdd");
    QString endDate = ui->indexEndDate_dateEdit->date().toString("yyyyMMdd");
    QStringList currIndexCodeList = getCurrIndexCodeList();

    qDebug() << dbConnectId << dbhost << startDate << endDate << currIndexCodeList;

    if (currIndexCodeList.size() == 0) 
    {
        QMessageBox::critical(NULL, "Error", QString(QString("还未选择指数")));
    } 
    else 
    {
        QString info = QString("开始提取权重信息: \n选择的指数为: %1;\n指数截止日起始时间: [%2, %3];")
                .arg(m_indexCodeList.join(",")).arg(startDate).arg(endDate);
        if (QMessageBox::Yes == QMessageBox::information(NULL, QString("确认信息"),
                                                         info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, info);

            ExtractWeightData* currExtractWeightData = new ExtractWeightData(dbhost, startDate, endDate, m_weightTimeList,
                                                                            currIndexCodeList, m_desFileDir, m_extractThreadCount);
            connect(currExtractWeightData, SIGNAL(extractWeightDataComplete_signal(QStringList)),
                    this, SLOT(extractDataComplete_slot(QStringList)) );
            currExtractWeightData -> startReadData();

            m_extractWeightDataObjList.append(currExtractWeightData);
        }
    }
}

void ExtractDataForm::on_extractIndustryData_pushButton_clicked()
{
    ++m_extractTimes;
    int dbConnectId = m_extractTimes * m_extractThreadCount;
    QString dbhost = ui->chooseDataSource_comboBox->currentText();
    QString startDate = ui->industryStartDate_dateEdit->date().toString("yyyyMMdd");
    QString endDate = ui->industryEndDate_dateEdit->date().toString("yyyyMMdd");
    QStringList currIndustryList = getCurrIndustryList();
    qDebug() << dbConnectId << dbhost << startDate << endDate << currIndustryList;

    if (currIndustryList.size() == 0) {
        QMessageBox::critical(NULL, QString("错误信息"), QString("还未选择行业分类指标"));
    } else {
        QString info = QString("开始提取行业分类数据: \n选择的行业分类指标为: \n%1\n时间区间为: [%2, %3]")
                                                .arg(currIndustryList.join(",\n")).arg(startDate).arg(endDate);
        if (QMessageBox::Yes == QMessageBox::information(NULL, QString("确认信息"),
                                                        info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, info);

            ExtractIndustryData* currExtractIndustryData = new ExtractIndustryData(dbConnectId, dbhost,
                                                                                   startDate, endDate,
                                                                                   currIndustryList, m_desFileDir,
                                                                                   m_extractThreadCount);
            connect(currExtractIndustryData, SIGNAL(extractIndustryDataComplete_signal(QStringList)),
                    this, SLOT(extractDataComplete_slot(QStringList)) );
            currExtractIndustryData -> startReadData();

            m_extractIndustryDataObjList.append(currExtractIndustryData);
        }
    }
}

void ExtractDataForm::on_startExtractFundament_pushButton_clicked()
{
    QString databaseName = "FundamentData";
    QString dbhost = ui->chooseDataSource_comboBox->currentText();
    QString startDate = ui->fundamentStart_dateEdit->date().toString("yyyyMMdd");
    QString endDate = ui->fundamentEnd_dateEdit->date().toString("yyyyMMdd");
    QStringList keyValueList = getCurrFundamentKeyList();


    QStringList checkRst = checkSecodeList(m_fundamentKeyList, dbhost, databaseName);
    if (checkRst.size() > 0 &&  checkRst[0] == "Error")  {
        if (QMessageBox::No == QMessageBox::information(NULL, QString("确认信息"),
                                                        checkRst[1], QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }

    qDebug() << databaseName << dbhost << startDate << endDate << keyValueList;

    if (keyValueList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString(QString("还未选择指标")));
    } else {
        QString info = QString("开始提取基本面数据: \n提取的指标: %2,\n时间区间: [%3, %4], \n")
                                .arg(m_keyValueList.join(",")).arg(startDate).arg(endDate);

        if (QMessageBox::Yes == QMessageBox::information(NULL, QString("确认信息"),
                                                         info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, info);
            ExtractMarketData* currExtractMarketDataObj = new ExtractMarketData(dbhost, databaseName,
                                                                                startDate, endDate,
                                                                                m_secodeList, m_desFileDir,
                                                                                keyValueList, m_extractThreadCount);

            connect(currExtractMarketDataObj, SIGNAL(extractMarketDataComplete_signal(QStringList)),
                    this, SLOT(extractDataComplete_slot(QStringList)));

            currExtractMarketDataObj->startReadData();

            m_extractMarketDataObjList.append(currExtractMarketDataObj);
        }
    }
}

void ExtractDataForm::extractDataComplete_slot(QStringList desFileNameList) {
    updateProgramInfo(ui->programInfo_Table, QString("提取数据完毕, 提取后的数据存储文件为: \n %1")
                                             .arg(desFileNameList.join("\n")) );
    setDesFileTable();
}

void ExtractDataForm::on_chooseOriFileDir_pushButton_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");
    if (file_name == "") return;
    m_oriFileDir = QString("%1/").arg(file_name);
    writeFileInfo(m_nativeFileName, "A1", m_oriFileDir);
    setOriFileTable();
}

void ExtractDataForm::on_chooseDesFileDir_pushButton_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");
    if (file_name == "") return;
    m_desFileDir = QString("%1/").arg(file_name);
    writeFileInfo(m_nativeFileName, "A2", m_desFileDir);
    setDesFileTable();
}

void ExtractDataForm::on_oriFile_tableView_clicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->oriFile_tableView->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_oriFileDir + fileName;
    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        m_currOriFile = cmpFileName;
    //        qDebug() << "m_currOriFile: " << m_currOriFile;
        m_secodeList = readExcelSecodeList(m_currOriFile, "Sheet1", 1, "ori");
    } else {
        QMessageBox::critical(NULL, "Error", QString("当前文件不存在,请刷新当前表格"));
    }
}

void ExtractDataForm::on_oriFile_tableView_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->oriFile_tableView->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_oriFileDir + fileName;
    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
    } else {
        QMessageBox::critical(NULL, "Error", QString("当前文件不存在,请刷新当前表格"));
    }
}

void ExtractDataForm::on_desFile_tableView_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->desFile_tableView->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_desFileDir + fileName;
    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
    } else {
        QMessageBox::critical(NULL, "Error", QString("当前文件不存在,请刷新当前表格"));
    }
}

void ExtractDataForm::checkAllIndustry() {
    qDebug() << "checkAllIndustry";
    m_setIndustryCombox = true;
    for (int i = 0; i < m_industryListWidget->count(); ++i) {
        QListWidgetItem *pItem = m_industryListWidget->item(i);
        QWidget *pWidget = m_industryListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        pCheckBox->setChecked(true);
    }
    m_setIndustryCombox = false;
}

void ExtractDataForm::industryComboxStateChange(int state) {
    if (m_setIndustryCombox) return;
    for (int i = 0; i < m_industryListWidget->count(); ++i) {
        QListWidgetItem *pItem = m_industryListWidget->item(i);
        QWidget *pWidget = m_industryListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {

            if (pCheckBox->text() == QString("全选")) {
                checkAllIndustry();
            }
        }
    }
    state;
}

void ExtractDataForm::on_chooseSecodeListFromExcel_pushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(NULL, QString("选择导入的EXCEL, 文档第一列为股票代码"),
                                                          ".", QString("Excel Files(*.xlsx)"));
    qDebug() << "file_name: " <<file_name;
    if (file_name != "") {
        m_currOriFile = file_name;
        updateProgramInfo(ui->programInfo_Table, QString("选择了股票代码列表文件: %1").arg(m_currOriFile));
        m_secodeList = readExcelSecodeList(m_currOriFile, "Sheet1", 1, "ori");
    }
}

void ExtractDataForm::on_chooseSecodeListFromTable_pushButton_clicked()
{
    QString dbhost = ui->chooseDataSource_comboBox->currentText();
    ChooseSecodeWindow* chooseSecodeWindow = new ChooseSecodeWindow(dbhost);
    connect(chooseSecodeWindow, SIGNAL(get_secodeList_signal(QStringList)),
            this, SLOT(get_secodeList_slot(QStringList)));
    chooseSecodeWindow->show();
    m_chooseSecodeWindowList.append(chooseSecodeWindow);
}

void ExtractDataForm::get_secodeList_slot(QStringList marketSecodeList) {
    m_secodeList = marketSecodeList;
}

void ExtractDataForm::on_chooseSecodeList_pushButton_clicked()
{
    QString dbhost = ui->chooseDataSource_comboBox->currentText();
    ChooseSecodeWindow* chooseSecodeWindow = new ChooseSecodeWindow(dbhost,false);
    connect(chooseSecodeWindow, SIGNAL(get_secodeList_signal(QStringList)),
            this, SLOT(get_secodeList_slot(QStringList)));
    chooseSecodeWindow->show();
    m_chooseSecodeWindowList.append(chooseSecodeWindow);
}

void ExtractDataForm::on_importSecodeList_pushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(NULL, QString("选择导入的EXCEL, 文档第一列为股票代码"),
                                                          ".", QString("Excel Files(*.xlsx)"));
    qDebug() << "file_name: " <<file_name;
    if (file_name != "") {
        m_currOriFile = file_name;
        updateProgramInfo(ui->programInfo_Table, QString("选择了股票代码列表文件: %1").arg(m_currOriFile));
        m_secodeList = readExcelSecodeList(m_currOriFile, "Sheet1", 1, "ori");
    }
}


void ExtractDataForm::on_extractSecodeTimeList_pushButton_clicked()
{
    m_marketTimeList = extractTimeList();
}

void ExtractDataForm::on_extractWeightTimeList_pushButton_clicked()
{
    m_weightTimeList = extractTimeList();
}

void ExtractDataForm::on_extractIndustryTimeList_pushButton_clicked()
{
    m_industryTimeList = extractTimeList();
}

void ExtractDataForm::on_extractFundamentTimeList_pushButton_clicked()
{
    m_fundamentTimeList = extractTimeList();
}
