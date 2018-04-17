#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDateTime>
#include <QDate>
#include <QScrollBar>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QFile>
#include <QAction>
#include <QTableWidgetItem>
#include <QDir>
#include "generateportfolioform.h"
#include "ui_generateportfolioform.h"
#include "toolfunc.h"
#include "xlsxdocument.h"

GeneratePortfolioForm::GeneratePortfolioForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneratePortfolioForm)
{
    ui->setupUi(this);

    initCommnData();

    initWidget();
}

GeneratePortfolioForm::~GeneratePortfolioForm()
{
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
    delete ui;
}

void GeneratePortfolioForm::initCommnData() {
    m_database = NULL;
    m_dbConnID = "1";
    m_priceTime = "realtime";
    m_indexCode = "000300";
    m_hasChooseAccount = false;

    m_dbhostList << "192.168.211.162"
                 << "192.168.211.165";

    initFileDir();
    initExcelFileInfo();
}

void GeneratePortfolioForm::setDatabase() {
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
    m_dbhost = ui->datasource_comboBox->currentText();
    m_database = new Database(m_dbConnID, m_dbhost);
    updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("当前链接的数据源是: %1").arg(m_dbhost));
}

void GeneratePortfolioForm::initFileDir() {
    m_nativeFileName = "./dirInfo.xlsx";
    QFile tmpFile(m_nativeFileName);
    if (!tmpFile.exists()) {
        qDebug() << QString("%1 does not exit!").arg(m_nativeFileName);
        QXlsx::Document xlsx;
        m_mainDir = "//192.168.211.182/";
        m_accountFatherDir = m_mainDir + QString::fromLocal8Bit("it程序设计/生成组合/");
        m_accountCurrDir = m_accountFatherDir + QString::fromLocal8Bit("账户1/");
        m_strategyDir = m_accountFatherDir + QString::fromLocal8Bit("Z策略/");
        m_accountPortfolioDir = m_accountCurrDir + QString::fromLocal8Bit("买卖组合/");
        xlsx.write("A1", m_accountCurrDir);
        xlsx.write("A2", m_strategyDir);
        xlsx.write("A3", m_accountPortfolioDir);
        xlsx.saveAs(m_nativeFileName);
    } else {
        QXlsx::Document xlsx(m_nativeFileName);
        xlsx.selectSheet("1");
        QXlsx::CellRange range = xlsx.dimension();
        if (range.rowCount() == 0) {
            m_mainDir = "//192.168.211.182/";
            m_accountFatherDir = m_mainDir + QString::fromLocal8Bit("it程序设计/生成组合/");
            m_accountCurrDir = m_accountFatherDir + QString::fromLocal8Bit("账户1/");
            m_strategyDir = m_accountFatherDir + QString::fromLocal8Bit("Z策略/");
            m_accountPortfolioDir = m_accountCurrDir + QString::fromLocal8Bit("买卖组合/");
            xlsx.write("A1", m_accountCurrDir);
            xlsx.write("A2", m_strategyDir);
            xlsx.write("A3", m_accountPortfolioDir);
            xlsx.save();
        } else {
            qDebug() << QString("%1 exits!").arg(m_nativeFileName);
            m_accountCurrDir = xlsx.cellAt(1, 1)->value().toString();
            m_strategyDir = xlsx.cellAt(2, 1)->value().toString();
            m_accountPortfolioDir = xlsx.cellAt(3, 1)->value().toString();

        }
    }
    qDebug() << "m_accountCurrDir: " << m_accountCurrDir;
    qDebug() << "m_strategyDir: " << m_strategyDir;
    qDebug() << "m_accountPortfolioDir: " << m_accountPortfolioDir;
}

void GeneratePortfolioForm::initExcelFileInfo() {
    m_strategyExcelFileColNumb = 2;
    m_accoutnExcelFileColNumbList << 2 << 51;
    QList<int> account1ExcelFileUserfulNumb;
    account1ExcelFileUserfulNumb << 0 << 1;
    QList<int> account2ExcelFileUserfulNumb;
    account2ExcelFileUserfulNumb << 2 << 11;
    m_accountExcelFileUsefulColNumbList << account1ExcelFileUserfulNumb << account2ExcelFileUserfulNumb;
}

void GeneratePortfolioForm::initWidget() {
    ui->chooseRealTimeRatio->setChecked(true);

    initChoosePriceDate();
    initProgramInfoTable();
    initDatasourceCombox();

    setAccountTable();
    setStrategyTable();
    setBuySalePortfolioTable();

    initTableContextMenu();
}

void GeneratePortfolioForm::initDatasourceCombox() {
    ui->datasource_comboBox->addItems(m_dbhostList);
    ui->datasource_comboBox->setCurrentIndex(0);
}

void GeneratePortfolioForm::initChoosePriceDate() {
    ui->chooseHistDate->setCalendarPopup(true);
    ui->chooseRealTimeRatio->setChecked(true);
}

