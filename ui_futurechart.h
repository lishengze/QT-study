/********************************************************************************
** Form generated from reading UI file 'futurechart.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUTURECHART_H
#define UI_FUTURECHART_H

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

class Ui_FutureChart
{
public:
    QGridLayout *gridLayout_2;
    QLabel *title_label;
    QGridLayout *chart_gridLayout;
    QGroupBox *groupBox;
    QLabel *spread_label;
    QLabel *time_label;

    void setupUi(QWidget *FutureChart)
    {
        if (FutureChart->objectName().isEmpty())
            FutureChart->setObjectName(QStringLiteral("FutureChart"));
        FutureChart->resize(1123, 524);
        gridLayout_2 = new QGridLayout(FutureChart);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        title_label = new QLabel(FutureChart);
        title_label->setObjectName(QStringLiteral("title_label"));
        title_label->setMaximumSize(QSize(16777215, 60));
        title_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(title_label, 0, 0, 1, 2);

        chart_gridLayout = new QGridLayout();
        chart_gridLayout->setObjectName(QStringLiteral("chart_gridLayout"));

        gridLayout_2->addLayout(chart_gridLayout, 1, 0, 1, 1);

        groupBox = new QGroupBox(FutureChart);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(200, 0));
        groupBox->setMaximumSize(QSize(200, 16777215));
        groupBox->setAlignment(Qt::AlignCenter);
        groupBox->setFlat(true);
        spread_label = new QLabel(groupBox);
        spread_label->setObjectName(QStringLiteral("spread_label"));
        spread_label->setGeometry(QRect(10, 70, 181, 21));
        time_label = new QLabel(groupBox);
        time_label->setObjectName(QStringLiteral("time_label"));
        time_label->setGeometry(QRect(10, 30, 181, 21));

        gridLayout_2->addWidget(groupBox, 1, 1, 1, 1);


        retranslateUi(FutureChart);

        QMetaObject::connectSlotsByName(FutureChart);
    } // setupUi

    void retranslateUi(QWidget *FutureChart)
    {
        FutureChart->setWindowTitle(QApplication::translate("FutureChart", "Form", Q_NULLPTR));
        title_label->setText(QApplication::translate("FutureChart", "Title", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("FutureChart", "\345\261\236\346\200\247\345\200\274", Q_NULLPTR));
        spread_label->setText(QApplication::translate("FutureChart", "\345\237\272\345\267\256:", Q_NULLPTR));
        time_label->setText(QApplication::translate("FutureChart", "\346\227\266\351\227\264:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FutureChart: public Ui_FutureChart {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUTURECHART_H
