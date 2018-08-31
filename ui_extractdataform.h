/********************************************************************************
** Form generated from reading UI file 'extractdataform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTRACTDATAFORM_H
#define UI_EXTRACTDATAFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ExtractDataForm
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QComboBox *chooseDataSource_comboBox;
    QGridLayout *gridLayout_2;
    QPushButton *chooseDesFileDir_pushButton;
    QPushButton *chooseOriFileDir_pushButton;
    QTableView *desFile_tableView;
    QTableView *oriFile_tableView;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *dataType_comboBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_4;
    QComboBox *keyValue_comboBox;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QDateEdit *startDate_dateEdit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QDateEdit *endDate_dateEdit;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *chooseSecodeListFromTable_pushButton;
    QPushButton *chooseSecodeListFromExcel_pushButton;
    QPushButton *extractMarketData_pushButton;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QComboBox *indexCode_comboBox;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QDateEdit *indexStartDate_dateEdit;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_9;
    QDateEdit *indexEndDate_dateEdit;
    QPushButton *extractWeightData_pushButton;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QComboBox *industry_comboBox;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_12;
    QDateEdit *industryEndDate_dateEdit;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_11;
    QDateEdit *industryStartDate_dateEdit;
    QPushButton *extractIndustryData_pushButton;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_14;
    QComboBox *fundamentValue_comboBox;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_16;
    QDateEdit *fundamentStart_dateEdit;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_17;
    QDateEdit *fundamentEnd_dateEdit;
    QHBoxLayout *horizontalLayout_18;
    QPushButton *chooseSecodeList_pushButton;
    QPushButton *importSecodeList_pushButton;
    QPushButton *startExtractFundament_pushButton;
    QVBoxLayout *verticalLayout;
    QLabel *label_5;
    QTableView *programInfo_Table;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ExtractDataForm)
    {
        if (ExtractDataForm->objectName().isEmpty())
            ExtractDataForm->setObjectName(QStringLiteral("ExtractDataForm"));
        ExtractDataForm->resize(1097, 770);
        centralwidget = new QWidget(ExtractDataForm);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_6 = new QGridLayout(centralwidget);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_5->addWidget(label_6);

        chooseDataSource_comboBox = new QComboBox(centralwidget);
        chooseDataSource_comboBox->setObjectName(QStringLiteral("chooseDataSource_comboBox"));

        horizontalLayout_5->addWidget(chooseDataSource_comboBox);


        gridLayout_6->addLayout(horizontalLayout_5, 0, 0, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        chooseDesFileDir_pushButton = new QPushButton(centralwidget);
        chooseDesFileDir_pushButton->setObjectName(QStringLiteral("chooseDesFileDir_pushButton"));

        gridLayout_2->addWidget(chooseDesFileDir_pushButton, 0, 1, 1, 1);

        chooseOriFileDir_pushButton = new QPushButton(centralwidget);
        chooseOriFileDir_pushButton->setObjectName(QStringLiteral("chooseOriFileDir_pushButton"));

        gridLayout_2->addWidget(chooseOriFileDir_pushButton, 0, 0, 1, 1);

        desFile_tableView = new QTableView(centralwidget);
        desFile_tableView->setObjectName(QStringLiteral("desFile_tableView"));

        gridLayout_2->addWidget(desFile_tableView, 1, 1, 1, 1);

        oriFile_tableView = new QTableView(centralwidget);
        oriFile_tableView->setObjectName(QStringLiteral("oriFile_tableView"));

        gridLayout_2->addWidget(oriFile_tableView, 1, 0, 1, 1);


        gridLayout_6->addLayout(gridLayout_2, 0, 1, 4, 1);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(280, 150));
        groupBox->setMaximumSize(QSize(16777215, 270));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        dataType_comboBox = new QComboBox(groupBox);
        dataType_comboBox->setObjectName(QStringLiteral("dataType_comboBox"));

        horizontalLayout->addWidget(dataType_comboBox);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);

        keyValue_comboBox = new QComboBox(groupBox);
        keyValue_comboBox->setObjectName(QStringLiteral("keyValue_comboBox"));

        horizontalLayout_2->addWidget(keyValue_comboBox);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        startDate_dateEdit = new QDateEdit(groupBox);
        startDate_dateEdit->setObjectName(QStringLiteral("startDate_dateEdit"));

        horizontalLayout_3->addWidget(startDate_dateEdit);


        gridLayout->addLayout(horizontalLayout_3, 2, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_4->addWidget(label_3);

        endDate_dateEdit = new QDateEdit(groupBox);
        endDate_dateEdit->setObjectName(QStringLiteral("endDate_dateEdit"));

        horizontalLayout_4->addWidget(endDate_dateEdit);


        gridLayout->addLayout(horizontalLayout_4, 3, 0, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        chooseSecodeListFromTable_pushButton = new QPushButton(groupBox);
        chooseSecodeListFromTable_pushButton->setObjectName(QStringLiteral("chooseSecodeListFromTable_pushButton"));

        horizontalLayout_8->addWidget(chooseSecodeListFromTable_pushButton);

        chooseSecodeListFromExcel_pushButton = new QPushButton(groupBox);
        chooseSecodeListFromExcel_pushButton->setObjectName(QStringLiteral("chooseSecodeListFromExcel_pushButton"));

        horizontalLayout_8->addWidget(chooseSecodeListFromExcel_pushButton);


        gridLayout->addLayout(horizontalLayout_8, 4, 0, 1, 1);

        extractMarketData_pushButton = new QPushButton(groupBox);
        extractMarketData_pushButton->setObjectName(QStringLiteral("extractMarketData_pushButton"));

        gridLayout->addWidget(extractMarketData_pushButton, 5, 0, 1, 1);


        gridLayout_6->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(250, 120));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_6->addWidget(label_7);

        indexCode_comboBox = new QComboBox(groupBox_2);
        indexCode_comboBox->setObjectName(QStringLiteral("indexCode_comboBox"));

        horizontalLayout_6->addWidget(indexCode_comboBox);


        gridLayout_3->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_7->addWidget(label_8);

        indexStartDate_dateEdit = new QDateEdit(groupBox_2);
        indexStartDate_dateEdit->setObjectName(QStringLiteral("indexStartDate_dateEdit"));

        horizontalLayout_7->addWidget(indexStartDate_dateEdit);


        gridLayout_3->addLayout(horizontalLayout_7, 1, 0, 1, 1);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_12->addWidget(label_9);

        indexEndDate_dateEdit = new QDateEdit(groupBox_2);
        indexEndDate_dateEdit->setObjectName(QStringLiteral("indexEndDate_dateEdit"));

        horizontalLayout_12->addWidget(indexEndDate_dateEdit);


        gridLayout_3->addLayout(horizontalLayout_12, 2, 0, 1, 1);

        extractWeightData_pushButton = new QPushButton(groupBox_2);
        extractWeightData_pushButton->setObjectName(QStringLiteral("extractWeightData_pushButton"));

        gridLayout_3->addWidget(extractWeightData_pushButton, 3, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_2, 2, 0, 1, 1);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(250, 120));
        gridLayout_4 = new QGridLayout(groupBox_3);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_10 = new QLabel(groupBox_3);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_9->addWidget(label_10);

        industry_comboBox = new QComboBox(groupBox_3);
        industry_comboBox->setObjectName(QStringLiteral("industry_comboBox"));

        horizontalLayout_9->addWidget(industry_comboBox);


        gridLayout_4->addLayout(horizontalLayout_9, 0, 0, 1, 1);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_11->addWidget(label_12);

        industryEndDate_dateEdit = new QDateEdit(groupBox_3);
        industryEndDate_dateEdit->setObjectName(QStringLiteral("industryEndDate_dateEdit"));

        horizontalLayout_11->addWidget(industryEndDate_dateEdit);


        gridLayout_4->addLayout(horizontalLayout_11, 2, 0, 1, 1);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_11 = new QLabel(groupBox_3);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_10->addWidget(label_11);

        industryStartDate_dateEdit = new QDateEdit(groupBox_3);
        industryStartDate_dateEdit->setObjectName(QStringLiteral("industryStartDate_dateEdit"));

        horizontalLayout_10->addWidget(industryStartDate_dateEdit);


        gridLayout_4->addLayout(horizontalLayout_10, 1, 0, 1, 1);

        extractIndustryData_pushButton = new QPushButton(groupBox_3);
        extractIndustryData_pushButton->setObjectName(QStringLiteral("extractIndustryData_pushButton"));

        gridLayout_4->addWidget(extractIndustryData_pushButton, 3, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_3, 3, 0, 1, 1);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setMinimumSize(QSize(250, 120));
        gridLayout_5 = new QGridLayout(groupBox_4);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        label_14 = new QLabel(groupBox_4);
        label_14->setObjectName(QStringLiteral("label_14"));

        horizontalLayout_13->addWidget(label_14);

        fundamentValue_comboBox = new QComboBox(groupBox_4);
        fundamentValue_comboBox->setObjectName(QStringLiteral("fundamentValue_comboBox"));

        horizontalLayout_13->addWidget(fundamentValue_comboBox);


        gridLayout_5->addLayout(horizontalLayout_13, 0, 0, 1, 1);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        label_16 = new QLabel(groupBox_4);
        label_16->setObjectName(QStringLiteral("label_16"));

        horizontalLayout_16->addWidget(label_16);

        fundamentStart_dateEdit = new QDateEdit(groupBox_4);
        fundamentStart_dateEdit->setObjectName(QStringLiteral("fundamentStart_dateEdit"));

        horizontalLayout_16->addWidget(fundamentStart_dateEdit);


        gridLayout_5->addLayout(horizontalLayout_16, 1, 0, 1, 1);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
        label_17 = new QLabel(groupBox_4);
        label_17->setObjectName(QStringLiteral("label_17"));

        horizontalLayout_17->addWidget(label_17);

        fundamentEnd_dateEdit = new QDateEdit(groupBox_4);
        fundamentEnd_dateEdit->setObjectName(QStringLiteral("fundamentEnd_dateEdit"));

        horizontalLayout_17->addWidget(fundamentEnd_dateEdit);


        gridLayout_5->addLayout(horizontalLayout_17, 2, 0, 1, 1);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
        chooseSecodeList_pushButton = new QPushButton(groupBox_4);
        chooseSecodeList_pushButton->setObjectName(QStringLiteral("chooseSecodeList_pushButton"));

        horizontalLayout_18->addWidget(chooseSecodeList_pushButton);

        importSecodeList_pushButton = new QPushButton(groupBox_4);
        importSecodeList_pushButton->setObjectName(QStringLiteral("importSecodeList_pushButton"));

        horizontalLayout_18->addWidget(importSecodeList_pushButton);


        gridLayout_5->addLayout(horizontalLayout_18, 3, 0, 1, 1);

        startExtractFundament_pushButton = new QPushButton(groupBox_4);
        startExtractFundament_pushButton->setObjectName(QStringLiteral("startExtractFundament_pushButton"));

        gridLayout_5->addWidget(startExtractFundament_pushButton, 4, 0, 1, 1);

        label_16->raise();
        label_17->raise();
        fundamentStart_dateEdit->raise();
        fundamentEnd_dateEdit->raise();
        chooseSecodeList_pushButton->raise();
        importSecodeList_pushButton->raise();
        startExtractFundament_pushButton->raise();

        gridLayout_6->addWidget(groupBox_4, 4, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_5);

        programInfo_Table = new QTableView(centralwidget);
        programInfo_Table->setObjectName(QStringLiteral("programInfo_Table"));

        verticalLayout->addWidget(programInfo_Table);


        gridLayout_6->addLayout(verticalLayout, 4, 1, 1, 1);

        ExtractDataForm->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ExtractDataForm);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1097, 23));
        ExtractDataForm->setMenuBar(menubar);
        statusbar = new QStatusBar(ExtractDataForm);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        ExtractDataForm->setStatusBar(statusbar);

        retranslateUi(ExtractDataForm);

        QMetaObject::connectSlotsByName(ExtractDataForm);
    } // setupUi

    void retranslateUi(QMainWindow *ExtractDataForm)
    {
        ExtractDataForm->setWindowTitle(QApplication::translate("ExtractDataForm", "MainWindow", Q_NULLPTR));
        label_6->setText(QApplication::translate("ExtractDataForm", " \351\200\211\346\213\251\346\225\260\346\215\256\346\272\220: ", Q_NULLPTR));
        chooseDesFileDir_pushButton->setText(QApplication::translate("ExtractDataForm", "\351\200\211\346\213\251\347\224\237\346\210\220\346\226\207\344\273\266\344\275\215\347\275\256", Q_NULLPTR));
        chooseOriFileDir_pushButton->setText(QApplication::translate("ExtractDataForm", "\351\200\211\346\213\251 \350\202\241\347\245\250\345\210\227\350\241\250EXCEL\346\226\207\344\273\266 \346\211\200\345\234\250\344\275\215\347\275\256", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("ExtractDataForm", "\346\217\220\345\217\226\350\241\214\346\203\205\346\225\260\346\215\256", Q_NULLPTR));
        label->setText(QApplication::translate("ExtractDataForm", "\350\241\214\346\203\205\346\225\260\346\215\256\351\242\221\347\216\207: ", Q_NULLPTR));
        label_4->setText(QApplication::translate("ExtractDataForm", "\350\241\214\346\203\205\346\214\207\346\240\207\357\274\232", Q_NULLPTR));
        label_2->setText(QApplication::translate("ExtractDataForm", "\350\265\267\345\247\213\346\227\266\351\227\264: ", Q_NULLPTR));
        label_3->setText(QApplication::translate("ExtractDataForm", "\346\210\252\346\255\242\346\227\266\351\227\264:", Q_NULLPTR));
        chooseSecodeListFromTable_pushButton->setText(QApplication::translate("ExtractDataForm", "\346\211\213\345\212\250\351\200\211\346\213\251\350\202\241\347\245\250\344\273\243\347\240\201", Q_NULLPTR));
        chooseSecodeListFromExcel_pushButton->setText(QApplication::translate("ExtractDataForm", "\344\273\216EXCEL\345\257\274\345\205\245\350\202\241\347\245\250\344\273\243\347\240\201", Q_NULLPTR));
        extractMarketData_pushButton->setText(QApplication::translate("ExtractDataForm", "\345\274\200\345\247\213\346\217\220\345\217\226", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("ExtractDataForm", "\346\217\220\345\217\226\346\214\207\346\225\260\346\235\203\351\207\215", Q_NULLPTR));
        label_7->setText(QApplication::translate("ExtractDataForm", "\346\214\207\346\225\260\345\220\215\347\247\260: ", Q_NULLPTR));
        label_8->setText(QApplication::translate("ExtractDataForm", "\346\214\207\346\225\260\346\210\252\346\255\242\346\227\245 \345\274\200\345\247\213\346\227\266\351\227\264\357\274\232", Q_NULLPTR));
        label_9->setText(QApplication::translate("ExtractDataForm", "\346\214\207\346\225\260\346\210\252\346\255\242\346\227\245 \347\273\223\346\235\237\346\227\266\351\227\264\357\274\232", Q_NULLPTR));
        extractWeightData_pushButton->setText(QApplication::translate("ExtractDataForm", "\345\274\200\345\247\213\346\217\220\345\217\226", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("ExtractDataForm", "\346\217\220\345\217\226\350\241\214\344\270\232\345\210\206\347\261\273", Q_NULLPTR));
        label_10->setText(QApplication::translate("ExtractDataForm", "\345\210\206\347\261\273\346\214\207\346\240\207", Q_NULLPTR));
        label_12->setText(QApplication::translate("ExtractDataForm", "\347\273\210\346\255\242\346\227\266\351\227\264", Q_NULLPTR));
        label_11->setText(QApplication::translate("ExtractDataForm", "\350\265\267\345\247\213\346\227\266\351\227\264", Q_NULLPTR));
        extractIndustryData_pushButton->setText(QApplication::translate("ExtractDataForm", "\345\274\200\345\247\213\346\217\220\345\217\226", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("ExtractDataForm", "\346\217\220\345\217\226\345\237\272\346\234\254\351\235\242\346\225\260\346\215\256:", Q_NULLPTR));
        label_14->setText(QApplication::translate("ExtractDataForm", "\345\237\272\346\234\254\351\235\242\346\214\207\346\240\207\357\274\232", Q_NULLPTR));
        label_16->setText(QApplication::translate("ExtractDataForm", "\350\265\267\345\247\213\346\227\266\351\227\264\357\274\232", Q_NULLPTR));
        label_17->setText(QApplication::translate("ExtractDataForm", "\346\210\252\346\255\242\346\227\266\351\227\264\357\274\232", Q_NULLPTR));
        chooseSecodeList_pushButton->setText(QApplication::translate("ExtractDataForm", "\346\211\213\345\212\250\351\200\211\346\213\251\350\202\241\347\245\250\344\273\243\347\240\201", Q_NULLPTR));
        importSecodeList_pushButton->setText(QApplication::translate("ExtractDataForm", "\344\273\216EXCEL \345\257\274\345\205\245\350\202\241\347\245\250\344\273\243\347\240\201", Q_NULLPTR));
        startExtractFundament_pushButton->setText(QApplication::translate("ExtractDataForm", "\345\274\200\345\247\213\346\217\220\345\217\226", Q_NULLPTR));
        label_5->setText(QApplication::translate("ExtractDataForm", "--- \347\250\213\345\272\217\350\277\220\350\241\214\344\277\241\346\201\257 ---", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ExtractDataForm: public Ui_ExtractDataForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTRACTDATAFORM_H