void GeneratePortfolioForm::initProgramInfoTable() {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QString::fromLocal8Bit("备注")));
    ui->programInfo_Table->setModel (initModel);
    ui->programInfo_Table->setColumnWidth (0, 250);
    ui->programInfo_Table->setColumnWidth (1, 600);
    ui->programInfo_Table->setColumnWidth (2, 150);
    ui->programInfo_Table->setShowGrid (false);
}

void GeneratePortfolioForm::setAccountTable() {
    m_currAccountFileList = getExcelFileInfo(m_accountCurrDir);
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->account_table->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
//        qDebug() << "Init account_table";
    } else {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_currAccountFileList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(m_currAccountFileList[i].fileName()));
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("持仓信息")));
    ui->account_table->setModel(standardItemModel);
    ui->account_table->setColumnWidth (0, 300);
    ui->account_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->account_table->setShowGrid (false);
}

void GeneratePortfolioForm::setStrategyTable() {
    m_strategyFileList = getExcelFileInfo(m_strategyDir);

    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
//        qDebug() << "Init strategy_table";
    } else {
        standardItemModel->clear();
    }

    for (int i = 0; i < m_strategyFileList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(m_strategyFileList[i].fileName()));
        standardItemModel->setItem(i, 1, new QStandardItem(QString("%1").arg(0.0)));
    }

    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("策略")));
    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("沪深300张数")));
    ui->strategy_table->setModel(standardItemModel);
    ui->strategy_table->setColumnWidth (0, 200);
    ui->strategy_table->setColumnWidth (1, 100);
    ui->strategy_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->strategy_table->setShowGrid (false);
    ui->strategy_table->setEditTriggers(QAbstractItemView::DoubleClicked);
}

void GeneratePortfolioForm::setBuySalePortfolioTable() {
    m_buySalePortfolioFileList = getExcelFileInfo(m_accountPortfolioDir);
    m_portfolioAmount = getPortfolioAmountMap(m_buySalePortfolioFileList);
//    printMap(m_portfolioAmount, "m_portfolioAmount: ");

    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    if (NULL == standardItemModel) {
        standardItemModel = new QStandardItemModel();
//        qDebug() << "Init buySalePortfolio_table";
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
    ui->buySalePortfolio_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void GeneratePortfolioForm::initTableContextMenu() {
    ui->account_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->strategy_table->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->buySalePortfolio_table->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->account_table,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_accountTable_contextMenu(QPoint)));

    connect(ui->strategy_table,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_strategyTable_contextMenu(QPoint)));

    connect(ui->buySalePortfolio_table,SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(show_portfolioTable_contextMenu(QPoint)));
}

void GeneratePortfolioForm::show_accountTable_contextMenu(QPoint pos) {
    //设置菜单选项
    QMenu *menu = new QMenu(ui->account_table);
    QAction *refreshTable = new QAction(QString::fromLocal8Bit("刷新"),ui->account_table);
    QAction *deleteFile = new QAction(QString::fromLocal8Bit("删除"),ui->account_table);

    connect (refreshTable,SIGNAL(triggered()),this,SLOT(refresh_account_table()));
    connect (deleteFile, SIGNAL(triggered()),this,SLOT(delete_account_file()));

    menu->addAction(refreshTable);
    menu->addAction(deleteFile);
    menu->move (cursor ().pos ());
    menu->show ();

    pos;
}

void GeneratePortfolioForm::show_strategyTable_contextMenu(QPoint pos) {
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

void GeneratePortfolioForm::show_portfolioTable_contextMenu(QPoint pos) {
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

void GeneratePortfolioForm::refresh_account_table() {
    setAccountTable();
}

void GeneratePortfolioForm::refresh_strategy_table() {
    setStrategyTable();
}

void GeneratePortfolioForm::refresh_portfolio_table() {
    setBuySalePortfolioTable();
}

void GeneratePortfolioForm::delete_account_file() {
    QMap<int, int> selectMap = getSelectRows(ui->account_table);
//    qDebug() << "selectMap: " << selectMap;
    QList<QString> selectData = getSelectTableData(ui->account_table, selectMap);
//    qDebug() << "selectData: " << selectData;

    for (QString stringData : selectData) {
        QString fileName = m_accountCurrDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
            }
        }
    }
    setAccountTable();
}

void GeneratePortfolioForm::delete_strategy_file() {
    QMap<int, int> selectMap = getSelectRows(ui->strategy_table);
//    qDebug() << "selectMap: " << selectMap;
    QList<QString> selectData = getSelectTableData(ui->strategy_table, selectMap);
//    qDebug() << "selectData: " << selectData;

    for (QString stringData : selectData) {
        QString fileName = m_strategyDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
            }
        }
    }
    setStrategyTable();
}

void GeneratePortfolioForm::delete_portfolio_file() {
    QMap<int, int> selectMap = getSelectRows(ui->buySalePortfolio_table);
//    qDebug() << "selectMap: " << selectMap;
    QList<QString> selectData = getSelectTableData(ui->buySalePortfolio_table, selectMap);
//    qDebug() << "selectData: " << selectData;

    for (QString stringData : selectData) {
        QString fileName = m_accountPortfolioDir + stringData;
        QFile tmpFile(fileName);
        if (tmpFile.exists()) {
            if (tmpFile.remove() ) {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("成功删除 %1").arg(stringData));
            } else {
                updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("删除 %1 失败").arg(stringData));
            }
        }
    }
    setBuySalePortfolioTable();
}

void GeneratePortfolioForm::on_account_table_clicked(const QModelIndex &index)
{
    m_currAccountMap.clear();
    m_hasChooseAccount = true;
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->account_table->model ());
    m_currAccountFileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_accountCurrDir + m_currAccountFileName;

    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QList<QStringList> oriAccountData = readExcelData(cmpFileName);

        qDebug() << "oriAccountData: "  << oriAccountData;

        for (int i = 0; i < oriAccountData.size(); ++i) {
            if (oriAccountData[i].size() < 2) {
                QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("读取失败: 账户持仓组合: %1, 内容有错误").arg(m_currAccountFileName));
                m_currAccountMap.clear();
                return;
            }
            QString secodeName = oriAccountData[i][0];
            int secodeCount = oriAccountData[i][1].toInt();
            if (isSecodeValid(secodeName)) {
                m_currAccountMap.insert(getCompleteSecode(secodeName, "ori"), secodeCount);
            }
        }
        updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("读取成功: 账户持仓组合 %1, ").arg(m_currAccountFileName));
    }

}

void GeneratePortfolioForm::on_chooseHistDate_editingFinished()
{
    ui->chooseRealTimeRatio->setChecked(false);
    m_priceTime = ui->chooseHistDate->date().toString("yyyyMMdd");
}

void GeneratePortfolioForm::on_chooseRealTimeRatio_clicked(bool checked)
{
    if (checked) {
        QDate defaultDate(2000, 1, 1);
        ui->chooseHistDate->setDate(defaultDate);
        m_priceTime = "realtime";
    }
}

void GeneratePortfolioForm::on_account_table_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->account_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_accountCurrDir + fileName;
//    qDebug() << "cmpFileName: " << cmpFileName;
    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
    } else {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件不存在,请刷新当前表格"));
    }
}

void GeneratePortfolioForm::on_strategy_table_doubleClicked(const QModelIndex &index)
{
    if (index.column() == 0) {
        QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
        QString fileName = standardItemModel->item(index.row(), index.column())->text();
        QString cmpFileName = m_strategyDir + fileName;
//        qDebug() << "cmpFileName: " << cmpFileName;
        QFile tmpFile(cmpFileName);
        if (tmpFile.exists()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
        } else {
            QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件不存在,请刷新当前表格"));
        }
    }
}

void GeneratePortfolioForm::on_buySalePortfolio_table_doubleClicked(const QModelIndex &index)
{
    QStandardItemModel* standardItemModel = dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    QString fileName = standardItemModel->item(index.row(), index.column())->text();
    QString cmpFileName = m_accountPortfolioDir + fileName;
//    qDebug() << "cmpFileName: " << cmpFileName;
    QFile tmpFile(cmpFileName);
    if (tmpFile.exists()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(cmpFileName));
    } else {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("当前文件不存在,请刷新当前表格"));
    }
}

void GeneratePortfolioForm::on_geneBuySalePortfolio_clicked()
{
    m_strategyWeight = getStrategyWeight();
    if (!isStrategyWeightValid()) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("沪深300张数错误"));
        return;
    }

    printMap(m_strategyWeight, "m_strategyWeight");

    if (false == m_hasChooseAccount) {
        QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("还未选择账户"));
        return;
    } else {
        QString strategyWeightInfo = "";
        for (QMap<QString, double>::iterator it = m_strategyWeight.begin();
             it != m_strategyWeight.end(); ++it) {
            strategyWeightInfo += it.key().section('.', 0, 0) + ": " + QString("%1").arg(it.value()) + "\n";
        }
        QString info = QString::fromLocal8Bit("选择的持仓组合为: ") + m_currAccountFileName + "\n"
                     + QString::fromLocal8Bit("设定的策略对应HS300张数: \n") + strategyWeightInfo
                     + QString::fromLocal8Bit("收盘价时间为: ") + m_priceTime;

        if (QMessageBox::Yes == QMessageBox::information(NULL, QString::fromLocal8Bit("确认信息"), info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("----- 生成买卖组合信息 -----"));
            setDatabase();

            m_allStrategy = getCurrStrategy(m_strategyWeight);
            if (m_allStrategy.find("Error") != m_allStrategy.end()) {
                return;
            }

            m_currSecodeList = getCurrSecodeList(m_allStrategy);
            m_currSecodeClosePrice = getClosePrice(m_currSecodeList, m_indexCode, m_priceTime);

            if (!isClosePriceValid(m_currSecodeClosePrice)) {
                QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("重新选择时间"));
                return;
            }

            m_currNewPortfolio.clear();
            for (QMap<QString, double>::iterator it = m_strategyWeight.begin();
                 it != m_strategyWeight.end(); ++it) {
                double currMarketCapitalization = m_currSecodeClosePrice[m_indexCode] * it.value() * 300;
                QMap<QString, double> currPortfolio = getNewPortfolio(m_allStrategy[it.key()], m_currSecodeClosePrice,
                                                                  currMarketCapitalization);
                m_currNewPortfolio = unitePortfolio(m_currNewPortfolio, currPortfolio);
            }
            m_currNewPortfolio = intPorfolio(m_currNewPortfolio);

            QList<QMap<QString, double>> newBuySale = getBuySalePortfolio();
            m_currBuyPortfolio = newBuySale[0];
            m_currSalePortfolio = newBuySale[1];

            storeBuySalePortfolio();

            QStringList infoList = info.split("\n");
            for (int i = 0; i < infoList.size(); ++i) {
                updateProgramInfo(ui->programInfo_Table, infoList[i]);
            }
            updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("----- 生成买卖组合结束 -----"));
            m_hasChooseAccount = false;
        }
    }
}

