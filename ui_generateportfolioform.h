/********************************************************************************
** Form generated from reading UI file 'generateportfolioform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERATEPORTFOLIOFORM_H
#define UI_GENERATEPORTFOLIOFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeneratePortfolioForm
{
public:
    QGridLayout *gridLayout_4;
    QGridLayout *gridLayout_3;
    QRadioButton *chooseRealTimeRatio;
    QLabel *label;
    QDateEdit *chooseHistDate;
    QTableView *account_table;
    QPushButton *choose_account_button;
    QLabel *label_2;
    QComboBox *datasource_comboBox;
    QGridLayout *gridLayout_2;
    QTableView *strategy_table;
    QPushButton *geneBuySalePortfolio;
    QLabel *strategy_Label;
    QPushButton *choose_strategy_button;
    QGridLayout *gridLayout;
    QTableView *buySalePortfolio_table;
    QLabel *buySalePortfolio_Label;
    QPushButton *choose_portfolio_button;
    QVBoxLayout *verticalLayout;
    QLabel *programInfo_Label;
    QTableView *programInfo_Table;

    void setupUi(QWidget *GeneratePortfolioForm)
    {
        if (GeneratePortfolioForm->objectName().isEmpty())
            GeneratePortfolioForm->setObjectName(QStringLiteral("GeneratePortfolioForm"));
        GeneratePortfolioForm->resize(1166, 871);
        gridLayout_4 = new QGridLayout(GeneratePortfolioForm);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        chooseRealTimeRatio = new QRadioButton(GeneratePortfolioForm);
        chooseRealTimeRatio->setObjectName(QStringLiteral("chooseRealTimeRatio"));

        gridLayout_3->addWidget(chooseRealTimeRatio, 3, 1, 1, 1);

        label = new QLabel(GeneratePortfolioForm);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 3, 0, 1, 1);

        chooseHistDate = new QDateEdit(GeneratePortfolioForm);
        chooseHistDate->setObjectName(QStringLiteral("chooseHistDate"));
        chooseHistDate->setMaximumSize(QSize(16777215, 20));

        gridLayout_3->addWidget(chooseHistDate, 3, 2, 1, 1);

        account_table = new QTableView(GeneratePortfolioForm);
        account_table->setObjectName(QStringLiteral("account_table"));

        gridLayout_3->addWidget(account_table, 6, 0, 1, 3);

        choose_account_button = new QPushButton(GeneratePortfolioForm);
        choose_account_button->setObjectName(QStringLiteral("choose_account_button"));
        choose_account_button->setMaximumSize(QSize(120, 16777215));

        gridLayout_3->addWidget(choose_account_button, 2, 0, 1, 1);

        label_2 = new QLabel(GeneratePortfolioForm);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_3->addWidget(label_2, 2, 1, 1, 1);

        datasource_comboBox = new QComboBox(GeneratePortfolioForm);
        datasource_comboBox->setObjectName(QStringLiteral("datasource_comboBox"));

        gridLayout_3->addWidget(datasource_comboBox, 2, 2, 1, 1);


        gridLayout_4->addLayout(gridLayout_3, 0, 0, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        strategy_table = new QTableView(GeneratePortfolioForm);
        strategy_table->setObjectName(QStringLiteral("strategy_table"));

        gridLayout_2->addWidget(strategy_table, 2, 0, 1, 3);

        geneBuySalePortfolio = new QPushButton(GeneratePortfolioForm);
        geneBuySalePortfolio->setObjectName(QStringLiteral("geneBuySalePortfolio"));
        geneBuySalePortfolio->setMinimumSize(QSize(120, 0));
        geneBuySalePortfolio->setMaximumSize(QSize(120, 16777215));

        gridLayout_2->addWidget(geneBuySalePortfolio, 0, 1, 1, 1);

        strategy_Label = new QLabel(GeneratePortfolioForm);
        strategy_Label->setObjectName(QStringLiteral("strategy_Label"));
        strategy_Label->setMinimumSize(QSize(0, 20));
        strategy_Label->setMaximumSize(QSize(120, 16777215));
        strategy_Label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(strategy_Label, 1, 1, 1, 1);

        choose_strategy_button = new QPushButton(GeneratePortfolioForm);
        choose_strategy_button->setObjectName(QStringLiteral("choose_strategy_button"));
        choose_strategy_button->setMinimumSize(QSize(120, 0));
        choose_strategy_button->setMaximumSize(QSize(120, 16777215));

        gridLayout_2->addWidget(choose_strategy_button, 0, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout_2, 0, 1, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buySalePortfolio_table = new QTableView(GeneratePortfolioForm);
        buySalePortfolio_table->setObjectName(QStringLiteral("buySalePortfolio_table"));

        gridLayout->addWidget(buySalePortfolio_table, 3, 0, 1, 2);

        buySalePortfolio_Label = new QLabel(GeneratePortfolioForm);
        buySalePortfolio_Label->setObjectName(QStringLiteral("buySalePortfolio_Label"));
        buySalePortfolio_Label->setMinimumSize(QSize(0, 20));
        buySalePortfolio_Label->setMaximumSize(QSize(16777215, 20));
        buySalePortfolio_Label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(buySalePortfolio_Label, 2, 0, 1, 1);

        choose_portfolio_button = new QPushButton(GeneratePortfolioForm);
        choose_portfolio_button->setObjectName(QStringLiteral("choose_portfolio_button"));
        choose_portfolio_button->setMaximumSize(QSize(120, 16777215));

        gridLayout->addWidget(choose_portfolio_button, 1, 0, 1, 1);


        gridLayout_4->addLayout(gridLayout, 0, 2, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        programInfo_Label = new QLabel(GeneratePortfolioForm);
        programInfo_Label->setObjectName(QStringLiteral("programInfo_Label"));
        programInfo_Label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(programInfo_Label);

        programInfo_Table = new QTableView(GeneratePortfolioForm);
        programInfo_Table->setObjectName(QStringLiteral("programInfo_Table"));

        verticalLayout->addWidget(programInfo_Table);


        gridLayout_4->addLayout(verticalLayout, 1, 0, 1, 3);


        retranslateUi(GeneratePortfolioForm);

        QMetaObject::connectSlotsByName(GeneratePortfolioForm);
    } // setupUi

    void retranslateUi(QWidget *GeneratePortfolioForm)
    {
        GeneratePortfolioForm->setWindowTitle(QApplication::translate("GeneratePortfolioForm", "Form", Q_NULLPTR));
        chooseRealTimeRatio->setText(QApplication::translate("GeneratePortfolioForm", "\345\256\236\346\227\266", Q_NULLPTR));
        label->setText(QApplication::translate("GeneratePortfolioForm", "\351\200\211\346\213\251\346\210\220\344\272\244\346\227\266\351\227\264: ", Q_NULLPTR));
        choose_account_button->setText(QApplication::translate("GeneratePortfolioForm", "\351\200\211\346\213\251\350\264\246\346\210\267\346\226\207\344\273\266\345\244\271", Q_NULLPTR));
        label_2->setText(QApplication::translate("GeneratePortfolioForm", "\351\200\211\346\213\251\346\225\260\346\215\256\346\272\220:", Q_NULLPTR));
        geneBuySalePortfolio->setText(QApplication::translate("GeneratePortfolioForm", "\347\224\237\346\210\220\344\271\260\345\215\226\347\273\204\345\220\210", Q_NULLPTR));
        strategy_Label->setText(QApplication::translate("GeneratePortfolioForm", "\347\255\226\347\225\245\344\277\241\346\201\257:", Q_NULLPTR));
        choose_strategy_button->setText(QApplication::translate("GeneratePortfolioForm", "\351\200\211\346\213\251\347\255\226\347\225\245\346\226\207\344\273\266\345\244\271", Q_NULLPTR));
        buySalePortfolio_Label->setText(QApplication::translate("GeneratePortfolioForm", "\345\257\271\345\272\224\350\264\246\346\210\267\344\271\260\345\215\226\347\273\204\345\220\210\357\274\232", Q_NULLPTR));
        choose_portfolio_button->setText(QApplication::translate("GeneratePortfolioForm", "\351\200\211\346\213\251\344\271\260\345\215\226\347\273\204\345\220\210\346\226\207\344\273\266\345\244\271", Q_NULLPTR));
        programInfo_Label->setText(QApplication::translate("GeneratePortfolioForm", "------ \347\250\213\345\272\217\350\277\220\350\241\214\344\277\241\346\201\257 -----", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GeneratePortfolioForm: public Ui_GeneratePortfolioForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERATEPORTFOLIOFORM_H
