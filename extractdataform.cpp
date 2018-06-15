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
#include "excel_func.h"
#include "widget_func.h"
#include "toolfunc.h"
#include "choosesecodewindow.h"
//#pragma execution_character_set("utf-8")

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

    if (NULL != m_chooseSecodeWindow) {
        delete m_chooseSecodeWindow;
        m_chooseSecodeWindow = NULL;
    }
}

void ExtractDataForm::initCommonData() {
    m_currOriFile = "";
    m_extractTimes = 0;
    m_keyValueList.clear();
    m_extractThreadCount = 16;
    m_chooseSecodeWindow = NULL;

    initDataTypeMap();
    initKeyValueMap();
    initIndexCodeMap();
    initIndustryList();

    initFileDir();
}

void ExtractDataForm::initFileDir() {
    m_nativeFileName = "./ExtractDataDir.xlsx";
    QFile tmpFile(m_nativeFileName);
    if (!tmpFile.exists()) {
        qDebug() << QString("%1 does not exit!").arg(m_nativeFileName);
        QXlsx::Document xlsx;
        m_oriFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/提取数据/");
        m_desFileDir = m_oriFileDir;
        xlsx.write("A1", m_oriFileDir);
        xlsx.write("A2", m_desFileDir);
        xlsx.saveAs(m_nativeFileName);
    } else {
        QXlsx::Document xlsx(m_nativeFileName);
        xlsx.selectSheet("1");
        QXlsx::CellRange range = xlsx.dimension();
        if (range.rowCount() == 0) {
            m_oriFileDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/提取数据/");
            m_desFileDir = m_oriFileDir;
            xlsx.write("A1", m_oriFileDir);
            xlsx.write("A2", m_desFileDir);
            xlsx.save();
        } else {
            m_oriFileDir = xlsx.cellAt(1, 1)->value().toString();
            m_desFileDir = xlsx.cellAt(2, 1)->value().toString();
        }
    }
    qDebug() << "m_oriFileDir: " << m_oriFileDir;
    qDebug() << "m_desFileDir: " << m_desFileDir;
}

void ExtractDataForm::initDataTypeMap() {
    m_dataTypeMap.insert(QString::fromLocal8Bit("天"), "MarketData_day");
    m_dataTypeMap.insert(QString::fromLocal8Bit("10分"), "MarketData_10m");
}

void ExtractDataForm::initKeyValueMap() {
    m_keyValueMap.insert(QString::fromLocal8Bit("开盘价"), "TOPEN");
    m_keyValueMap.insert(QString::fromLocal8Bit("收盘价"), "TCLOSE");
    m_keyValueMap.insert(QString::fromLocal8Bit("最高价"), "HIGH");
    m_keyValueMap.insert(QString::fromLocal8Bit("最低价"), "LOW");
    m_keyValueMap.insert(QString::fromLocal8Bit("成交量"), "VATRUNOVER");
    m_keyValueMap.insert(QString::fromLocal8Bit("成交额"), "VOTRUNOVER");
    m_keyValueMap.insert(QString::fromLocal8Bit("昨收"), "YCLOSE");
    m_keyValueMap.insert(QString::fromLocal8Bit("换手率"), "TURNOVER");
}

void ExtractDataForm::initIndexCodeMap() {
    m_indexCodeMap.insert(QString::fromLocal8Bit("上证50"), "SH000016");
    m_indexCodeMap.insert(QString::fromLocal8Bit("沪深300"), "SH000300");
    m_indexCodeMap.insert(QString::fromLocal8Bit("中证1000"), "SH000852");
    m_indexCodeMap.insert(QString::fromLocal8Bit("中证200"), "SH000904");
    m_indexCodeMap.insert(QString::fromLocal8Bit("中证500"), "SH000905");
    m_indexCodeMap.insert(QString::fromLocal8Bit("中证800"), "SH000906");
    m_indexCodeMap.insert(QString::fromLocal8Bit("中证100"), "SZ399903");
}

