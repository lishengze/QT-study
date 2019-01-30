/********************************************************************************
** Form generated from reading UI file 'chartform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTFORM_H
#define UI_CHARTFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartForm
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *Title_Label;
    QGroupBox *groupBox;
    QLabel *TimeLabel;
    QLabel *StrategyValue_Label;
    QLabel *VotRunover_Label;
    QLabel *DIFF_Label;
    QLabel *DEA_Label;
    QLabel *MACD_Label;
    QLabel *preCLoseSpreadValue_Label;

    void setupUi(QWidget *ChartForm)
    {
        if (ChartForm->objectName().isEmpty())
            ChartForm->setObjectName(QStringLiteral("ChartForm"));
        ChartForm->resize(1123, 806);
        ChartForm->setMinimumSize(QSize(100, 400));
        gridLayout_2 = new QGridLayout(ChartForm);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(10);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(-1, 0, 5, -1);

        gridLayout_2->addLayout(gridLayout, 2, 0, 1, 1);

        Title_Label = new QLabel(ChartForm);
        Title_Label->setObjectName(QStringLiteral("Title_Label"));
        Title_Label->setStyleSheet(QString::fromUtf8("color: rgb(85, 255, 255);\n"
"font: 75 14pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";"));
        Title_Label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(Title_Label, 0, 0, 1, 1);

        groupBox = new QGroupBox(ChartForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(200, 600));
        groupBox->setMaximumSize(QSize(100, 4000));
        groupBox->setMouseTracking(false);
        groupBox->setAlignment(Qt::AlignCenter);
        groupBox->setFlat(true);
        groupBox->setCheckable(false);
        TimeLabel = new QLabel(groupBox);
        TimeLabel->setObjectName(QStringLiteral("TimeLabel"));
        TimeLabel->setGeometry(QRect(10, 20, 180, 16));
        TimeLabel->setMinimumSize(QSize(180, 0));
        StrategyValue_Label = new QLabel(groupBox);
        StrategyValue_Label->setObjectName(QStringLiteral("StrategyValue_Label"));
        StrategyValue_Label->setGeometry(QRect(10, 80, 180, 16));
        StrategyValue_Label->setMinimumSize(QSize(180, 0));
        VotRunover_Label = new QLabel(groupBox);
        VotRunover_Label->setObjectName(QStringLiteral("VotRunover_Label"));
        VotRunover_Label->setGeometry(QRect(10, 110, 180, 16));
        VotRunover_Label->setMinimumSize(QSize(180, 0));
        DIFF_Label = new QLabel(groupBox);
        DIFF_Label->setObjectName(QStringLiteral("DIFF_Label"));
        DIFF_Label->setGeometry(QRect(10, 140, 180, 16));
        DIFF_Label->setMinimumSize(QSize(180, 0));
        DEA_Label = new QLabel(groupBox);
        DEA_Label->setObjectName(QStringLiteral("DEA_Label"));
        DEA_Label->setGeometry(QRect(10, 170, 180, 16));
        DEA_Label->setMinimumSize(QSize(180, 0));
        MACD_Label = new QLabel(groupBox);
        MACD_Label->setObjectName(QStringLiteral("MACD_Label"));
        MACD_Label->setGeometry(QRect(10, 200, 180, 16));
        MACD_Label->setMinimumSize(QSize(180, 0));
        preCLoseSpreadValue_Label = new QLabel(groupBox);
        preCLoseSpreadValue_Label->setObjectName(QStringLiteral("preCLoseSpreadValue_Label"));
        preCLoseSpreadValue_Label->setGeometry(QRect(10, 50, 180, 16));
        preCLoseSpreadValue_Label->setMinimumSize(QSize(180, 0));

        gridLayout_2->addWidget(groupBox, 2, 2, 1, 1);


        retranslateUi(ChartForm);

        QMetaObject::connectSlotsByName(ChartForm);
    } // setupUi

    void retranslateUi(QWidget *ChartForm)
    {
        ChartForm->setWindowTitle(QApplication::translate("ChartForm", "Form", Q_NULLPTR));
        Title_Label->setText(QApplication::translate("ChartForm", "TextLabel", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("ChartForm", "\345\261\236\346\200\247\345\200\274\357\274\232", Q_NULLPTR));
        TimeLabel->setText(QApplication::translate("ChartForm", "\346\227\266\351\227\264:", Q_NULLPTR));
        StrategyValue_Label->setText(QApplication::translate("ChartForm", "\347\202\271\345\267\256:", Q_NULLPTR));
        VotRunover_Label->setText(QApplication::translate("ChartForm", "\346\210\220\344\272\244\351\242\235:", Q_NULLPTR));
        DIFF_Label->setText(QApplication::translate("ChartForm", "DIFF:", Q_NULLPTR));
        DEA_Label->setText(QApplication::translate("ChartForm", "DEA:", Q_NULLPTR));
        MACD_Label->setText(QApplication::translate("ChartForm", "MACD:", Q_NULLPTR));
        preCLoseSpreadValue_Label->setText(QApplication::translate("ChartForm", "\346\230\250\346\227\245\347\202\271\345\267\256:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChartForm: public Ui_ChartForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTFORM_H