QMap<QString, double> GeneratePortfolioForm::getClosePrice(QList<QString> secodeList, QString indexCode, QString timepType) {
    QMap<QString, double> result;
    for (QString secode : secodeList) {
        double closePrice = m_database->getClosePrice(secode, timepType);
        result.insert(secode, closePrice);
    }

    QString transIndexCode;
    if (timepType == "realtime") {
        transIndexCode = getCompleteIndexCode(indexCode, "wind");
    } else {
        transIndexCode = getCompleteIndexCode(indexCode, "tinysoft");
    }
    double closePrice = m_database->getClosePrice(transIndexCode, timepType);
    result.insert(indexCode, closePrice);
    return result;
}

QMap<QString, double> GeneratePortfolioForm::getClosePrice() {
    QMap<QString, double> result;
    for (QMap<QString, double>::iterator it = m_currStrategyMap.begin();
         it != m_currStrategyMap.end(); ++it) {
        QString oriSecode = it.key();
        double closePrice = m_database->getClosePrice(oriSecode, m_priceTime);
        result.insert(oriSecode, closePrice);
    }

    QString indexCode;
    if (m_priceTime == "realtime") {
        indexCode = getCompleteIndexCode(m_indexCode, "wind");
    } else {
        indexCode = getCompleteIndexCode(m_indexCode, "tinysoft");
    }
    double closePrice = m_database->getClosePrice(indexCode, m_priceTime);
    result.insert(m_indexCode, closePrice);

    return result;
}

bool GeneratePortfolioForm::isClosePriceValid(QMap<QString, double> secodePrice) {
    for (QMap<QString, double>::iterator it = secodePrice.begin();
         it != secodePrice.end(); ++it) {
        if (it.value() <= 0) {
            qDebug() << it.key() << ": " << it.value();
            return false;
        }
    }
    return true;
}

bool GeneratePortfolioForm::isStrategyWeightValid() {
    double sum = 0.0;
    for(QMap<QString, double>::iterator it = m_strategyWeight.begin();
        it != m_strategyWeight.end(); ++it) {
            sum += it.value();
    }
    if (sum <=0.0) {
        return false;
    } else {
        return true;
    }
}

QMap<QString, double> GeneratePortfolioForm::getNewPortfolio(QMap<QString, double> strategyMap,
                                                          QMap<QString, double> secodePrice,
                                                          double marketCapitalization) {
    QMap<QString, double> result;
    double sumWeight = 0.0;
    for (QMap<QString, double>::iterator it = strategyMap.begin();
         it != strategyMap.end(); ++it) {
        sumWeight += it.value();
    }

    for (QMap<QString, double>::iterator it = strategyMap.begin();
         it != strategyMap.end(); ++it) {
        QString secode = it.key();
        double newPosition = marketCapitalization * strategyMap[secode] / sumWeight / secodePrice[secode] ;
        result.insert(secode, newPosition);
    }
    return result;
}

QMap<QString, double> GeneratePortfolioForm::unitePortfolio(QMap<QString, double> desMap,
                                                            QMap<QString, double> oriMap) {
    for (QMap<QString, double>::iterator it = oriMap.begin();
         it != oriMap.end(); ++it) {
        if (desMap.find(it.key())!= desMap.end()) {
            desMap[it.key()] += it.value();
        } else {
            desMap.insert(it.key(), it.value());
        }
    }
    return desMap;
}

QMap<QString, double> GeneratePortfolioForm::intPorfolio(QMap<QString, double> oriPortfolio) {
    QMap<QString, double> result;
    for (QMap<QString, double>::iterator it = oriPortfolio.begin();
         it != oriPortfolio.end(); ++it) {
        QString secode = it.key();
        int newPosition = it.value();
        int modvalue = newPosition % 100;
        newPosition -= modvalue;
        if (modvalue >= 50) {
            newPosition += 100;
        }
        result.insert(secode, newPosition);
    }
    return result;
}

