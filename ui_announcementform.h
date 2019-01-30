/********************************************************************************
** Form generated from reading UI file 'announcementform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANNOUNCEMENTFORM_H
#define UI_ANNOUNCEMENTFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AnnouncementForm
{
public:
    QGridLayout *gridLayout;
    QTableView *AnnouncementTableView;
    QPushButton *refreshTable;

    void setupUi(QWidget *AnnouncementForm)
    {
        if (AnnouncementForm->objectName().isEmpty())
            AnnouncementForm->setObjectName(QStringLiteral("AnnouncementForm"));
        AnnouncementForm->resize(1075, 716);
        gridLayout = new QGridLayout(AnnouncementForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        AnnouncementTableView = new QTableView(AnnouncementForm);
        AnnouncementTableView->setObjectName(QStringLiteral("AnnouncementTableView"));

        gridLayout->addWidget(AnnouncementTableView, 1, 0, 1, 1);

        refreshTable = new QPushButton(AnnouncementForm);
        refreshTable->setObjectName(QStringLiteral("refreshTable"));
        refreshTable->setMaximumSize(QSize(100, 16777215));

        gridLayout->addWidget(refreshTable, 0, 0, 1, 1);


        retranslateUi(AnnouncementForm);

        QMetaObject::connectSlotsByName(AnnouncementForm);
    } // setupUi

    void retranslateUi(QWidget *AnnouncementForm)
    {
        AnnouncementForm->setWindowTitle(QApplication::translate("AnnouncementForm", "Form", Q_NULLPTR));
        refreshTable->setText(QApplication::translate("AnnouncementForm", "\345\210\267\346\226\260", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class AnnouncementForm: public Ui_AnnouncementForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANNOUNCEMENTFORM_H
