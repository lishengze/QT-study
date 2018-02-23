#ifndef GENERATEPORTFOLIOFORM_H
#define GENERATEPORTFOLIOFORM_H

#include <QWidget>
#include <QFileInfo>
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

    void setAccountTable();
    void setBuySalePortfolioTable();

    QMap<QString, double> getClosePrice();
    double getCapitalization();

    QMap<QString, double> getNewPortfolio();
    QList<QMap<QString, double>> getBuySalePortfolio();
    void storeBuySalePortfolio();

    ~GeneratePortfolioForm();

private slots:
    void on_chooseAccount_ComboBox_currentIndexChanged(int index);

    void on_strategy_table_clicked(const QModelIndex &index);

    void on_account_table_clicked(const QModelIndex &index);

    void on_geneBuySalePortfolio_clicked();

    void on_chooseHistDate_editingFinished();

    void on_chooseRealTimeRatio_clicked(bool checked);

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

    Database* m_database;
    Excel m_excel;
    bool m_isInit;
};

#endif // GENERATEPORTFOLIOFORM_H