void ExtractDataForm::initIndustryList() {
    m_industryList.append(QString::fromLocal8Bit("中证一级行业"));
    m_industryList.append(QString::fromLocal8Bit("中证二级行业"));
    m_industryList.append(QString::fromLocal8Bit("中证三级行业"));
    m_industryList.append(QString::fromLocal8Bit("申万一级行业"));
    m_industryList.append(QString::fromLocal8Bit("申万二级行业"));
    m_industryList.append(QString::fromLocal8Bit("申万三级行业"));
    m_industryList.append(QString::fromLocal8Bit("万得一级行业"));
    m_industryList.append(QString::fromLocal8Bit("万得二级行业"));
    m_industryList.append(QString::fromLocal8Bit("万得三级行业"));
    m_industryList.append(QString::fromLocal8Bit("万得四级行业"));
    m_industryList.append(QString::fromLocal8Bit("全选"));
}

void ExtractDataForm::initWidget() {
    initDate();
    initDataSourceCombox();
    initDataTypeCombox();

    initMarketKeyComboBox();
    initIndexCodeComboBox();
    initIndustryComboBox();

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
        m_marketKeyListWidget->addItem(pItem);
        pItem->setData(Qt::UserRole, it.key());
        QCheckBox *pCheckBox = new QCheckBox(this);
        pCheckBox->setText(it.key());
        m_marketKeyListWidget->addItem(pItem);
        m_marketKeyListWidget->setItemWidget(pItem, pCheckBox);
        connect(pCheckBox, SIGNAL(stateChanged(int)),
                this,     SLOT(stateChanged(int)));
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
        connect(pCheckBox, SIGNAL(stateChanged(int)),
                this,     SLOT(industryComboxStateChange(int)));
    }
    ui->industry_comboBox->setModel(m_industryListWidget->model());
    ui->industry_comboBox->setView(m_industryListWidget);
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

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("时间")));
    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("信息")));
    standardItemModel-> setHorizontalHeaderItem (2, new QStandardItem(QString::fromLocal8Bit("备注")));
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

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("源信息文件")));
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

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("生成文件")));
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

void ExtractDataForm::show_oriFileTable_contextMenu(QPoint pos) {
    QMenu *menu = new QMenu(ui->oriFile_tableView);
    QAction *refreshTable = new QAction(QString::fromLocal8Bit("刷新"),ui->oriFile_tableView);
    QAction *deleteFile = new QAction(QString::fromLocal8Bit("删除"),ui->oriFile_tableView);
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
    QAction *refreshTable = new QAction(QString::fromLocal8Bit("刷新"),ui->desFile_tableView);
    QAction *deleteFile = new QAction(QString::fromLocal8Bit("删除"),ui->desFile_tableView);
    QAction *openDir = new QAction(QString::fromLocal8Bit("打开所在文件夹"),ui->desFile_tableView);

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
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
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
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
            }
        }
    }
    setDesFileTable();
}

void ExtractDataForm::open_oriDir() {
    qDebug() <<"m_oriFileDir: " << m_oriFileDir ;
    QString dir = QString::fromLocal8Bit(m_oriFileDir.toStdString().c_str());
    if (m_oriFileDir.startsWith("//")) {
        dir = QString::fromLocal8Bit("file:%1").arg(QString::fromLocal8Bit(m_oriFileDir.toStdString().c_str()));
    }
    dir = QString("D:/课程/");
    qDebug() << "dir: " << dir;
    QDesktopServices::openUrl(QUrl(dir, QUrl::TolerantMode));
}

void ExtractDataForm::open_desDir() {
    qDebug() <<"m_desFileDir: " << m_desFileDir ;
    QString dir = QString::fromLocal8Bit(m_desFileDir.toStdString().c_str());
    if (m_desFileDir.startsWith("//")) {
        dir = QString::fromLocal8Bit("file:%1").arg(QString::fromLocal8Bit(m_desFileDir.toStdString().c_str()));
    }
    qDebug() << "dir: " << dir;
    QDesktopServices::openUrl(QUrl(m_desFileDir, QUrl::TolerantMode));
}

