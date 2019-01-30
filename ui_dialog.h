/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QPushButton *startProcessData;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(569, 467);
        startProcessData = new QPushButton(Dialog);
        startProcessData->setObjectName(QStringLiteral("startProcessData"));
        startProcessData->setGeometry(QRect(50, 50, 131, 41));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", Q_NULLPTR));
        startProcessData->setText(QApplication::translate("Dialog", "\345\274\200\345\247\213\350\257\273\345\217\226\346\225\260\346\215\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
