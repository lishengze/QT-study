/********************************************************************************
** Form generated from reading UI file 'choosesecodewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHOOSESECODEWINDOW_H
#define UI_CHOOSESECODEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChooseSecodeWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QTreeView *treeView;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLabel *label_3;
    QTableView *candidate_tableView;
    QVBoxLayout *verticalLayout_3;
    QPushButton *chooseSecode_pushButton;
    QPushButton *chooseAllCode_pushButton;
    QPushButton *returnSecode_pushButton;
    QPushButton *returnAllCode_pushButton;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QLabel *label_5;
    QTableView *selected_tableView;
    QPushButton *sendCodeList_pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ChooseSecodeWindow)
    {
        if (ChooseSecodeWindow->objectName().isEmpty())
            ChooseSecodeWindow->setObjectName(QStringLiteral("ChooseSecodeWindow"));
        ChooseSecodeWindow->resize(949, 484);
        centralwidget = new QWidget(ChooseSecodeWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        treeView = new QTreeView(centralwidget);
        treeView->setObjectName(QStringLiteral("treeView"));

        verticalLayout->addWidget(treeView);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);


        verticalLayout_2->addLayout(horizontalLayout);

        candidate_tableView = new QTableView(centralwidget);
        candidate_tableView->setObjectName(QStringLiteral("candidate_tableView"));

        verticalLayout_2->addWidget(candidate_tableView);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        chooseSecode_pushButton = new QPushButton(centralwidget);
        chooseSecode_pushButton->setObjectName(QStringLiteral("chooseSecode_pushButton"));

        verticalLayout_3->addWidget(chooseSecode_pushButton);

        chooseAllCode_pushButton = new QPushButton(centralwidget);
        chooseAllCode_pushButton->setObjectName(QStringLiteral("chooseAllCode_pushButton"));

        verticalLayout_3->addWidget(chooseAllCode_pushButton);

        returnSecode_pushButton = new QPushButton(centralwidget);
        returnSecode_pushButton->setObjectName(QStringLiteral("returnSecode_pushButton"));

        verticalLayout_3->addWidget(returnSecode_pushButton);

        returnAllCode_pushButton = new QPushButton(centralwidget);
        returnAllCode_pushButton->setObjectName(QStringLiteral("returnAllCode_pushButton"));

        verticalLayout_3->addWidget(returnAllCode_pushButton);


        horizontalLayout_3->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);

        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_2->addWidget(label_5);


        verticalLayout_4->addLayout(horizontalLayout_2);

        selected_tableView = new QTableView(centralwidget);
        selected_tableView->setObjectName(QStringLiteral("selected_tableView"));

        verticalLayout_4->addWidget(selected_tableView);


        horizontalLayout_3->addLayout(verticalLayout_4);


        gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        sendCodeList_pushButton = new QPushButton(centralwidget);
        sendCodeList_pushButton->setObjectName(QStringLiteral("sendCodeList_pushButton"));
        sendCodeList_pushButton->setMaximumSize(QSize(60, 16777215));
        sendCodeList_pushButton->setLayoutDirection(Qt::LeftToRight);

        gridLayout->addWidget(sendCodeList_pushButton, 1, 0, 1, 1);

        ChooseSecodeWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ChooseSecodeWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 949, 23));
        ChooseSecodeWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ChooseSecodeWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        ChooseSecodeWindow->setStatusBar(statusbar);

        retranslateUi(ChooseSecodeWindow);

        QMetaObject::connectSlotsByName(ChooseSecodeWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ChooseSecodeWindow)
    {
        ChooseSecodeWindow->setWindowTitle(QApplication::translate("ChooseSecodeWindow", "MainWindow", Q_NULLPTR));
        label->setText(QApplication::translate("ChooseSecodeWindow", "\350\214\203\345\233\264", Q_NULLPTR));
        label_2->setText(QApplication::translate("ChooseSecodeWindow", "\345\276\205\351\200\211:", Q_NULLPTR));
        label_3->setText(QApplication::translate("ChooseSecodeWindow", "\346\200\273\346\225\260: 0", Q_NULLPTR));
        chooseSecode_pushButton->setText(QApplication::translate("ChooseSecodeWindow", "\351\200\211\345\217\226", Q_NULLPTR));
        chooseAllCode_pushButton->setText(QApplication::translate("ChooseSecodeWindow", "\345\205\250\351\200\211", Q_NULLPTR));
        returnSecode_pushButton->setText(QApplication::translate("ChooseSecodeWindow", "\345\210\240\351\231\244", Q_NULLPTR));
        returnAllCode_pushButton->setText(QApplication::translate("ChooseSecodeWindow", "\346\270\205\347\251\272", Q_NULLPTR));
        label_4->setText(QApplication::translate("ChooseSecodeWindow", "\351\200\211\344\270\255:", Q_NULLPTR));
        label_5->setText(QApplication::translate("ChooseSecodeWindow", "\346\200\273\346\225\260:", Q_NULLPTR));
        sendCodeList_pushButton->setText(QApplication::translate("ChooseSecodeWindow", "\347\241\256\345\256\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChooseSecodeWindow: public Ui_ChooseSecodeWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHOOSESECODEWINDOW_H
