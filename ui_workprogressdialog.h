/********************************************************************************
** Form generated from reading UI file 'workprogressdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORKPROGRESSDIALOG_H
#define UI_WORKPROGRESSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_WorkProgressDialog
{
public:
    QLabel *info_label;
    QProgressBar *wrok_progressBar;
    QPushButton *stop_pushButton;

    void setupUi(QDialog *WorkProgressDialog)
    {
        if (WorkProgressDialog->objectName().isEmpty())
            WorkProgressDialog->setObjectName(QStringLiteral("WorkProgressDialog"));
        WorkProgressDialog->resize(572, 183);
        info_label = new QLabel(WorkProgressDialog);
        info_label->setObjectName(QStringLiteral("info_label"));
        info_label->setGeometry(QRect(20, 10, 541, 31));
        wrok_progressBar = new QProgressBar(WorkProgressDialog);
        wrok_progressBar->setObjectName(QStringLiteral("wrok_progressBar"));
        wrok_progressBar->setGeometry(QRect(170, 70, 221, 41));
        wrok_progressBar->setValue(24);
        stop_pushButton = new QPushButton(WorkProgressDialog);
        stop_pushButton->setObjectName(QStringLiteral("stop_pushButton"));
        stop_pushButton->setEnabled(true);
        stop_pushButton->setGeometry(QRect(480, 150, 81, 21));

        retranslateUi(WorkProgressDialog);

        QMetaObject::connectSlotsByName(WorkProgressDialog);
    } // setupUi

    void retranslateUi(QDialog *WorkProgressDialog)
    {
        WorkProgressDialog->setWindowTitle(QApplication::translate("WorkProgressDialog", "Dialog", Q_NULLPTR));
        info_label->setText(QApplication::translate("WorkProgressDialog", "WorkInfo", Q_NULLPTR));
        stop_pushButton->setText(QApplication::translate("WorkProgressDialog", "\345\201\234\346\255\242\346\217\220\345\217\226", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class WorkProgressDialog: public Ui_WorkProgressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORKPROGRESSDIALOG_H
