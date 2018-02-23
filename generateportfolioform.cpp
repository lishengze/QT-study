#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDateTime>
#include <QDate>
#include <QScrollBar>
#include <QMessageBox>
#include "generateportfolioform.h"
#include "ui_generateportfolioform.h"
#include "toolfunc.h"

GeneratePortfolioForm::GeneratePortfolioForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneratePortfolioForm)
{
    ui->setupUi(this);

    initCommnData();
    initWidget();

    m_isInit = true;
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
//    m_accountFatherDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/");
//    m_strategyDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/策略/");
    m_isInit = false;
    m_mainDir = "//192.168.211.182/";
    m_accountFatherDir = m_mainDir + "it程序设计/";
    m_strategyDir = m_accountFatherDir + "策略/";
    m_database = new Database("3", "192.168.211.165");
    m_priceTime = "realtime";

    initAccountNameList();
    initExcelFileInfo();
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

    QString curBuySalePortfolioDir = m_currAccountInfo.filePath() + "/买卖组合/";
    m_buySalePortfolioFileList = getExcelFileInfo(curBuySalePortfolioDir);
    QStandardItemModel* standardItemModel = getStandardItemModel("买卖组合", m_buySalePortfolioFileList);
    standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("买卖组合信息")));
    ui->buySalePortfolio_table->setModel(standardItemModel);
    ui->buySalePortfolio_table->setColumnWidth (0, 300);
    ui->buySalePortfolio_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->buySalePortfolio_table->setShowGrid (false);
}

void GeneratePortfolioForm::initStrategyTable() {
    m_strategyFileList = getExcelFileInfo(m_strategyDir);
    QStandardItemModel* strategyItemModel = getStandardItemModel("策略名称", m_strategyFileList);
    ui->strategy_table->setModel(strategyItemModel);
    ui->strategy_table->setColumnWidth (0, 300);
    ui->strategy_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    qDebug() << "setAccountTable currAccountInfo.filePath(): " << currAccountInfo.filePath();
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

void GeneratePortfolioForm::setBuySalePortfolioTable() {
    m_currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
    ui->buySalePortfolio_Label->setText(m_currAccountInfo.fileName() + " 买卖组合信息: ");

    QString curBuySalePortfolioDir = m_currAccountInfo.filePath() + "/买卖组合/";
    m_buySalePortfolioFileList = getExcelFileInfo(curBuySalePortfolioDir);

    QStandardItemModel* standardItemModel =  dynamic_cast<QStandardItemModel*>(ui->buySalePortfolio_table->model ());
    if(NULL != standardItemModel) {
        standardItemModel->clear();
        standardItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("买卖组合信息")));
        ui->buySalePortfolio_table->setColumnWidth (0, 300);

        QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
        for (int i = 0; i < m_buySalePortfolioFileList.size(); ++i) {
            int row = standardItemModel->rowCount ();
            standardItemModel->setItem (row, 0,  new QStandardItem(m_buySalePortfolioFileList[i].fileName()));
            QScrollBar* verScrollBar = ui->buySalePortfolio_table->verticalScrollBar();
            verScrollBar->setSliderPosition(verScrollBar->maximum());
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

void GeneratePortfolioForm::on_chooseAccount_ComboBox_currentIndexChanged(int index)
{
    index;
    if (m_isInit) {
        setAccountTable();
        setBuySalePortfolioTable();
    }
}

void GeneratePortfolioForm::on_strategy_table_clicked(const QModelIndex &index)
{
    m_currStrategyMap.clear();
    int intIndex = index.row ();
    QString strategyFilePath = m_strategyFileList[intIndex].filePath();
    m_currStrategFileInfo = m_strategyFileList[intIndex];
    QList<QStringList> oriStrategyData = m_excel.readOriData(strategyFilePath, m_strategyExcelFileColNumb);
    for (int i = 0; i < oriStrategyData.size(); ++i) {
        m_currStrategyMap.insert(oriStrategyData[i][0], oriStrategyData[i][1].toDouble());
    }
//    qDebug() << m_currStrategyMap;
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

void GeneratePortfolioForm::on_geneBuySalePortfolio_clicked()
{
    if (m_currStrategyMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择策略");
        return;
    } else if (m_currAccountMap.size() == 0) {
        QMessageBox::critical(NULL, "Error", "还未选择账户");
        return;
    } else {
        QString info = "选择的持仓组合为: " + m_currAccountFileInfo.fileName() + "\n"
                     + "选择的策略为: " + m_currStrategFileInfo.fileName() + "\n"
                     + "收盘价时间为: " + m_priceTime + ", HS张数: " + QString("%1").arg(ui->chooseHS300Count->value());
//        info = QString::fromLocal8Bit(info);
        if (QMessageBox::Yes == QMessageBox::information(NULL, "确认信息", info, QMessageBox::Yes | QMessageBox::No)) {
            QMap<QString, double> secodeClosePrice = getClosePrice();
            double marketCapitalization = getCapitalization();
            qDebug() << "secodeClosePrice: " << secodeClosePrice;
            qDebug() << "marketCapitalization: " << marketCapitalization;
        }

    }

}
