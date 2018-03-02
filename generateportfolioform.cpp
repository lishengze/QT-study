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

    m_isInit = true;
//    m_timer.start(m_refreshTime);
}

GeneratePortfolioForm::~GeneratePortfolioForm()
{
    m_timer.stop();
    if (NULL != m_database) {
        delete m_database;
        m_database = NULL;
    }
    delete ui;
}

void GeneratePortfolioForm::initCommnData() {
//    m_accountFatherDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/");
//    m_strategyDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/策略/");
    m_isInit = false;
    m_mainDir = "//192.168.211.182/";
    m_accountFatherDir = m_mainDir + "it程序设计/";
    m_strategyDir = m_accountFatherDir + "策略/";
    m_database = new Database("3", "192.168.211.165");
    m_priceTime = "realtime";

    m_refreshTime = 1000;

    initAccountNameList();
    initExcelFileInfo();
    initAllStrategy();
}

void GeneratePortfolioForm::initExcelFileInfo() {
    m_strategyExcelFileColNumb = 2;
    m_accoutnExcelFileColNumbList << 11 << 51;
    QList<int> account1ExcelFileUserfulNumb;
    account1ExcelFileUserfulNumb << 3 << 5;
    QList<int> account2ExcelFileUserfulNumb;
    account2ExcelFileUserfulNumb << 2 << 11;
    m_accountExcelFileUsefulColNumbList << account1ExcelFileUserfulNumb << account2ExcelFileUserfulNumb;
}

void GeneratePortfolioForm::initAccountNameList() {
    QList<QFileInfo> allInfo = getDirName(m_accountFatherDir);
    QString accountDirPrex = "账户";
    for (int i = 0; i < allInfo.size(); ++i) {
        if (allInfo[i].fileName().indexOf(accountDirPrex) >= 0) {
            m_accountDirList.append(allInfo[i]);
        }
    }
}

void GeneratePortfolioForm::initAllStrategy() {
    m_strategyFileList = getExcelFileInfo(m_strategyDir);
    for (int i = 0; i < m_strategyFileList.size(); ++i) {
        QList<QStringList> oriStrategyData = m_excel.readOriData(m_strategyFileList[i].filePath(), m_strategyExcelFileColNumb);
        QMap<QString, double> curMapData;
        for (int j = 0; j < oriStrategyData.size(); ++j) {
            curMapData.insert(oriStrategyData[j][0], oriStrategyData[j][1].toDouble());
        }
        m_allStrategy.insert(m_strategyFileList[i].fileName(), curMapData);
    }
}

void GeneratePortfolioForm::initWidget() {
    ui->chooseHS300Count->setValue(2.0);
    ui->chooseRealTimeRatio->setChecked(true);

    initChoosePriceDate();
    initAccountCombox();
    initAccountTable();
    initSalePortfolioTable();
    initStrategyTable();
    initProgramInfoTable();
}

void GeneratePortfolioForm::initChoosePriceDate() {
    ui->chooseHistDate->setCalendarPopup(true);
    ui->chooseRealTimeRatio->setChecked(true);
}

void GeneratePortfolioForm::initAccountCombox() {
    QStringList accountNameList;
    for (int i = 0; i < m_accountDirList.size(); ++i) {
        accountNameList.append(m_accountDirList[i].fileName());
    }
    ui->chooseAccount_ComboBox->addItems(accountNameList);
    ui->chooseAccount_ComboBox->setCurrentIndex(0);
}

void GeneratePortfolioForm::initAccountTable() {
    QFileInfo currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
    m_currAccountFileList = getExcelFileInfo(currAccountInfo.filePath());
    qDebug() << "initAccountTable currAccountInfo.filePath(): " << currAccountInfo.filePath();

    QStandardItemModel* standardItemModel = getStandardItemModel("账户信息", m_currAccountFileList);
    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("持仓信息")));
    ui->account_table->setModel(standardItemModel);
    ui->account_table->setColumnWidth (0, 300);
    ui->account_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->account_table->setShowGrid (false);

}

