#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include "generateportfolioform.h"
#include "ui_generateportfolioform.h"
#include "toolfunc.h"

GeneratePortfolioForm::GeneratePortfolioForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneratePortfolioForm)
{
    ui->setupUi(this);

    initCommnData();
    initAccountNameList();
    initAccountCombox();
    setAccountTable();

    initStrategyTable();

    setBuySalePortfolioTable();
}

GeneratePortfolioForm::~GeneratePortfolioForm()
{
    delete ui;
}

void GeneratePortfolioForm::initCommnData() {
//    m_accountFatherDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/");
//    m_strategyDir = QString::fromLocal8Bit("//192.168.211.182/it程序设计/策略/");
    m_accountFatherDir = "//192.168.211.182/it程序设计/";
    m_strategyDir = "//192.168.211.182/it程序设计/策略/";
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

void GeneratePortfolioForm::initAccountCombox() {
    QStringList accountNameList;
    for (int i = 0; i < m_accountDirList.size(); ++i) {
        accountNameList.append(m_accountDirList[i].fileName());
    }
    ui->chooseAccount_ComboBox->addItems(accountNameList);
    ui->chooseAccount_ComboBox->setCurrentIndex(0);
}

void GeneratePortfolioForm::setAccountTable() {
    QFileInfo currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
    m_currAccountFileList = getExcelFileInfo(currAccountInfo.filePath());
    QStandardItemModel* strategyItemModel = getStandardItemModel("账户信息", m_currAccountFileList);
    ui->account_table->setModel(strategyItemModel);
    ui->account_table->setColumnWidth (0, 300);
    ui->account_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->account_table->setShowGrid (false);
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

}

void GeneratePortfolioForm::setBuySalePortfolioTable() {
    m_currAccountInfo = m_accountDirList[ui->chooseAccount_ComboBox->currentIndex()];
    ui->buySalePortfolio_Label->setText(m_currAccountInfo.fileName() + "买卖组合信息: ");

    QString curBuySalePortfolioDir = m_currAccountInfo.filePath() + "/买卖组合/";
    m_buySalePortfolioFileList = getExcelFileInfo(curBuySalePortfolioDir);
    QStandardItemModel* strategyItemModel = getStandardItemModel("买卖组合", m_buySalePortfolioFileList);
    ui->buySalePortfolio_table->setModel(strategyItemModel);
    ui->buySalePortfolio_table->setColumnWidth (0, 300);
    ui->buySalePortfolio_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->buySalePortfolio_table->setShowGrid (false);
}

void GeneratePortfolioForm::on_chooseAccount_ComboBox_currentIndexChanged(int index)
{
    index;
    setAccountTable();
    setBuySalePortfolioTable();
}
