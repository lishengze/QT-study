/********************************************************************************
** Form generated from reading UI file 'extractdatawindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTRACTDATAWINDOW_H
#define UI_EXTRACTDATAWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExtractDataWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *chooseValue_comboBox;
    QPushButton *chooseDir_pushButton;
    QPushButton *startExtractData_pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ExtractDataWindow)
    {
        if (ExtractDataWindow->objectName().isEmpty())
            ExtractDataWindow->setObjectName(QStringLiteral("ExtractDataWindow"));
        ExtractDataWindow->resize(618, 100);
        centralwidget = new QWidget(ExtractDataWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setMaximumSize(QSize(60, 16777215));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        chooseValue_comboBox = new QComboBox(centralwidget);
        chooseValue_comboBox->setObjectName(QStringLiteral("chooseValue_comboBox"));
        chooseValue_comboBox->setMaximumSize(QSize(100, 16777215));

        gridLayout->addWidget(chooseValue_comboBox, 0, 1, 1, 1);

        chooseDir_pushButton = new QPushButton(centralwidget);
        chooseDir_pushButton->setObjectName(QStringLiteral("chooseDir_pushButton"));
        chooseDir_pushButton->setMaximumSize(QSize(200, 16777215));

        gridLayout->addWidget(chooseDir_pushButton, 0, 2, 1, 1);

        startExtractData_pushButton = new QPushButton(centralwidget);
        startExtractData_pushButton->setObjectName(QStringLiteral("startExtractData_pushButton"));
        startExtractData_pushButton->setMaximumSize(QSize(150, 16777215));

        gridLayout->addWidget(startExtractData_pushButton, 0, 3, 1, 1);

        ExtractDataWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ExtractDataWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 618, 23));
        ExtractDataWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ExtractDataWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        ExtractDataWindow->setStatusBar(statusbar);

        retranslateUi(ExtractDataWindow);

        QMetaObject::connectSlotsByName(ExtractDataWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ExtractDataWindow)
    {
        ExtractDataWindow->setWindowTitle(QApplication::translate("ExtractDataWindow", "MainWindow", Q_NULLPTR));
        label->setText(QApplication::translate("ExtractDataWindow", "\351\200\211\346\213\251\346\214\207\346\240\207:", Q_NULLPTR));
        chooseDir_pushButton->setText(QApplication::translate("ExtractDataWindow", "\351\200\211\346\213\251\346\217\220\345\217\226\345\220\216\346\225\260\346\215\256\345\255\230\345\202\250\347\232\204\346\226\207\344\273\266\345\244\271", Q_NULLPTR));
        startExtractData_pushButton->setText(QApplication::translate("ExtractDataWindow", "\345\274\200\345\247\213\346\217\220\345\217\226\346\225\260\346\215\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ExtractDataWindow: public Ui_ExtractDataWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTRACTDATAWINDOW_H