void GeneratePortfolioForm::initSalePortfolioTable() {
    m_currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
    ui->buySalePortfolio_Label->setText(m_currAccountInfo.fileName() + " 买卖组合信息: ");

    m_accountPortfolioDir= m_currAccountInfo.filePath() + "/买卖组合/";
    m_buySalePortfolioFileList = getExcelFileInfo(m_accountPortfolioDir);
    QStandardItemModel* standardItemModel = getStandardItemModel("买卖组合", m_buySalePortfolioFileList);
    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("买卖组合信息")));
    standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("")));
    ui->buySalePortfolio_table->setModel(standardItemModel);
    ui->buySalePortfolio_table->setColumnWidth (0, 400);
    ui->buySalePortfolio_table->setColumnWidth (1, 100);
    ui->buySalePortfolio_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->buySalePortfolio_table->setShowGrid (false);

    ui->buySalePortfolio_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void GeneratePortfolioForm::initStrategyTable() {   
    QStandardItemModel* standardItemModel = new QStandardItemModel();
    standardItemModel->setHorizontalHeaderItem(0, new QStandardItem("策略"));
    standardItemModel->setHorizontalHeaderItem(1, new QStandardItem("沪深三百张数"));
    for (int i = 0; i < m_strategyFileList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(m_strategyFileList[i].fileName()));
        standardItemModel->setItem(i, 1, new QStandardItem(QString("%1").arg(0.0)));
        m_strategyWeight.insert(m_strategyFileList[i].fileName(), 0.0);
    }

    ui->strategy_table->setModel(standardItemModel);
    ui->strategy_table->setColumnWidth (0, 200);
    ui->strategy_table->setColumnWidth (1, 100);
    ui->strategy_table->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->strategy_table->setShowGrid (false);
}

void GeneratePortfolioForm::initProgramInfoTable() {
    QStandardItemModel* initModel = new QStandardItemModel();
    initModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("时间")));
    initModel -> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("消息")));
    initModel -> setHorizontalHeaderItem (2, new QStandardItem(QObject::tr("备注")));
    ui->programInfo_Table->setModel (initModel);
    ui->programInfo_Table->setColumnWidth (0, 250);
    ui->programInfo_Table->setColumnWidth (1, 600);
    ui->programInfo_Table->setColumnWidth (2, 150);
    ui->programInfo_Table->setShowGrid (false);
}

void GeneratePortfolioForm::setAccountTable() {
    QFileInfo currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
//    qDebug() << "setAccountTable currAccountInfo.filePath(): " << currAccountInfo.filePath();
    m_currAccountFileList = getExcelFileInfo(currAccountInfo.filePath());
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->account_table->model ());
    if (NULL != standardItemModel) {
        standardItemModel->clear();
        standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("持仓信息")));
        ui->account_table->setColumnWidth (0, 300);

        QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
        for (int i = 0; i < m_currAccountFileList.size(); ++i) {
            int row = standardItemModel->rowCount ();
//            qDebug() << m_currAccountFileList[i].fileName();
            standardItemModel->setItem (row, 0,  new QStandardItem(m_currAccountFileList[i].fileName()));
            QScrollBar* verScrollBar = ui->account_table->verticalScrollBar();
            verScrollBar->setSliderPosition(verScrollBar->maximum());
        }
    }
}

void GeneratePortfolioForm::setStrategyTable() {
    m_strategyFileList = getExcelFileInfo(m_strategyDir);
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());

    if(NULL != standardItemModel) {
        standardItemModel->clear();
        standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("策略")));
        standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("沪深三百张数")));
        ui->strategy_table->setColumnWidth (0, 200);
        ui->strategy_table->setColumnWidth (1, 100);
        for (int i = 0; i < m_strategyFileList.size(); ++i) {
            int row = standardItemModel->rowCount ();
            standardItemModel->setItem (row, 0,  new QStandardItem(m_strategyFileList[i].fileName()));
            standardItemModel->setItem (row, 1,  new QStandardItem(QString("%1").arg(0.0)));
            QScrollBar* verScrollBar = ui->strategy_table->verticalScrollBar();
            verScrollBar->setSliderPosition(verScrollBar->maximum());
        }
    }
}

void GeneratePortfolioForm::setBuySalePortfolioTable() {
    m_currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
    ui->buySalePortfolio_Label->setText(m_currAccountInfo.fileName() + " 买卖组合信息: ");

    m_accountPortfolioDir = m_currAccountInfo.filePath() + "/买卖组合/";
    m_buySalePortfolioFileList = getExcelFileInfo(m_accountPortfolioDir);

    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    if(NULL != standardItemModel) {
        standardItemModel->clear();
        standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("买卖组合信息")));
        standardItemModel-> setHorizontalHeaderItem (1, new QStandardItem(QObject::tr("")));
        ui->buySalePortfolio_table->setColumnWidth (0, 400);
        ui->buySalePortfolio_table->setColumnWidth (1, 100);
        QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
        for (int i = 0; i < m_buySalePortfolioFileList.size(); ++i) {
            int row = standardItemModel->rowCount ();
            standardItemModel->setItem (row, 0,  new QStandardItem(m_buySalePortfolioFileList[i].fileName()));
            QScrollBar* verScrollBar = ui->buySalePortfolio_table->verticalScrollBar();
            verScrollBar->setSliderPosition(verScrollBar->maximum());
        }
        ui->buySalePortfolio_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
}

void GeneratePortfolioForm::on_chooseAccount_ComboBox_currentIndexChanged(int index)
{
    index;
    if (m_isInit) {
        setAccountTable();
        setBuySalePortfolioTable();
    }
}

