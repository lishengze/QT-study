#ifndef GENERATEPORTFOLIOFORM_H
#define GENERATEPORTFOLIOFORM_H

#include <QWidget>
#include <QFileInfo>
#include <QTimer>
#include "database.h"
#include "excel.h"

namespace Ui {
class GeneratePortfolioForm;
}

class GeneratePortfolioForm : public QWidget
{
    Q_OBJECT

public:
    explicit GeneratePortfolioForm(QWidget *parent = 0);
    void initCommnData();
    void initExcelFileInfo();
    void initAccountNameList();

    void initWidget();
    void initChoosePriceDate();
    void initAccountCombox();
    void initAccountTable();
    void initSalePortfolioTable();
    void initStrategyTable();
    void initProgramInfoTable();

    void initAllStrategy();

    void setAccountTable();
    void setStrategyTable();
    void setBuySalePortfolioTable();

    bool isClosePriceValid(QMap<QString, double> secodePrice);
    bool isCapitalizationValid(double Capitalization);
    bool isStrategyWeightValid();

    QMap<QString, double> getClosePrice();
    double getCapitalization();

    QMap<QString, int> getNewPortfolio();
    QList<QMap<QString, int>> getBuySalePortfolio();
    void storeBuySalePortfolio();

    QMap<QString, double> getStrategyWeight();
    QMap<QString, double> getCurStrategy();

    void clearGeneInfo();
    ~GeneratePortfolioForm();

private slots:
    void on_chooseAccount_ComboBox_currentIndexChanged(int index);

    void on_account_table_clicked(const QModelIndex &index);

    void on_geneBuySalePortfolio_clicked();

    void on_chooseHistDate_editingFinished();

    void on_chooseRealTimeRatio_clicked(bool checked);

    void on_buySalePortfolio_table_doubleClicked(const QModelIndex &index);

    void on_account_table_doubleClicked(const QModelIndex &index);

    void on_refreshAllTable_clicked();

private:
    Ui::GeneratePortfolioForm *ui;
    QString m_mainDir;
    QString m_accountFatherDir;
    QString m_accountCurrDir;
    QString m_accountPortfolioDir;
    QString m_strategyDir;
    QFileInfo m_currAccountInfo;

    QList<QFileInfo> m_accountDirList;
    QList<QFileInfo> m_currAccountFileList;
    QList<QFileInfo> m_strategyFileList;   
    QList<QFileInfo> m_buySalePortfolioFileList;

    QFileInfo m_currStrategFileInfo;
    QFileInfo m_currAccountFileInfo;

    int m_strategyExcelFileColNumb;
    QList<int> m_accoutnExcelFileColNumbList;
    QList<QList<int>> m_accountExcelFileUsefulColNumbList;

    QMap<QString, double> m_currStrategyMap;
    QMap<QString, int> m_currAccountMap;
    QMap<QString, double> m_currPriceMap;
    QString m_priceTime;

    double m_currMarketCapitalization;
    QMap<QString, double> m_strategyWeight;
    QMap<QString, double> m_currSecodeClosePrice;
    QMap<QString, int> m_currNewPortfolio;
    QMap<QString, int> m_currBuyPortfolio;
    QMap<QString, int> m_currSalePortfolio;
    QMap<QString, QMap<QString, double>> m_allStrategy;

    Database* m_database;
    Excel m_excel;
    bool m_isInit;

    QTimer m_timer;
    int m_refreshTime;

};

#endif // GENERATEPORTFOLIOFORM_H