QList<QMap<QString, double>> GeneratePortfolioForm::getBuySalePortfolio() {
    QMap<QString, double> buyPortfolio;
    QMap<QString, double> salePortfolio;
    for (QMap<QString,double>::iterator it = m_currNewPortfolio.begin();
         it != m_currNewPortfolio.end(); ++it) {
        QString secode = it.key();
        if (m_currAccountMap.find(secode) == m_currAccountMap.end() && m_currNewPortfolio[secode] != 0) {
            buyPortfolio.insert(secode, m_currNewPortfolio[secode]);
        } else {
            int newPosition = m_currNewPortfolio[secode] - m_currAccountMap[secode];
            if (newPosition > 0) {
                buyPortfolio.insert(secode, newPosition);
            } else if (newPosition < 0) {
                salePortfolio.insert(secode, newPosition*-1);
            }
        }
    }

    for (QMap<QString, int>::iterator it = m_currAccountMap.begin();
         it != m_currAccountMap.end(); ++it) {
        QString secode = it.key();
        if (m_currNewPortfolio.find(it.key()) == m_currNewPortfolio.end() && m_currAccountMap[secode] != 0) {
            salePortfolio.insert(secode, m_currAccountMap[secode]);
        }
    }
    QList<QMap<QString, double>> result;
    result.append(buyPortfolio);
    result.append(salePortfolio);
    return result;
}

QString GeneratePortfolioForm::getStrategyWeightStringInfo(QMap<QString, double>  strategyWeight) {
    QString strategyWeightInfo = " ";
    QStringList splitFlagList;
    splitFlagList << "-" << "_" << "-" << "——";

    for (QMap<QString, double>::iterator it = strategyWeight.begin();
         it != strategyWeight.end(); ++it) {
        if (it.value() != 0) {
            QString pureStrategyFileName = it.key().section('.', 0, 0);
            QString splitFlag = "";

            for (QString curflag : splitFlagList) {
                if (pureStrategyFileName.indexOf(curflag) >= 0) {
                    splitFlag = curflag;
                    break;
                }
            }
            QString strategyClassName = (pureStrategyFileName.split(splitFlag))[0];
            strategyWeightInfo += strategyClassName + "_" + QString("%1").arg(it.value()) + "_";
        }
    }
    qDebug() << "strategyWeightInfo: " << strategyWeightInfo;
    return strategyWeightInfo;
}

QList<QString> GeneratePortfolioForm::getAccountStringInfo(QString accountFileName) {
    QString pureAccountFileName = accountFileName.section('.', 0, 0);
    QString currAccountName;
    QString currAccoutTimeInfo;

    QString splitFlag = " ";

    QStringList splitFlagList;
    splitFlagList << "-" << "_" << "-" << "——";
    for (QString curflag : splitFlagList) {
        if (pureAccountFileName.indexOf(curflag) >= 0) {
            splitFlag = curflag;
            break;
        }
    }

    QList<QString> currAccountFileInfoList = pureAccountFileName.split(splitFlag);
    if (currAccountFileInfoList.size() >= 3) {
        currAccountName = currAccountFileInfoList[1];
        currAccoutTimeInfo = currAccountFileInfoList[2];
    } else {
        currAccountName = pureAccountFileName;
        currAccoutTimeInfo = "";
    }

    QList<QString> result;
    result.append(currAccountName);
    result.append(currAccoutTimeInfo);
    return result;
}

