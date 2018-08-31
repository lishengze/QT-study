/********************************************************************************
** Form generated from reading UI file 'updaterealtimedataform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPDATEREALTIMEDATAFORM_H
#define UI_UPDATEREALTIMEDATAFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UpdateRealtimeDataForm
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QComboBox *dataSource_ComboBox;
    QPushButton *startDownloadData_Button;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QTableView *regularMsgTableView;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QTableView *errorMsgTableView;

    void setupUi(QWidget *UpdateRealtimeDataForm)
    {
        if (UpdateRealtimeDataForm->objectName().isEmpty())
            UpdateRealtimeDataForm->setObjectName(QStringLiteral("UpdateRealtimeDataForm"));
        UpdateRealtimeDataForm->resize(1083, 780);
        gridLayout = new QGridLayout(UpdateRealtimeDataForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(UpdateRealtimeDataForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 40));
        widget = new QWidget(groupBox);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 0, 531, 41));
        horizontalLayout_3 = new QHBoxLayout(widget);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        label->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_3->addWidget(label);

        dataSource_ComboBox = new QComboBox(widget);
        dataSource_ComboBox->setObjectName(QStringLiteral("dataSource_ComboBox"));
        dataSource_ComboBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_3->addWidget(dataSource_ComboBox);

        startDownloadData_Button = new QPushButton(widget);
        startDownloadData_Button->setObjectName(QStringLiteral("startDownloadData_Button"));
        startDownloadData_Button->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_3->addWidget(startDownloadData_Button);


        horizontalLayout->addWidget(groupBox);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_3 = new QLabel(UpdateRealtimeDataForm);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_3);

        regularMsgTableView = new QTableView(UpdateRealtimeDataForm);
        regularMsgTableView->setObjectName(QStringLiteral("regularMsgTableView"));

        verticalLayout->addWidget(regularMsgTableView);


        horizontalLayout_2->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_4 = new QLabel(UpdateRealtimeDataForm);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_4);

        errorMsgTableView = new QTableView(UpdateRealtimeDataForm);
        errorMsgTableView->setObjectName(QStringLiteral("errorMsgTableView"));

        verticalLayout_2->addWidget(errorMsgTableView);


        horizontalLayout_2->addLayout(verticalLayout_2);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        retranslateUi(UpdateRealtimeDataForm);

        QMetaObject::connectSlotsByName(UpdateRealtimeDataForm);
    } // setupUi

    void retranslateUi(QWidget *UpdateRealtimeDataForm)
    {
        UpdateRealtimeDataForm->setWindowTitle(QApplication::translate("UpdateRealtimeDataForm", "Form", Q_NULLPTR));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("UpdateRealtimeDataForm", "\351\200\211\346\213\251\346\225\260\346\215\256\346\272\220:", Q_NULLPTR));
        startDownloadData_Button->setText(QApplication::translate("UpdateRealtimeDataForm", "\345\274\200\345\247\213\346\233\264\346\226\260\346\225\260\346\215\256", Q_NULLPTR));
        label_3->setText(QApplication::translate("UpdateRealtimeDataForm", "\345\256\236\346\227\266\344\277\241\346\201\257", Q_NULLPTR));
        label_4->setText(QApplication::translate("UpdateRealtimeDataForm", "\351\224\231\350\257\257\344\277\241\346\201\257", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class UpdateRealtimeDataForm: public Ui_UpdateRealtimeDataForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATEREALTIMEDATAFORM_H
