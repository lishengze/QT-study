/********************************************************************************
** Form generated from reading UI file 'csschartform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSSCHARTFORM_H
#define UI_CSSCHARTFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CSSChartForm
{
public:
    QGridLayout *gridLayout;
    QGridLayout *main_gridLayout;

    void setupUi(QWidget *CSSChartForm)
    {
        if (CSSChartForm->objectName().isEmpty())
            CSSChartForm->setObjectName(QStringLiteral("CSSChartForm"));
        CSSChartForm->resize(1172, 1072);
        gridLayout = new QGridLayout(CSSChartForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        main_gridLayout = new QGridLayout();
        main_gridLayout->setSpacing(0);
        main_gridLayout->setObjectName(QStringLiteral("main_gridLayout"));

        gridLayout->addLayout(main_gridLayout, 0, 0, 1, 1);


        retranslateUi(CSSChartForm);

        QMetaObject::connectSlotsByName(CSSChartForm);
    } // setupUi

    void retranslateUi(QWidget *CSSChartForm)
    {
        CSSChartForm->setWindowTitle(QApplication::translate("CSSChartForm", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CSSChartForm: public Ui_CSSChartForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSSCHARTFORM_H