void GeneratePortfolioForm::storeBuySalePortfolio() {
    QString strategyWeightInfo = getStrategyWeightStringInfo(m_strategyWeight);
    QList<QString> tmpAccountInfo = getAccountStringInfo(m_currAccountFileName);
    QString currAccountName = tmpAccountInfo[0];
    QString currAccoutTimeInfo = tmpAccountInfo[1];

    qDebug() << "strategyWeightInfo: " << strategyWeightInfo;
    qDebug() << "currAccountName: " << currAccountName;
    qDebug() << "currAccoutTimeInfo: " << currAccoutTimeInfo;

    QString geneTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmm");

    QString buyPortfolioFileName = m_accountPortfolioDir + currAccountName   + "_" + geneTime + "_"
                                 + QString::fromLocal8Bit("买入组合") + "_" +currAccoutTimeInfo + "_" + strategyWeightInfo
                                 + QString::fromLocal8Bit("价格时间_") + QString("%1").arg(m_priceTime) + ".xlsx";

    QString salePortfolioFileName = m_accountPortfolioDir + currAccountName + "_" + geneTime + "_"
                                  + QString::fromLocal8Bit("卖出组合") + "_" +currAccoutTimeInfo + "_" + strategyWeightInfo
                                  + QString::fromLocal8Bit("价格时间_") + QString("%1").arg(m_priceTime) + ".xlsx";

    QString newPortfolioFileName = m_accountPortfolioDir + currAccountName + "_" + geneTime + "_"
                                 + QString::fromLocal8Bit("持仓组合")  + "_" +currAccoutTimeInfo + "_" + strategyWeightInfo
                                 + QString::fromLocal8Bit("价格时间_") + QString("%1").arg(m_priceTime) + ".xlsx";

    QString buySalePortfolioFileName = m_accountPortfolioDir + currAccountName + "_" + geneTime + "_"
                                     + QString::fromLocal8Bit("点差_买卖组合") + "_" +currAccoutTimeInfo + "_" + strategyWeightInfo
                                     + QString::fromLocal8Bit("价格时间_") + QString("%1").arg(m_priceTime) + ".xlsx";


    m_portfolioAmount.insert(buyPortfolioFileName, QString("%1").arg(getPortfolioAmount(m_currSecodeClosePrice, m_currBuyPortfolio)));
    m_portfolioAmount.insert(salePortfolioFileName, QString("%1").arg(getPortfolioAmount(m_currSecodeClosePrice, m_currSalePortfolio)));
    m_portfolioAmount.insert(newPortfolioFileName, QString("%1").arg(getPortfolioAmount(m_currSecodeClosePrice, m_currNewPortfolio)));

    QFile buyfile(buyPortfolioFileName);
    if (buyfile.exists()) {
        buyfile.remove();
        qDebug() << "delete " << buyPortfolioFileName;
    }

    QFile salefile(salePortfolioFileName);
    if (salefile.exists()) {
        salefile.remove();
        qDebug() << "delete " << salePortfolioFileName;
    }

    QFile newfile(newPortfolioFileName);
    if (newfile.exists()) {
        newfile.remove();
        qDebug() << "delete " << newPortfolioFileName;
    }

    QFile buySalefile(buySalePortfolioFileName);
    if (buySalefile.exists()) {
        buySalefile.remove();
        qDebug() << "delete " << buySalePortfolioFileName;
    }

    writePortfolio(m_currBuyPortfolio, buyPortfolioFileName, "2", "buy");
    writeFileInfo(buyPortfolioFileName, "A1", m_portfolioAmount[buyPortfolioFileName], "Sheet2");

    writePortfolio(m_currSalePortfolio, salePortfolioFileName, "2", "sale");
    writeFileInfo(salePortfolioFileName, "A1", m_portfolioAmount[salePortfolioFileName], "Sheet2");

//    writePortfolio(m_currNewPortfolio, newPortfolioFileName, "ori", "ori");
//    writeFileInfo(newPortfolioFileName, "A1", m_portfolioAmount[newPortfolioFileName], "Sheet2");

    writePortfolio(m_currSalePortfolio, buySalePortfolioFileName, "", "sale", "sale");
    writePortfolio(m_currBuyPortfolio, buySalePortfolioFileName, "", "buy", "buy");

    setBuySalePortfolioTable();
    clearGeneInfo();
}

QMap<QString, double> GeneratePortfolioForm::getStrategyWeight() {
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    QMap<QString, double> result;
    for (int i = 0; i < standardItemModel->rowCount(); ++i) {
        QString strategyFileName = standardItemModel->item(i,0)->text();
        double HS300Count = standardItemModel->item(i,1)->text().toDouble();
        if (HS300Count > 0.0) {
            result.insert(strategyFileName, HS300Count);
        }
    }
    return result;
}

QMap<QString, QMap<QString, double>> GeneratePortfolioForm::getCurrStrategy(QMap<QString, double> strategyWeight) {
    QMap<QString, QMap<QString, double>> result;
    for (QMap<QString, double>::iterator it = strategyWeight.begin();
         it != strategyWeight.end(); ++it) {
        QString fileName = m_strategyDir + it.key();
        QList<QStringList> oriStrategyData = readExcelData(fileName);
        QMap<QString, double> curMapData;
        for (int j = 0; j < oriStrategyData.size(); ++j) {
            if (oriStrategyData[j].size() < 2) {
                QMessageBox::critical(NULL, "Error", QString::fromLocal8Bit("读取策略失败: %1, 内容有错误").arg(fileName));
                result.insert("Error", curMapData);
                return result;
            } else {
                curMapData.insert(getCompleteSecode(oriStrategyData[j][0], "ori"), oriStrategyData[j][1].toDouble());
            }
        }
        result.insert(it.key(), curMapData);
    }
    return result;
}

QList<QString> GeneratePortfolioForm::getCurrSecodeList(QMap<QString, QMap<QString, double>> allStrategyMap) {
    QList<QString> result;
    for (QMap<QString, QMap<QString, double>>::iterator it = allStrategyMap.begin();
         it != allStrategyMap.end(); ++it) {
        QList<QString> tmpList = it.value().keys();
        for (QString secode : tmpList) {
            if (result.indexOf(secode) < 0) {
                result.append(secode);
            }
        }
    }
    return result;
}

QMap<QString, double> GeneratePortfolioForm::getCurStrategy() {
    QMap<QString, double> result;
    for (QMap<QString, double>::iterator itWeight = m_strategyWeight.begin();
         itWeight != m_strategyWeight.end(); ++itWeight) {

        QMap<QString, double> curStrategy = m_allStrategy[itWeight.key()];
        if (itWeight.value() != 0) {
            for (QMap<QString, double>::iterator itStrategy = curStrategy.begin();
                 itStrategy != curStrategy.end(); ++itStrategy) {
                if (result.find(itStrategy.key()) == result.end()) {
                    result.insert(itStrategy.key(), itStrategy.value() * itWeight.value());
                } else {
                    result[itStrategy.key()] += itStrategy.value() * itWeight.value();
                }
            }
        }
    }
    return result;
}