void ExtractDataForm::basicTest() {
        QString dbhost = "192.168.211.162";
        // QString dbhost = "localhost";
//         QString databaseName = "MarketData_10m";
        QString databaseName = "MarketData_day";
        QString startDate = "20150130";
        QString endDate = "20180417";

        int dbConnectID = m_extractTimes;
        QStringList keyValueList;
        keyValueList << "TCLOSE";
        QString secodeFileName = QString::fromLocal8Bit("D:/excel/沪深300成分股 20170130~20180417 10min收盘价.xlsx");
        QString desDirName = QString::fromLocal8Bit("D:/excel/cplus/");

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
    m_keyValueList.clear();
    for (int i = 0; i < m_marketKeyListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_marketKeyListWidget->item(i);
        QWidget *pWidget = m_marketKeyListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            result.append(m_keyValueMap[strText]);
            m_keyValueList.append(strText);
        }
    }
    return result;
}

QStringList ExtractDataForm::getSecodeList(QString excelFileName, QString dbhost, QString dbName) {
    QStringList result = readExcelSecodeList(excelFileName, "Sheet1", 1, "tinysoft");
    Database databaseObj("0", dbhost);
    QList<QString> tableList = databaseObj.getTableList(dbName);
    QStringList errorCodeList;
    for (QString secode:result) {
        if (tableList.indexOf(secode) < 0) {
            errorCodeList.append(secode);
            result.removeAll(secode);
        }
    }
    if (errorCodeList.size() > 0) {
        QString info = QString::fromLocal8Bit("存在错误股票代码: \n%1,\n是否放弃当前源股票代码信息文件, 否则忽略错误股票代码")
                .arg(errorCodeList.join("\n"));
        if (QMessageBox::Yes == QMessageBox::information(NULL, QString::fromLocal8Bit("确认信息"),
                                                         info, QMessageBox::Yes | QMessageBox::No)) {
            result.insert(0, "Error");
        }
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
    int dbConnectId = m_extractTimes * m_extractThreadCount;
    QStringList keyValueList = getCurrKeyValueList();
    QStringList secodeList = getSecodeList(m_currOriFile, dbhost, databaseName);

    if (secodeList.size() > 0 &&  secodeList[0] == "Error") return;

    qDebug() << databaseName << dbhost << startDate << endDate << keyValueList;

    if (m_currOriFile.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString(QString::fromLocal8Bit("还未选择源文件")));
    } else if (keyValueList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString(QString::fromLocal8Bit("还未选择指标")));
    } else {
        QString info = QString::fromLocal8Bit("开始提取行情数据: \n数据类型: %1,\n提取的指标: %2,\n时间区间: [%3, %4], \n")
                                            .arg(ui->dataType_comboBox->currentText()).arg(m_keyValueList.join(","))
                                            .arg(startDate).arg(endDate);

        if (QMessageBox::Yes == QMessageBox::information(NULL, QString::fromLocal8Bit("确认信息"),
                                                         info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, info);
            ExtractMarketData* currExtractMarketDataObj = new ExtractMarketData(dbhost, dbConnectId, databaseName,
                                                                                startDate, endDate,
                                                                                secodeList, m_desFileDir,
                                                                                keyValueList, m_extractThreadCount);

            connect(currExtractMarketDataObj, SIGNAL(extractMarketDataComplete_signal(QStringList)),
                    this, SLOT(extractDataComplete_slot(QStringList)));

            currExtractMarketDataObj->startReadData();

            m_extractMarketDataObjList.append(currExtractMarketDataObj);
        }
    }
}

