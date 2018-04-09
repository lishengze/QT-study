#ifndef GENERATEPORTFOLIOFORM_H
#define GENERATEPORTFOLIOFORM_H

#include <QWidget>
#include <QFileInfo>
#include <QTimer>
#include <QMenu>
#include <QFileDialog>
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
    void initFileDir();
    void initExcelFileInfo();

    void initWidget();
    void initChoosePriceDate();

    void initProgramInfoTable();
    void initTableContextMenu();

    void setAccountTable();
    void setStrategyTable();
    void setBuySalePortfolioTable();

    bool isClosePriceValid(QMap<QString, double> secodePrice);
    bool isCapitalizationValid(double Capitalization);
    bool isStrategyWeightValid();

    QMap<QString, double> getClosePrice();
    QMap<QString, double> getClosePrice(QList<QString> secodeList, QString indexCode, QString timepType);

    double getCapitalization(double HS300Count);

    QMap<QString, double> getNewPortfolio(QMap<QString, double> strategyMap,
                                          QMap<QString, double> secodePrice,
                                          double marketCapitalization);

    QMap<QString, double> unitePortfolio(QMap<QString, double> desMap,
                                         QMap<QString, double> oriMap);

    QMap<QString, double> intPorfolio(QMap<QString, double> oriPortfolio);

    QList<QMap<QString, double>> getBuySalePortfolio();
    void storeBuySalePortfolio();

    QMap<QString, double> getStrategyWeight();
    QMap<QString, QMap<QString, double>> getCurrStrategy(QMap<QString, double> strategyWeight);
    QList<QString> getCurrSecodeList(QMap<QString, QMap<QString, double>> allStrategyMap);
    QMap<QString, double> getCurStrategy();

    QString getStrategyWeightStringInfo(QMap<QString, double>  strategyWeight);
    QList<QString> getAccountStringInfo(QString accountFileName);

    void clearGeneInfo();
    ~GeneratePortfolioForm();

private slots:

    void on_account_table_clicked(const QModelIndex &index);

    void on_geneBuySalePortfolio_clicked();

    void on_chooseHistDate_editingFinished();

    void on_chooseRealTimeRatio_clicked(bool checked);

    void on_buySalePortfolio_table_doubleClicked(const QModelIndex &index);

    void on_strategy_table_doubleClicked(const QModelIndex &index);

    void on_account_table_doubleClicked(const QModelIndex &index);

    void show_accountTable_contextMenu(QPoint);

    void show_strategyTable_contextMenu(QPoint);

    void show_portfolioTable_contextMenu(QPoint);

    void refresh_account_table();

    void refresh_strategy_table();

    void refresh_portfolio_table();

    void delete_account_file();

    void delete_strategy_file();

    void delete_portfolio_file();

    void on_choose_account_button_clicked();

    void on_choose_strategy_button_clicked();

    void on_choose_portfolio_button_clicked();

private:
    Ui::GeneratePortfolioForm *            ui;
    Database*                              m_database;
    QString                                m_indexCode;

    QString                                m_nativeFileName;
    QString                                m_mainDir;
    QString                                m_accountFatherDir;
    QString                                m_accountPortfolioDir;
    QString                                m_accountCurrDir;
    QString                                m_strategyDir;
    QString                                m_currAccountFileName;

    QList<QFileInfo>                       m_accountDirList;
    QList<QFileInfo>                       m_currAccountFileList;
    QList<QFileInfo>                       m_strategyFileList;
    QList<QFileInfo>                       m_buySalePortfolioFileList;

    bool                                   m_hasChooseAccount;
    QList<QString>                         m_currSecodeList;
    QMap<QString, double>                  m_currStrategyMap;
    QMap<QString, int>                     m_currAccountMap;
    QMap<QString, double>                  m_currPriceMap;
    QString                                m_priceTime;

    double                                 m_currMarketCapitalization;
    QMap<QString, double>                  m_strategyWeight;
    QMap<QString, double>                  m_currSecodeClosePrice;
    QMap<QString, double>                  m_currNewPortfolio;
    QMap<QString, double>                  m_currBuyPortfolio;
    QMap<QString, double>                  m_currSalePortfolio;
    QMap<QString, QMap<QString, double>>   m_allStrategy;

    QMap<QString, QString>                 m_portfolioAmount;

    int                                    m_strategyExcelFileColNumb;
    QList<int>                             m_accoutnExcelFileColNumbList;
    QList<QList<int>>                      m_accountExcelFileUsefulColNumbList;
};


//    void on_refreshAllTable_clicked();


#endif // GENERATEPORTFOLIOFORM_H