void GeneratePortfolioForm::on_account_table_clicked(const QModelIndex &index)
{
    m_currAccountMap.clear();
    int intIndex = index.row ();
    QString accountFilePath = m_currAccountFileList[intIndex].filePath();
    m_currAccountFileInfo = m_currAccountFileList[intIndex];

    int accountIndex = ui->chooseAccount_ComboBox->currentIndex();
    QList<QStringList> oriAccountData = m_excel.readOriData(accountFilePath, m_accoutnExcelFileColNumbList[accountIndex]);

    QList<int> accountExcelFileUserfulNumbList = m_accountExcelFileUsefulColNumbList[accountIndex];
    for (int i = 1; i < oriAccountData.size(); ++i) {
        QString secodeName = oriAccountData[i][accountExcelFileUserfulNumbList[0]];
        int secodeCount = oriAccountData[i][accountExcelFileUserfulNumbList[1]].toInt();
        if (isSecodeValid(secodeName)) {
            m_currAccountMap.insert(secodeName, secodeCount);
        }
    }
    updateProgramInfo(ui->programInfo_Table, QString("读取账户持仓组合: %1").arg(m_currAccountFileInfo.fileName()));
//    qDebug() << m_currAccountMap;
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
    int intIndex = index.row ();
    QString fileName = m_currAccountFileList[intIndex].filePath();
//    fileName = QString::fromLocal8Bit(fileName.toStdString().c_str());
    qDebug() << "fileName: " << fileName;
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void GeneratePortfolioForm::on_buySalePortfolio_table_doubleClicked(const QModelIndex &index)
{
    int intIndex = index.row ();
    QString fileName = m_buySalePortfolioFileList[intIndex].filePath();
    QDesktopServices ds;
    ds.openUrl(QUrl::fromLocalFile(fileName));
}

void GeneratePortfolioForm::on_geneBuySalePortfolio_clicked()
{
    m_strategyWeight = getStrategyWeight();
    if (!isStrategyWeightValid()) {
        QMessageBox::critical(NULL, "Error", "策略权重错误");
        return;
    }
    m_currStrategyMap = getCurStrategy();
//    printMap(m_strategyWeight, "m_strategyWeight");
//    printMap(m_currStrategyMap, "m_currStrategyMap");

    if (m_currAccountMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择账户");
        return;
    } else {

        QString strategyWeightInfo = "";
        for (QMap<QString, double>::iterator it = m_strategyWeight.begin();
             it != m_strategyWeight.end(); ++it) {
            strategyWeightInfo += it.key().section('.', 0, 0) + ": " + QString("%1").arg(it.value()) + "\n";
        }

        QString info = "选择的持仓组合为: " + m_currAccountFileInfo.fileName() + "\n"
                     + "设定的策略权重为: \n" + strategyWeightInfo
                     + "收盘价时间为: " + m_priceTime + "\n"
                     + "HS张数: " + QString("%1").arg(ui->chooseHS300Count->value());
        if (QMessageBox::Yes == QMessageBox::information(NULL, "确认信息", info, QMessageBox::Yes | QMessageBox::No)) {
            updateProgramInfo(ui->programInfo_Table, tr("----- 生成买卖组合信息 -----"));
            m_currSecodeClosePrice = getClosePrice();
            m_currMarketCapitalization = getCapitalization();

            printMap(m_currSecodeClosePrice, "m_currSecodeClosePrice");
            qDebug() << "m_currMarketCapitalization: " << m_currMarketCapitalization;

            if (!isClosePriceValid(m_currSecodeClosePrice) || !isCapitalizationValid(m_currMarketCapitalization)) {
                QMessageBox::critical(NULL, "Error", "重新选择时间");
                return;
            }

            m_currNewPortfolio = getNewPortfolio();
            QList<QMap<QString, int>> newBuySale = getBuySalePortfolio();
            m_currBuyPortfolio = newBuySale[0];
            m_currSalePortfolio = newBuySale[1];

            QStringList infoList = info.split("\n");

            for (int i = 0; i < infoList.size(); ++i) {
                updateProgramInfo(ui->programInfo_Table, infoList[i]);
            }
            storeBuySalePortfolio();
            updateProgramInfo(ui->programInfo_Table, "----- 生成买卖组合结束 -----");
        }
    }
}

QMap<QString, double> GeneratePortfolioForm::getClosePrice() {
    QMap<QString, double> result;
    for (QMap<QString, double>::iterator it = m_currStrategyMap.begin();
         it != m_currStrategyMap.end(); ++it) {
        QString oriSecode = it.key();
        double closePrice = m_database->getClosePrice(oriSecode, m_priceTime);
        result.insert(oriSecode, closePrice);
    }
    return result;
}

double GeneratePortfolioForm::getCapitalization() {
    QString indexCode = "000300";
    if (m_priceTime == "realtime") {
        indexCode = getCompleteIndexCode(indexCode, "wind");
    } else {
        indexCode = getCompleteIndexCode(indexCode, "tinysoft");
    }
    double closePrice = m_database->getClosePrice(indexCode, m_priceTime);
    double HS300Count = ui->chooseHS300Count->value();
    double result = closePrice * 300 * HS300Count;
    return result;
}

bool GeneratePortfolioForm::isClosePriceValid(QMap<QString, double> secodePrice) {
    for (QMap<QString, double>::iterator it = secodePrice.begin();
         it != secodePrice.end(); ++it) {
        if (it.value() <= 0) {
            return false;
        }
    }
    return true;
}

bool GeneratePortfolioForm::isCapitalizationValid(double Capitalization) {
    if (Capitalization <= 0) {
        return false;
    } else {
        return true;
    }
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

QMap<QString, int> GeneratePortfolioForm::getNewPortfolio() {
    QMap<QString, int> result;
    double sumWeight = 0.0;
    for (QMap<QString, double>::iterator it = m_currStrategyMap.begin();
         it != m_currStrategyMap.end(); ++it) {
        sumWeight += it.value();
    }

    for (QMap<QString, double>::iterator it = m_currStrategyMap.begin();
         it != m_currStrategyMap.end(); ++it) {
        QString secode = it.key();
        int newPosition = m_currMarketCapitalization * m_currStrategyMap[secode] / sumWeight / m_currSecodeClosePrice[secode] ;
        int modvalue = newPosition % 100;
        newPosition -= modvalue;
        if (modvalue >= 50) {
            newPosition += 100;
        }
        result.insert(secode, newPosition);
    }
    return result;
}

QList<QMap<QString, int>> GeneratePortfolioForm::getBuySalePortfolio() {
    QMap<QString, int> buyPortfolio;
    QMap<QString, int> salePortfolio;
    for (QMap<QString,int>::iterator it = m_currNewPortfolio.begin();
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
        if (m_currNewPortfolio.find(it.key()) == m_currNewPortfolio.end()) {
            salePortfolio.insert(secode, m_currAccountMap[secode]);
        }
    }
    QList<QMap<QString, int>> result;
    result.append(buyPortfolio);
    result.append(salePortfolio);
    return result;
}

void GeneratePortfolioForm::storeBuySalePortfolio() {
    QString strategyWeightInfo = "";
    for (QMap<QString, double>::iterator it = m_strategyWeight.begin();
         it != m_strategyWeight.end(); ++it) {
        if (it.value() != 0) {
            strategyWeightInfo += it.key().section('.', 0, 0) + "_" + QString("%1").arg(it.value()) + "_";
        }
    }

//    QString fileNamePrefix = m_accountPortfolioDir + m_currAccountFileInfo.fileName().section('.', 0, 0)
//                        + "_" + strategyWeightInfo
//                        + "HS300张数_" + QString("%1").arg(ui->chooseHS300Count->value())
//                        + "_价格时间_" + QString("%1").arg(m_priceTime);

    QString buyPortfolioFileName = m_accountPortfolioDir + m_currAccountFileInfo.fileName().section('.', 0, 0)
                                + "_买入组合" + "_" + strategyWeightInfo
                                + "HS300张数_" + QString("%1").arg(ui->chooseHS300Count->value())
                                + "_价格时间_" + QString("%1").arg(m_priceTime) + ".xlsx";

    QString salePortfolioFileName = m_accountPortfolioDir + m_currAccountFileInfo.fileName().section('.', 0, 0)
                                + "_卖出组合" + "_" + strategyWeightInfo
                                + "HS300张数_" + QString("%1").arg(ui->chooseHS300Count->value())
                                + "_价格时间_" + QString("%1").arg(m_priceTime) + ".xlsx";

    qDebug() << "buyPortfolioFileName: " << buyPortfolioFileName;
    qDebug() << "salePortfolioFileName: " << salePortfolioFileName;

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

    writePortfolio(m_currBuyPortfolio, buyPortfolioFileName);
    writePortfolio(m_currSalePortfolio, salePortfolioFileName);
    setBuySalePortfolioTable();

    clearGeneInfo();
}

QMap<QString, double> GeneratePortfolioForm::getStrategyWeight() {
    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->strategy_table->model ());
    QMap<QString, double> result;
    for (int i = 0; i < standardItemModel->rowCount(); ++i) {
        QString strategyFileName = standardItemModel->item(i,0)->text();
        result.insert(strategyFileName, standardItemModel->item(i,1)->text().toDouble());
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

void GeneratePortfolioForm::on_refreshAllTable_clicked()
{
    setAccountTable();
    setStrategyTable();
    setBuySalePortfolioTable();
}

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



