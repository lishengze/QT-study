/********************************************************************************
** Form generated from reading UI file 'csschartformone.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSSCHARTFORMONE_H
#define UI_CSSCHARTFORMONE_H

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

class Ui_CSSChartFormOne
{
public:
    QGridLayout *gridLayout_2;
    QLabel *title_Label;
    QGridLayout *gridLayout;
    QGroupBox *groupBox;

    void setupUi(QWidget *CSSChartFormOne)
    {
        if (CSSChartFormOne->objectName().isEmpty())
            CSSChartFormOne->setObjectName(QStringLiteral("CSSChartFormOne"));
        CSSChartFormOne->resize(1154, 696);
        gridLayout_2 = new QGridLayout(CSSChartFormOne);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        title_Label = new QLabel(CSSChartFormOne);
        title_Label->setObjectName(QStringLiteral("title_Label"));
        title_Label->setMinimumSize(QSize(0, 30));
        title_Label->setMaximumSize(QSize(16777215, 30));
        title_Label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(title_Label, 0, 0, 1, 2);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));

        gridLayout_2->addLayout(gridLayout, 1, 0, 1, 1);

        groupBox = new QGroupBox(CSSChartFormOne);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(150, 600));
        groupBox->setMaximumSize(QSize(150, 16777215));
        groupBox->setAlignment(Qt::AlignCenter);
        groupBox->setFlat(true);

        gridLayout_2->addWidget(groupBox, 1, 1, 1, 1);


        retranslateUi(CSSChartFormOne);

        QMetaObject::connectSlotsByName(CSSChartFormOne);
    } // setupUi

    void retranslateUi(QWidget *CSSChartFormOne)
    {
        CSSChartFormOne->setWindowTitle(QApplication::translate("CSSChartFormOne", "Form", Q_NULLPTR));
        title_Label->setText(QApplication::translate("CSSChartFormOne", "TextLabel", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("CSSChartFormOne", "\345\261\236\346\200\247\345\200\274", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CSSChartFormOne: public Ui_CSSChartFormOne {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSSCHARTFORMONE_H
