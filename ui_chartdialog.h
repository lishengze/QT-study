/********************************************************************************
** Form generated from reading UI file 'chartdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTDIALOG_H
#define UI_CHARTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_chartDialog
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QPushButton *changeTheme;

    void setupUi(QDialog *chartDialog)
    {
        if (chartDialog->objectName().isEmpty())
            chartDialog->setObjectName(QStringLiteral("chartDialog"));
        chartDialog->resize(1035, 750);
        chartDialog->setMinimumSize(QSize(500, 500));
        gridLayoutWidget = new QWidget(chartDialog);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(9, 9, 1021, 671));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        changeTheme = new QPushButton(chartDialog);
        changeTheme->setObjectName(QStringLiteral("changeTheme"));
        changeTheme->setGeometry(QRect(10, 710, 101, 23));

        retranslateUi(chartDialog);

        QMetaObject::connectSlotsByName(chartDialog);
    } // setupUi

    void retranslateUi(QDialog *chartDialog)
    {
        chartDialog->setWindowTitle(QApplication::translate("chartDialog", "Dialog", Q_NULLPTR));
        changeTheme->setText(QApplication::translate("chartDialog", "changeTheme", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class chartDialog: public Ui_chartDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTDIALOG_H
