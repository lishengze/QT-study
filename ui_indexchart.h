/********************************************************************************
** Form generated from reading UI file 'indexchart.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INDEXCHART_H
#define UI_INDEXCHART_H

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

class Ui_IndexChart
{
public:
    QGridLayout *gridLayout_2;
    QLabel *title_label;
    QGridLayout *chart_gridLayout;
    QGroupBox *groupBox;
    QLabel *time_label;
    QLabel *cssValue_label;
    QLabel *acmuEarning_label;

    void setupUi(QWidget *IndexChart)
    {
        if (IndexChart->objectName().isEmpty())
            IndexChart->setObjectName(QStringLiteral("IndexChart"));
        IndexChart->resize(1132, 568);
        gridLayout_2 = new QGridLayout(IndexChart);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        title_label = new QLabel(IndexChart);
        title_label->setObjectName(QStringLiteral("title_label"));
        title_label->setMaximumSize(QSize(16777215, 40));
        title_label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(title_label, 0, 0, 1, 1);

        chart_gridLayout = new QGridLayout();
        chart_gridLayout->setObjectName(QStringLiteral("chart_gridLayout"));

        gridLayout_2->addLayout(chart_gridLayout, 1, 0, 1, 1);

        groupBox = new QGroupBox(IndexChart);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(200, 0));
        groupBox->setMaximumSize(QSize(200, 16777215));
        groupBox->setAlignment(Qt::AlignCenter);
        groupBox->setFlat(true);
        time_label = new QLabel(groupBox);
        time_label->setObjectName(QStringLiteral("time_label"));
        time_label->setGeometry(QRect(20, 40, 181, 16));
        cssValue_label = new QLabel(groupBox);
        cssValue_label->setObjectName(QStringLiteral("cssValue_label"));
        cssValue_label->setGeometry(QRect(20, 100, 181, 16));
        acmuEarning_label = new QLabel(groupBox);
        acmuEarning_label->setObjectName(QStringLiteral("acmuEarning_label"));
        acmuEarning_label->setGeometry(QRect(20, 70, 181, 16));

        gridLayout_2->addWidget(groupBox, 1, 1, 1, 1);


        retranslateUi(IndexChart);

        QMetaObject::connectSlotsByName(IndexChart);
    } // setupUi

    void retranslateUi(QWidget *IndexChart)
    {
        IndexChart->setWindowTitle(QApplication::translate("IndexChart", "Form", Q_NULLPTR));
        title_label->setText(QApplication::translate("IndexChart", "\346\240\207\351\242\230", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("IndexChart", "\345\261\236\346\200\247\345\200\274", Q_NULLPTR));
        time_label->setText(QApplication::translate("IndexChart", "\346\227\266\351\227\264\357\274\232", Q_NULLPTR));
        cssValue_label->setText(QApplication::translate("IndexChart", "\344\270\273\346\214\207\346\240\207\357\274\232", Q_NULLPTR));
        acmuEarning_label->setText(QApplication::translate("IndexChart", "\347\264\257\350\256\241\346\224\266\347\233\212\357\274\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class IndexChart: public Ui_IndexChart {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INDEXCHART_H