QStringList ExtractDataForm::getCurrIndexCodeList() {
    QStringList result;
    m_indexCodeList.clear();
    for (int i = 0; i < m_weightIndexListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_weightIndexListWidget->item(i);
        QWidget *pWidget = m_weightIndexListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            result.append(m_indexCodeMap[strText]);
            m_indexCodeList.append(strText);
        }
    }
    return result;
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

    if (currIndexCodeList.size() == 0) {
        QMessageBox::critical(NULL, "Error", QString(QString::fromLocal8Bit("还未选择指数")));
    } else {
        QString info = QString::fromLocal8Bit("开始提取权重信息: \n选择的指数为: %1;\n指数截止日起始时间: [%2, %3];")
                .arg(m_indexCodeList.join(",")).arg(startDate).arg(endDate);
        if (QMessageBox::Yes == QMessageBox::information(NULL, QString::fromLocal8Bit("确认信息"),
                                                         info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, info);

            ExtractWeightData* currExtractWeightData = new ExtractWeightData(dbhost, dbConnectId,
                                                                             startDate, endDate, currIndexCodeList,
                                                                             m_desFileDir, m_extractThreadCount);
            connect(currExtractWeightData, SIGNAL(extractWeightDataComplete_signal(QStringList)),
                    this, SLOT(extractDataComplete_slot(QStringList)) );
            currExtractWeightData -> startReadData();

            m_extractWeightDataObjList.append(currExtractWeightData);
        }
    }
}

QStringList ExtractDataForm::getCurrIndustryList() {
    QStringList result;
    for (int i = 0; i < m_industryListWidget->count(); ++i)
    {
        QListWidgetItem *pItem = m_industryListWidget->item(i);
        QWidget *pWidget = m_industryListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            if (strText == QString::fromLocal8Bit("全选")) {
                result = m_industryList;
                qDebug() << "Choose All";
                break;
            } else {
                result.append(strText);
            }
        }
    }
    result.removeAll(QString::fromLocal8Bit("全选"));
    qDebug() << "result: " << result;
    return result;
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
        QMessageBox::critical(NULL, QString::fromLocal8Bit("错误信息"), QString::fromLocal8Bit("还未选择行业分类指标"));
    } else {
        QString info = QString::fromLocal8Bit("开始提取行业分类数据: \n选择的行业分类指标为: \n%1\n时间区间为: [%2, %3]")
                                                .arg(currIndustryList.join(",\n")).arg(startDate).arg(endDate);
        if (QMessageBox::Yes == QMessageBox::information(NULL, QString::fromLocal8Bit("确认信息"),
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

void ExtractDataForm::extractDataComplete_slot(QStringList desFileNameList) {
    updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("提取数据完毕, 提取后的数据存储文件为: \n %1")
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
        qDebug() << "m_currOriFile: " << m_currOriFile;
    } else {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件不存在,请刷新当前表格"));
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
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件不存在,请刷新当前表格"));
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
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件不存在,请刷新当前表格"));
    }
}

void ExtractDataForm::stateChanged(int state) {
    QString strSelectedData("");
    int nCount = m_marketKeyListWidget->count();
    for (int i = 0; i < nCount; ++i)
    {
        QListWidgetItem *pItem = m_marketKeyListWidget->item(i);
        QWidget *pWidget = m_marketKeyListWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            strSelectedData.append(strText).append(";");
            qDebug() << "strText: " << strText;
        }
    }
    state;
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

            if (pCheckBox->text() == QString::fromLocal8Bit("全选")) {
                checkAllIndustry();
            }
        }
    }
    state;
}

void ExtractDataForm::on_chooseSecodeListFromExcel_pushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(NULL, QString::fromLocal8Bit("选择导入的EXCEL, 文档第一列为股票代码"),
                                                          ".", QString("Excel Files(*.xlsx)"));
    qDebug() << "file_name: " <<file_name;
    if (file_name != "") {
        m_currOriFile = file_name;
        updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("选择了股票代码列表文件: %1").arg(m_currOriFile));
    }
}

void ExtractDataForm::on_chooseSecodeListFromTable_pushButton_clicked()
{
    m_chooseSecodeWindow = new ChooseSecodeWindow();
    m_chooseSecodeWindow->show();
}