void GeneratePortfolioForm::clearGeneInfo() {
    m_strategyWeight.clear();
    m_currStrategyMap.clear();
    m_currAccountMap.clear();
}

double GeneratePortfolioForm::getCapitalization(double HS300Count) {
    QString indexCode = "000300";
    if (m_priceTime == "realtime") {
        indexCode = getCompleteIndexCode(indexCode, "wind");
    } else {
        indexCode = getCompleteIndexCode(indexCode, "tinysoft");
    }
    double closePrice = m_database->getClosePrice(indexCode, m_priceTime);
    double result = closePrice * 300 * HS300Count;
    return result;
}

bool GeneratePortfolioForm::isCapitalizationValid(double Capitalization) {
    if (Capitalization <= 0) {
        return false;
    } else {
        return true;
    }
}

void GeneratePortfolioForm::on_choose_account_button_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(NULL, "caption", ".");
    if (file_name == "") return;
    m_accountCurrDir = file_name + "/";
    writeFileInfo(m_nativeFileName, "A1", m_accountCurrDir);
    setAccountTable();
    updateProgramInfo(ui->programInfo_Table, QString(QString::fromLocal8Bit("切换持仓账户文件夹为: %1"))
                                                    .arg(m_accountCurrDir));
    qDebug() << "new m_accountCurrDir: " << file_name;

    QDir porfolioDir(m_accountCurrDir + "买卖组合/");
    if (porfolioDir.exists()) {
        m_accountPortfolioDir = m_accountCurrDir + "买卖组合/";
        setBuySalePortfolioTable();
        updateProgramInfo(ui->programInfo_Table, QString(QString::fromLocal8Bit("切换生成组合文件夹为: %1"))
                                                        .arg(m_accountPortfolioDir));
    }
}

void GeneratePortfolioForm::on_choose_strategy_button_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");
    if (file_name == "") return;
    m_strategyDir = file_name+"/";
    writeFileInfo(m_nativeFileName, "A2", m_strategyDir);
    setStrategyTable();
    updateProgramInfo(ui->programInfo_Table, QString::fromLocal8Bit("切换策略文件夹为: %1")
                                                    .arg(m_strategyDir));
    qDebug() << "new m_strategyDir: " <<file_name;
}

void GeneratePortfolioForm::on_choose_portfolio_button_clicked()
{
    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");
    if (file_name == "") return;
    m_accountPortfolioDir = file_name + "/";
    writeFileInfo(m_nativeFileName, "A3", m_accountPortfolioDir);
    setBuySalePortfolioTable();
    updateProgramInfo(ui->programInfo_Table, QString(QString::fromLocal8Bit("切换生成组合文件夹为: %1"))
                                                    .arg(m_accountPortfolioDir));
    qDebug() << "new m_accountPortfolioDir: " << file_name;

}

//    void initAccountTable();
//    void initSalePortfolioTable();
//    void initStrategyTable();

//void GeneratePortfolioForm::initAccountTable() {
//    m_currAccountFileList = getExcelFileInfo(m_accountCurrDir);

//    QStandardItemModel* standardItemModel = getStandardItemModel(m_currAccountFileList);
//    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("持仓信息")));
//    ui->account_table->setModel(standardItemModel);
//    ui->account_table->setColumnWidth (0, 300);
//    ui->account_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->account_table->setShowGrid (false);

//}

//void GeneratePortfolioForm::initSalePortfolioTable() {
//    m_buySalePortfolioFileList = getExcelFileInfo(m_accountPortfolioDir);
//    QStandardItemModel* standardItemModel = getStandardItemModel( m_buySalePortfolioFileList);
//    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("买卖组合信息")));
//    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("金额")));
//    ui->buySalePortfolio_table->setModel(standardItemModel);
//    ui->buySalePortfolio_table->setColumnWidth (0, 400);
//    ui->buySalePortfolio_table->setColumnWidth (1, 100);
//    ui->buySalePortfolio_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    ui->buySalePortfolio_table->setShowGrid (false);

//    ui->buySalePortfolio_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//}

//void GeneratePortfolioForm::initStrategyTable() {
//    m_strategyFileList = getExcelFileInfo(m_strategyDir);
//    QStandardItemModel* standardItemModel = new QStandardItemModel();
//    standardItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString::fromLocal8Bit("策略")));
//    standardItemModel->setHorizontalHeaderItem(1, new QStandardItem(QString::fromLocal8Bit("沪深300张数")));
//    for (int i = 0; i < m_strategyFileList.size(); ++i) {
//        standardItemModel->setItem(i, 0, new QStandardItem(m_strategyFileList[i].fileName()));
//        standardItemModel->setItem(i, 1, new QStandardItem(QString("%1").arg(0.0)));
//        m_strategyWeight.insert(m_strategyFileList[i].fileName(), 0.0);
//    }

