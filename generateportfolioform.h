#ifndef GENERATEPORTFOLIOFORM_H
#define GENERATEPORTFOLIOFORM_H

#include <QWidget>
#include <QFileInfo>
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
    void initAccountNameList();
    void initAccountCombox();
    void setAccountTable();
    void initStrategyTable();
    void initProgramInfoTable();
    void setBuySalePortfolioTable();

    ~GeneratePortfolioForm();

private slots:
    void on_chooseAccount_ComboBox_currentIndexChanged(int index);

private:
    Ui::GeneratePortfolioForm *ui;

    QString m_accountFatherDir;
    QString m_accountCurrDir;
    QString m_accountPortfolioDir;
    QString m_strategyDir;
    QFileInfo m_currAccountInfo;

    QList<QFileInfo> m_accountDirList;
    QList<QFileInfo> m_currAccountFileList;
    QList<QFileInfo> m_strategyFileList;
    QList<QFileInfo> m_buySalePortfolioFileList;
};

#endif // GENERATEPORTFOLIOFORM_H