//    ui->strategy_table->setModel(standardItemModel);
//    ui->strategy_table->setColumnWidth (0, 200);
//    ui->strategy_table->setColumnWidth (1, 100);
//    ui->strategy_table->setEditTriggers(QAbstractItemView::DoubleClicked);
//    ui->strategy_table->setShowGrid (false);
//}

//void GeneratePortfolioForm::setBuySalePortfolioTable() {
//    m_buySalePortfolioFileList = getExcelFileInfo(m_accountPortfolioDir);

//    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
//    if(NULL == standardItemModel) {
//        standardItemModel = new QStandardItemModel();
//    }
//    standardItemModel->clear();
//    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString::fromLocal8Bit("买卖组合信息")));
//    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QString::fromLocal8Bit("金额")));
//    ui->buySalePortfolio_table->setColumnWidth (0, 400);
//    ui->buySalePortfolio_table->setColumnWidth (1, 100);
//    QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
//    for (int i = 0; i < m_buySalePortfolioFileList.size(); ++i) {
//        int row = standardItemModel->rowCount ();
//        QString fileName = m_buySalePortfolioFileList[i].fileName();
//        standardItemModel->setItem (row, 0,  new QStandardItem(fileName));

//        if (m_portfolioAmount.find(m_buySalePortfolioFileList[i].filePath()) != m_portfolioAmount.end()) {
//            QString amount = QString("%1").arg(m_portfolioAmount[m_buySalePortfolioFileList[i].filePath()]);
//            standardItemModel->setItem (row, 1,  new QStandardItem(amount));
//        }

//        QScrollBar* verScrollBar = ui->buySalePortfolio_table->verticalScrollBar();
//        verScrollBar->setSliderPosition(verScrollBar->maximum());
//    }
//    ui->buySalePortfolio_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//}

//void GeneratePortfolioForm::initAccountNameList() {
//    QList<QFileInfo> allInfo = getDirName(m_accountFatherDir);
//    QString accountDirPrex = "账户";
//    for (int i = 0; i < allInfo.size(); ++i) {
//        if (allInfo[i].fileName().indexOf(accountDirPrex) >= 0) {
//            m_accountDirList.append(allInfo[i]);
//        }
//    }
//}

//void GeneratePortfolioForm::on_chooseAccount_ComboBox_currentIndexChanged(int index)
//{
//    index;
//    if (m_isInit) {
//        m_currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
//        m_accountCurrDir = m_currAccountInfo.filePath();
//        m_accountPortfolioDir = m_accountCurrDir + "/买卖组合/";
//        setAccountTable();
//        setBuySalePortfolioTable();
//    }
//}

//void GeneratePortfolioForm::initAccountCombox() {
//    QStringList accountNameList;
//    for (int i = 0; i < m_accountDirList.size(); ++i) {
//        accountNameList.append(m_accountDirList[i].fileName());
//    }
//    ui->chooseAccount_ComboBox->addItems(accountNameList);
//    ui->chooseAccount_ComboBox->setCurrentIndex(0);
//}

//void GeneratePortfolioForm::on_refreshAllTable_clicked()
//{
//    setAccountTable();
//    setStrategyTable();
//    setBuySalePortfolioTable();
//}

//void GeneratePortfolioForm::initAllStrategy() {
//    for (int i = 0; i < m_strategyFileList.size(); ++i) {
////        QList<QStringList> oriStrategyData = m_excel.readOriData(m_strategyFileList[i].filePath(), m_strategyExcelFileColNumb);
//        QList<QStringList> oriStrategyData = readExcelData(m_strategyFileList[i].filePath());
//        QMap<QString, double> curMapData;
//        for (int j = 0; j < oriStrategyData.size(); ++j) {
//            curMapData.insert(oriStrategyData[j][0], oriStrategyData[j][1].toDouble());
//        }
//        m_allStrategy.insert(m_strategyFileList[i].fileName(), curMapData);
//    }
//}

//void GeneratePortfolioForm::contextMenuEvent(QContextMenuEvent *event)
//{
//    m_contextMenu = new QMenu(this);
//    m_contextMenu->addAction(new QAction("刷新图表", this));
//    m_contextMenu->move(cursor().pos());
//    m_contextMenu->show();
//}

//void GeneratePortfolioForm::on_strategy_table_clicked(const QModelIndex &index)
//{
//    m_currStrategyMap.clear();
//    int intIndex = index.row ();
//    QString strategyFilePath = m_strategyFileList[intIndex].filePath();
//    m_currStrategFileInfo = m_strategyFileList[intIndex];
//    QList<QStringList> oriStrategyData = m_excel.readOriData(strategyFilePath, m_strategyExcelFileColNumb);
//    for (int i = 0; i < oriStrategyData.size(); ++i) {
//        m_currStrategyMap.insert(oriStrategyData[i][0], oriStrategyData[i][1].toDouble());
//    }
//    updateProgramInfo(ui->programInfo_Table, QString("读取策略: %1").arg(m_currStrategFileInfo.fileName()));
//}
