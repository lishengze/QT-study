/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout_13;
    QGroupBox *chooseDataSource;
    QGridLayout *gridLayout_12;
    QGridLayout *gridLayout_11;
    QLabel *label_4;
    QComboBox *dataSource_ComboBox;
    QGroupBox *choooseFuture;
    QGridLayout *gridLayout_7;
    QGridLayout *gridLayout_6;
    QLabel *label;
    QComboBox *futureList_ComboBox;
    QLabel *label_2;
    QDoubleSpinBox *maxSpread_SpinBox;
    QLabel *label_3;
    QDoubleSpinBox *minSpread_SpinBox;
    QPushButton *showFutureSpread_Button;
    QGroupBox *chooseAnnouncement;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_5;
    QLabel *endTimeLabel_2;
    QDateEdit *AnnouncementEndDate;
    QDateEdit *AnnouncementStartDate;
    QLabel *startTimeLabel_2;
    QPushButton *Annoucnement_Button;
    QGroupBox *chooseTimeBox;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *chooseTime_gridLayout;
    QGridLayout *MACDTime_gridLayout;
    QLabel *EMA1Time_label;
    QLabel *EMA2Time_label;
    QLabel *DIFFTime_label;
    QSpinBox *EMA1TimeSpinBox;
    QSpinBox *EMA2TimeSpinBox;
    QSpinBox *DIFFTimeSpinBox;
    QGridLayout *gridLayout_2;
    QLabel *timeFre_label;
    QComboBox *dataFrequency;
    QLabel *hedgeTarget_Label;
    QComboBox *hedgeTarget_comboBox;
    QLabel *hedgeCount_label;
    QSpinBox *hedgeCount_spinBox;
    QGroupBox *chooseEnergy_groupBox;
    QGridLayout *gridLayout_10;
    QGridLayout *gridLayout_9;
    QLabel *energyTime_label;
    QComboBox *energyDataFrequency_comboBox;
    QLabel *label_11;
    QLineEdit *energyHedgedCode_lineEdit;
    QLabel *label_12;
    QLabel *label_13;
    QSpinBox *pictureNumb_spinBox;
    QLineEdit *energySelectCode_lineEdit;
    QLabel *label_10;
    QComboBox *energyAveParam_comboBox;
    QComboBox *energyCssParam_comboBox;
    QPushButton *indexEnergyChart;
    QLabel *label_14;
    QGroupBox *CalendarBox;
    QGridLayout *gridLayout_4;
    QCalendarWidget *calendarWidget;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *programInfo_verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *strategy_label;
    QPushButton *chooseStrategyDir_Button;
    QLabel *strategy_label_2;
    QPushButton *choosePortfolioDir_Button;
    QHBoxLayout *horizontalLayout_2;
    QTableView *strategy_table;
    QTableView *buySalePortfolio_table;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *portfolioIndexHistSpreadChart;
    QPushButton *portfolioIndexRealSpreadChart;
    QPushButton *portfolioIndexEnergyChart;
    QPushButton *portfolioHedgedHistSpreadChart;
    QPushButton *portfolioHedgedRealSpreadChart;
    QPushButton *portfolioHedgedEnergyChart;
    QRadioButton *isNetValueHedged;
    QLabel *programInfo_Label;
    QTableView *programInfo_tableView;
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_3;
    QDateEdit *endDate;
    QLabel *label_7;
    QLabel *energyUpdate_label;
    QSpinBox *energyUpdate_spinBox;
    QSpinBox *spreadUpdate_spinBox;
    QLabel *endDate_Label;
    QDateEdit *startDate;
    QLabel *startDate_Label;
    QDateEdit *weightDate;
    QLabel *label_5;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->setEnabled(true);
        Widget->resize(1406, 993);
        Widget->setMinimumSize(QSize(280, 240));
        gridLayout_13 = new QGridLayout(Widget);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        chooseDataSource = new QGroupBox(Widget);
        chooseDataSource->setObjectName(QStringLiteral("chooseDataSource"));
        chooseDataSource->setMinimumSize(QSize(350, 0));
        gridLayout_12 = new QGridLayout(chooseDataSource);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        gridLayout_11 = new QGridLayout();
        gridLayout_11->setSpacing(6);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        label_4 = new QLabel(chooseDataSource);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_11->addWidget(label_4, 0, 0, 1, 1);

        dataSource_ComboBox = new QComboBox(chooseDataSource);
        dataSource_ComboBox->setObjectName(QStringLiteral("dataSource_ComboBox"));

        gridLayout_11->addWidget(dataSource_ComboBox, 0, 1, 1, 1);


        gridLayout_12->addLayout(gridLayout_11, 0, 0, 1, 1);


        gridLayout_13->addWidget(chooseDataSource, 6, 0, 1, 1);

        choooseFuture = new QGroupBox(Widget);
        choooseFuture->setObjectName(QStringLiteral("choooseFuture"));
        choooseFuture->setMinimumSize(QSize(350, 0));
        choooseFuture->setMaximumSize(QSize(350, 16777215));
        gridLayout_7 = new QGridLayout(choooseFuture);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setSpacing(6);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label = new QLabel(choooseFuture);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_6->addWidget(label, 0, 0, 1, 1);

        futureList_ComboBox = new QComboBox(choooseFuture);
        futureList_ComboBox->setObjectName(QStringLiteral("futureList_ComboBox"));

        gridLayout_6->addWidget(futureList_ComboBox, 0, 1, 1, 1);

        label_2 = new QLabel(choooseFuture);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_6->addWidget(label_2, 1, 0, 1, 1);

        maxSpread_SpinBox = new QDoubleSpinBox(choooseFuture);
        maxSpread_SpinBox->setObjectName(QStringLiteral("maxSpread_SpinBox"));

        gridLayout_6->addWidget(maxSpread_SpinBox, 1, 1, 1, 1);

        label_3 = new QLabel(choooseFuture);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_6->addWidget(label_3, 2, 0, 1, 1);

        minSpread_SpinBox = new QDoubleSpinBox(choooseFuture);
        minSpread_SpinBox->setObjectName(QStringLiteral("minSpread_SpinBox"));

        gridLayout_6->addWidget(minSpread_SpinBox, 2, 1, 1, 1);

        showFutureSpread_Button = new QPushButton(choooseFuture);
        showFutureSpread_Button->setObjectName(QStringLiteral("showFutureSpread_Button"));
        showFutureSpread_Button->setMinimumSize(QSize(0, 22));

        gridLayout_6->addWidget(showFutureSpread_Button, 2, 2, 1, 1);


        gridLayout_7->addLayout(gridLayout_6, 0, 0, 1, 1);


        gridLayout_13->addWidget(choooseFuture, 4, 0, 1, 1);

        chooseAnnouncement = new QGroupBox(Widget);
        chooseAnnouncement->setObjectName(QStringLiteral("chooseAnnouncement"));
        chooseAnnouncement->setMinimumSize(QSize(350, 0));
        chooseAnnouncement->setMaximumSize(QSize(350, 16777215));
        gridLayout_8 = new QGridLayout(chooseAnnouncement);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_5 = new QGridLayout();
        gridLayout_5->setSpacing(6);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        endTimeLabel_2 = new QLabel(chooseAnnouncement);
        endTimeLabel_2->setObjectName(QStringLiteral("endTimeLabel_2"));

        gridLayout_5->addWidget(endTimeLabel_2, 1, 0, 1, 1);

        AnnouncementEndDate = new QDateEdit(chooseAnnouncement);
        AnnouncementEndDate->setObjectName(QStringLiteral("AnnouncementEndDate"));

        gridLayout_5->addWidget(AnnouncementEndDate, 1, 1, 1, 1);

        AnnouncementStartDate = new QDateEdit(chooseAnnouncement);
        AnnouncementStartDate->setObjectName(QStringLiteral("AnnouncementStartDate"));

        gridLayout_5->addWidget(AnnouncementStartDate, 0, 1, 1, 1);

        startTimeLabel_2 = new QLabel(chooseAnnouncement);
        startTimeLabel_2->setObjectName(QStringLiteral("startTimeLabel_2"));

        gridLayout_5->addWidget(startTimeLabel_2, 0, 0, 1, 1);

        Annoucnement_Button = new QPushButton(chooseAnnouncement);
        Annoucnement_Button->setObjectName(QStringLiteral("Annoucnement_Button"));
        Annoucnement_Button->setMinimumSize(QSize(0, 22));
        Annoucnement_Button->setMaximumSize(QSize(100, 16777215));

        gridLayout_5->addWidget(Annoucnement_Button, 1, 2, 1, 1);


        gridLayout_8->addLayout(gridLayout_5, 0, 0, 1, 1);


        gridLayout_13->addWidget(chooseAnnouncement, 5, 0, 1, 1);

        chooseTimeBox = new QGroupBox(Widget);
        chooseTimeBox->setObjectName(QStringLiteral("chooseTimeBox"));
        chooseTimeBox->setEnabled(true);
        chooseTimeBox->setMinimumSize(QSize(350, 0));
        chooseTimeBox->setMaximumSize(QSize(350, 16777215));
        gridLayout = new QGridLayout(chooseTimeBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        chooseTime_gridLayout = new QGridLayout();
        chooseTime_gridLayout->setSpacing(6);
        chooseTime_gridLayout->setObjectName(QStringLiteral("chooseTime_gridLayout"));

        verticalLayout_3->addLayout(chooseTime_gridLayout);

        MACDTime_gridLayout = new QGridLayout();
        MACDTime_gridLayout->setSpacing(6);
        MACDTime_gridLayout->setObjectName(QStringLiteral("MACDTime_gridLayout"));
        EMA1Time_label = new QLabel(chooseTimeBox);
        EMA1Time_label->setObjectName(QStringLiteral("EMA1Time_label"));
        EMA1Time_label->setMaximumSize(QSize(16777215, 25));

        MACDTime_gridLayout->addWidget(EMA1Time_label, 0, 0, 1, 1);

        EMA2Time_label = new QLabel(chooseTimeBox);
        EMA2Time_label->setObjectName(QStringLiteral("EMA2Time_label"));

        MACDTime_gridLayout->addWidget(EMA2Time_label, 0, 1, 1, 1);

        DIFFTime_label = new QLabel(chooseTimeBox);
        DIFFTime_label->setObjectName(QStringLiteral("DIFFTime_label"));

        MACDTime_gridLayout->addWidget(DIFFTime_label, 0, 2, 1, 1);

        EMA1TimeSpinBox = new QSpinBox(chooseTimeBox);
        EMA1TimeSpinBox->setObjectName(QStringLiteral("EMA1TimeSpinBox"));

        MACDTime_gridLayout->addWidget(EMA1TimeSpinBox, 1, 0, 1, 1);

        EMA2TimeSpinBox = new QSpinBox(chooseTimeBox);
        EMA2TimeSpinBox->setObjectName(QStringLiteral("EMA2TimeSpinBox"));

        MACDTime_gridLayout->addWidget(EMA2TimeSpinBox, 1, 1, 1, 1);

        DIFFTimeSpinBox = new QSpinBox(chooseTimeBox);
        DIFFTimeSpinBox->setObjectName(QStringLiteral("DIFFTimeSpinBox"));

        MACDTime_gridLayout->addWidget(DIFFTimeSpinBox, 1, 2, 1, 1);


        verticalLayout_3->addLayout(MACDTime_gridLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        timeFre_label = new QLabel(chooseTimeBox);
        timeFre_label->setObjectName(QStringLiteral("timeFre_label"));
        timeFre_label->setMaximumSize(QSize(60, 16777215));
        timeFre_label->setSizeIncrement(QSize(40, 0));

        gridLayout_2->addWidget(timeFre_label, 0, 0, 1, 1);

        dataFrequency = new QComboBox(chooseTimeBox);
        dataFrequency->setObjectName(QStringLiteral("dataFrequency"));
        dataFrequency->setMaximumSize(QSize(80, 16777215));

        gridLayout_2->addWidget(dataFrequency, 0, 1, 1, 1);

        hedgeTarget_Label = new QLabel(chooseTimeBox);
        hedgeTarget_Label->setObjectName(QStringLiteral("hedgeTarget_Label"));
        hedgeTarget_Label->setMaximumSize(QSize(60, 16777215));

        gridLayout_2->addWidget(hedgeTarget_Label, 1, 0, 1, 1);

        hedgeTarget_comboBox = new QComboBox(chooseTimeBox);
        hedgeTarget_comboBox->setObjectName(QStringLiteral("hedgeTarget_comboBox"));
        hedgeTarget_comboBox->setMaximumSize(QSize(100, 16777215));

        gridLayout_2->addWidget(hedgeTarget_comboBox, 1, 1, 1, 1);

        hedgeCount_label = new QLabel(chooseTimeBox);
        hedgeCount_label->setObjectName(QStringLiteral("hedgeCount_label"));
        hedgeCount_label->setMaximumSize(QSize(100, 16777215));
        hedgeCount_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(hedgeCount_label, 1, 2, 1, 1);

        hedgeCount_spinBox = new QSpinBox(chooseTimeBox);
        hedgeCount_spinBox->setObjectName(QStringLiteral("hedgeCount_spinBox"));
        hedgeCount_spinBox->setMaximumSize(QSize(100, 16777215));

        gridLayout_2->addWidget(hedgeCount_spinBox, 1, 3, 1, 1);


        verticalLayout_3->addLayout(gridLayout_2);


        gridLayout->addLayout(verticalLayout_3, 0, 0, 1, 1);


        gridLayout_13->addWidget(chooseTimeBox, 2, 0, 1, 1);

        chooseEnergy_groupBox = new QGroupBox(Widget);
        chooseEnergy_groupBox->setObjectName(QStringLiteral("chooseEnergy_groupBox"));
        chooseEnergy_groupBox->setMinimumSize(QSize(350, 0));
        chooseEnergy_groupBox->setMaximumSize(QSize(350, 16777215));
        gridLayout_10 = new QGridLayout(chooseEnergy_groupBox);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        gridLayout_9 = new QGridLayout();
        gridLayout_9->setSpacing(6);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        energyTime_label = new QLabel(chooseEnergy_groupBox);
        energyTime_label->setObjectName(QStringLiteral("energyTime_label"));

        gridLayout_9->addWidget(energyTime_label, 0, 0, 1, 1);

        energyDataFrequency_comboBox = new QComboBox(chooseEnergy_groupBox);
        energyDataFrequency_comboBox->setObjectName(QStringLiteral("energyDataFrequency_comboBox"));

        gridLayout_9->addWidget(energyDataFrequency_comboBox, 0, 3, 1, 3);

        label_11 = new QLabel(chooseEnergy_groupBox);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setMinimumSize(QSize(60, 0));
        label_11->setMaximumSize(QSize(200, 16777215));

        gridLayout_9->addWidget(label_11, 2, 0, 1, 3);

        energyHedgedCode_lineEdit = new QLineEdit(chooseEnergy_groupBox);
        energyHedgedCode_lineEdit->setObjectName(QStringLiteral("energyHedgedCode_lineEdit"));
        energyHedgedCode_lineEdit->setMinimumSize(QSize(100, 0));
        energyHedgedCode_lineEdit->setMaximumSize(QSize(200, 16777215));

        gridLayout_9->addWidget(energyHedgedCode_lineEdit, 2, 3, 1, 3);

        label_12 = new QLabel(chooseEnergy_groupBox);
        label_12->setObjectName(QStringLiteral("label_12"));

        gridLayout_9->addWidget(label_12, 3, 0, 1, 1);

        label_13 = new QLabel(chooseEnergy_groupBox);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setMaximumSize(QSize(60, 16777215));

        gridLayout_9->addWidget(label_13, 4, 0, 1, 1);

        pictureNumb_spinBox = new QSpinBox(chooseEnergy_groupBox);
        pictureNumb_spinBox->setObjectName(QStringLiteral("pictureNumb_spinBox"));

        gridLayout_9->addWidget(pictureNumb_spinBox, 3, 5, 1, 1);

        energySelectCode_lineEdit = new QLineEdit(chooseEnergy_groupBox);
        energySelectCode_lineEdit->setObjectName(QStringLiteral("energySelectCode_lineEdit"));

        gridLayout_9->addWidget(energySelectCode_lineEdit, 1, 3, 1, 3);

        label_10 = new QLabel(chooseEnergy_groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout_9->addWidget(label_10, 1, 0, 1, 2);

        energyAveParam_comboBox = new QComboBox(chooseEnergy_groupBox);
        energyAveParam_comboBox->setObjectName(QStringLiteral("energyAveParam_comboBox"));

        gridLayout_9->addWidget(energyAveParam_comboBox, 3, 1, 1, 2);

        energyCssParam_comboBox = new QComboBox(chooseEnergy_groupBox);
        energyCssParam_comboBox->setObjectName(QStringLiteral("energyCssParam_comboBox"));
        energyCssParam_comboBox->setMinimumSize(QSize(100, 0));

        gridLayout_9->addWidget(energyCssParam_comboBox, 4, 1, 1, 2);

        indexEnergyChart = new QPushButton(chooseEnergy_groupBox);
        indexEnergyChart->setObjectName(QStringLiteral("indexEnergyChart"));

        gridLayout_9->addWidget(indexEnergyChart, 4, 3, 1, 3);

        label_14 = new QLabel(chooseEnergy_groupBox);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_9->addWidget(label_14, 3, 3, 1, 2);


        gridLayout_10->addLayout(gridLayout_9, 0, 0, 1, 1);


        gridLayout_13->addWidget(chooseEnergy_groupBox, 3, 0, 1, 1);

        CalendarBox = new QGroupBox(Widget);
        CalendarBox->setObjectName(QStringLiteral("CalendarBox"));
        CalendarBox->setMinimumSize(QSize(350, 1));
        CalendarBox->setMaximumSize(QSize(350, 250));
        gridLayout_4 = new QGridLayout(CalendarBox);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        calendarWidget = new QCalendarWidget(CalendarBox);
        calendarWidget->setObjectName(QStringLiteral("calendarWidget"));
        calendarWidget->setMinimumSize(QSize(200, 200));

        gridLayout_4->addWidget(calendarWidget, 0, 0, 1, 1);


        gridLayout_13->addWidget(CalendarBox, 0, 0, 1, 1);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        programInfo_verticalLayout = new QVBoxLayout();
        programInfo_verticalLayout->setSpacing(6);
        programInfo_verticalLayout->setObjectName(QStringLiteral("programInfo_verticalLayout"));
        programInfo_verticalLayout->setContentsMargins(-1, 20, -1, -1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        strategy_label = new QLabel(Widget);
        strategy_label->setObjectName(QStringLiteral("strategy_label"));
        strategy_label->setMinimumSize(QSize(0, 16));

        horizontalLayout_4->addWidget(strategy_label);

        chooseStrategyDir_Button = new QPushButton(Widget);
        chooseStrategyDir_Button->setObjectName(QStringLiteral("chooseStrategyDir_Button"));

        horizontalLayout_4->addWidget(chooseStrategyDir_Button);

        strategy_label_2 = new QLabel(Widget);
        strategy_label_2->setObjectName(QStringLiteral("strategy_label_2"));
        strategy_label_2->setMinimumSize(QSize(0, 16));

        horizontalLayout_4->addWidget(strategy_label_2);

        choosePortfolioDir_Button = new QPushButton(Widget);
        choosePortfolioDir_Button->setObjectName(QStringLiteral("choosePortfolioDir_Button"));

        horizontalLayout_4->addWidget(choosePortfolioDir_Button);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        strategy_table = new QTableView(Widget);
        strategy_table->setObjectName(QStringLiteral("strategy_table"));

        horizontalLayout_2->addWidget(strategy_table);

        buySalePortfolio_table = new QTableView(Widget);
        buySalePortfolio_table->setObjectName(QStringLiteral("buySalePortfolio_table"));

        horizontalLayout_2->addWidget(buySalePortfolio_table);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        portfolioIndexHistSpreadChart = new QPushButton(Widget);
        portfolioIndexHistSpreadChart->setObjectName(QStringLiteral("portfolioIndexHistSpreadChart"));
        portfolioIndexHistSpreadChart->setEnabled(true);
        portfolioIndexHistSpreadChart->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_3->addWidget(portfolioIndexHistSpreadChart);

        portfolioIndexRealSpreadChart = new QPushButton(Widget);
        portfolioIndexRealSpreadChart->setObjectName(QStringLiteral("portfolioIndexRealSpreadChart"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(4);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(portfolioIndexRealSpreadChart->sizePolicy().hasHeightForWidth());
        portfolioIndexRealSpreadChart->setSizePolicy(sizePolicy);
        portfolioIndexRealSpreadChart->setMaximumSize(QSize(130, 16777215));
        portfolioIndexRealSpreadChart->setLayoutDirection(Qt::RightToLeft);
        portfolioIndexRealSpreadChart->setAutoFillBackground(false);

        horizontalLayout_3->addWidget(portfolioIndexRealSpreadChart);

        portfolioIndexEnergyChart = new QPushButton(Widget);
        portfolioIndexEnergyChart->setObjectName(QStringLiteral("portfolioIndexEnergyChart"));
        portfolioIndexEnergyChart->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_3->addWidget(portfolioIndexEnergyChart);

        portfolioHedgedHistSpreadChart = new QPushButton(Widget);
        portfolioHedgedHistSpreadChart->setObjectName(QStringLiteral("portfolioHedgedHistSpreadChart"));
        portfolioHedgedHistSpreadChart->setMinimumSize(QSize(0, 0));
        portfolioHedgedHistSpreadChart->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_3->addWidget(portfolioHedgedHistSpreadChart);

        portfolioHedgedRealSpreadChart = new QPushButton(Widget);
        portfolioHedgedRealSpreadChart->setObjectName(QStringLiteral("portfolioHedgedRealSpreadChart"));
        portfolioHedgedRealSpreadChart->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_3->addWidget(portfolioHedgedRealSpreadChart);

        portfolioHedgedEnergyChart = new QPushButton(Widget);
        portfolioHedgedEnergyChart->setObjectName(QStringLiteral("portfolioHedgedEnergyChart"));
        portfolioHedgedEnergyChart->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_3->addWidget(portfolioHedgedEnergyChart);

        isNetValueHedged = new QRadioButton(Widget);
        isNetValueHedged->setObjectName(QStringLiteral("isNetValueHedged"));
        isNetValueHedged->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(isNetValueHedged);


        verticalLayout_2->addLayout(horizontalLayout_3);


        programInfo_verticalLayout->addLayout(verticalLayout_2);

        programInfo_Label = new QLabel(Widget);
        programInfo_Label->setObjectName(QStringLiteral("programInfo_Label"));
        programInfo_Label->setMinimumSize(QSize(0, 25));
        programInfo_Label->setAlignment(Qt::AlignCenter);

        programInfo_verticalLayout->addWidget(programInfo_Label);

        programInfo_tableView = new QTableView(Widget);
        programInfo_tableView->setObjectName(QStringLiteral("programInfo_tableView"));
        programInfo_tableView->setMaximumSize(QSize(16777215, 200));

        programInfo_verticalLayout->addWidget(programInfo_tableView);


        verticalLayout_4->addLayout(programInfo_verticalLayout);


        gridLayout_13->addLayout(verticalLayout_4, 0, 1, 9, 1);

        groupBox = new QGroupBox(Widget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 170));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 14, 331, 152));
        gridLayout_3 = new QGridLayout(layoutWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        endDate = new QDateEdit(layoutWidget);
        endDate->setObjectName(QStringLiteral("endDate"));

        gridLayout_3->addWidget(endDate, 1, 1, 1, 1);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout_3->addWidget(label_7, 4, 0, 1, 1);

        energyUpdate_label = new QLabel(layoutWidget);
        energyUpdate_label->setObjectName(QStringLiteral("energyUpdate_label"));

        gridLayout_3->addWidget(energyUpdate_label, 3, 0, 1, 1);

        energyUpdate_spinBox = new QSpinBox(layoutWidget);
        energyUpdate_spinBox->setObjectName(QStringLiteral("energyUpdate_spinBox"));

        gridLayout_3->addWidget(energyUpdate_spinBox, 3, 1, 1, 1);

        spreadUpdate_spinBox = new QSpinBox(layoutWidget);
        spreadUpdate_spinBox->setObjectName(QStringLiteral("spreadUpdate_spinBox"));

        gridLayout_3->addWidget(spreadUpdate_spinBox, 4, 1, 1, 1);

        endDate_Label = new QLabel(layoutWidget);
        endDate_Label->setObjectName(QStringLiteral("endDate_Label"));

        gridLayout_3->addWidget(endDate_Label, 1, 0, 1, 1);

        startDate = new QDateEdit(layoutWidget);
        startDate->setObjectName(QStringLiteral("startDate"));

        gridLayout_3->addWidget(startDate, 0, 1, 1, 1);

        startDate_Label = new QLabel(layoutWidget);
        startDate_Label->setObjectName(QStringLiteral("startDate_Label"));

        gridLayout_3->addWidget(startDate_Label, 0, 0, 1, 1);

        weightDate = new QDateEdit(layoutWidget);
        weightDate->setObjectName(QStringLiteral("weightDate"));

        gridLayout_3->addWidget(weightDate, 2, 1, 1, 1);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_3->addWidget(label_5, 2, 0, 1, 1);


        gridLayout_13->addWidget(groupBox, 1, 0, 1, 1);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", Q_NULLPTR));
        chooseDataSource->setTitle(QApplication::translate("Widget", "\351\200\211\346\213\251\346\225\260\346\215\256\346\272\220", Q_NULLPTR));
        label_4->setText(QApplication::translate("Widget", "\351\200\211\346\213\251\346\225\260\346\215\256\346\272\220:", Q_NULLPTR));
        choooseFuture->setTitle(QApplication::translate("Widget", "\346\234\237\350\264\247\345\256\236\346\227\266\345\237\272\345\267\256\345\233\276\357\274\232", Q_NULLPTR));
        label->setText(QApplication::translate("Widget", "\346\234\237\350\264\247\345\223\201\347\247\215:", Q_NULLPTR));
        label_2->setText(QApplication::translate("Widget", "\345\237\272\345\267\256\346\234\200\345\244\247\345\200\274:", Q_NULLPTR));
        label_3->setText(QApplication::translate("Widget", "\345\237\272\345\267\256\346\234\200\345\260\217\345\200\274:", Q_NULLPTR));
        showFutureSpread_Button->setText(QApplication::translate("Widget", "\346\230\276\347\244\272\345\237\272\345\267\256\345\233\276\345\203\217", Q_NULLPTR));
        chooseAnnouncement->setTitle(QApplication::translate("Widget", "\351\200\211\346\213\251\345\205\254\345\221\212\344\277\241\346\201\257", Q_NULLPTR));
        endTimeLabel_2->setText(QApplication::translate("Widget", "\345\205\254\345\221\212\347\273\210\346\255\242\346\227\266\351\227\264:", Q_NULLPTR));
        startTimeLabel_2->setText(QApplication::translate("Widget", "\345\205\254\345\221\212\350\265\267\345\247\213\346\227\266\351\227\264:", Q_NULLPTR));
        Annoucnement_Button->setText(QApplication::translate("Widget", "\346\230\276\347\244\272\345\205\254\345\221\212\344\277\241\346\201\257", Q_NULLPTR));
        chooseTimeBox->setTitle(QApplication::translate("Widget", "\347\202\271\345\267\256\345\233\276\345\217\202\346\225\260\357\274\232", Q_NULLPTR));
        EMA1Time_label->setText(QApplication::translate("Widget", "T1", Q_NULLPTR));
        EMA2Time_label->setText(QApplication::translate("Widget", "T2", Q_NULLPTR));
        DIFFTime_label->setText(QApplication::translate("Widget", "T3", Q_NULLPTR));
        timeFre_label->setText(QApplication::translate("Widget", "\346\227\266\351\227\264\351\242\221\347\216\207:", Q_NULLPTR));
        hedgeTarget_Label->setText(QApplication::translate("Widget", "\345\257\271\345\206\262\347\233\256\346\240\207:", Q_NULLPTR));
        hedgeCount_label->setText(QApplication::translate("Widget", "\347\254\224\346\225\260\357\274\232", Q_NULLPTR));
        chooseEnergy_groupBox->setTitle(QApplication::translate("Widget", "\345\212\277\350\203\275\345\233\276\345\217\202\346\225\260\357\274\232", Q_NULLPTR));
        energyTime_label->setText(QApplication::translate("Widget", "\346\227\266\351\227\264\351\242\221\347\216\207:", Q_NULLPTR));
        label_11->setText(QApplication::translate("Widget", "\345\257\271\345\206\262\346\214\207\346\225\260\346\210\226\350\202\241\347\245\250[SH\346\210\226SZ\345\274\200\345\244\264]:", Q_NULLPTR));
        label_12->setText(QApplication::translate("Widget", "\345\235\207\347\272\277\345\217\202\346\225\260\357\274\232", Q_NULLPTR));
        label_13->setText(QApplication::translate("Widget", "\346\214\207\346\240\207\345\217\202\346\225\260\357\274\232 ", Q_NULLPTR));
        label_10->setText(QApplication::translate("Widget", "\347\233\256\346\240\207\346\214\207\346\225\260\346\210\226\350\202\241\347\245\250[SH\346\210\226SZ\345\274\200\345\244\264]:", Q_NULLPTR));
        indexEnergyChart->setText(QApplication::translate("Widget", "\346\230\276\347\244\272\345\233\276\345\203\217", Q_NULLPTR));
        label_14->setText(QApplication::translate("Widget", "\345\233\276\345\203\217\346\225\260\347\233\256:", Q_NULLPTR));
        CalendarBox->setTitle(QApplication::translate("Widget", "\346\227\245\345\216\206", Q_NULLPTR));
        strategy_label->setText(QApplication::translate("Widget", "\347\273\204\345\220\210\344\277\241\346\201\257\357\274\232", Q_NULLPTR));
        chooseStrategyDir_Button->setText(QApplication::translate("Widget", "\351\200\211\346\213\251\347\273\204\345\220\210\346\226\207\344\273\266\345\244\271", Q_NULLPTR));
        strategy_label_2->setText(QApplication::translate("Widget", "\345\257\271\345\206\262\347\273\204\345\220\210\344\277\241\346\201\257\357\274\232", Q_NULLPTR));
        choosePortfolioDir_Button->setText(QApplication::translate("Widget", "\351\200\211\346\213\251\345\257\271\345\206\262\347\273\204\345\220\210\346\226\207\344\273\266\345\244\271", Q_NULLPTR));
        portfolioIndexHistSpreadChart->setText(QApplication::translate("Widget", "\345\216\206\345\217\262\346\214\207\346\225\260\347\202\271\345\267\256\345\233\276", Q_NULLPTR));
        portfolioIndexRealSpreadChart->setText(QApplication::translate("Widget", "\345\256\236\346\227\266\346\214\207\346\225\260\347\202\271\345\267\256\345\233\276", Q_NULLPTR));
        portfolioIndexEnergyChart->setText(QApplication::translate("Widget", "\347\273\204\345\220\210\345\212\277\350\203\275\345\233\276", Q_NULLPTR));
        portfolioHedgedHistSpreadChart->setText(QApplication::translate("Widget", "\345\216\206\345\217\262\347\273\204\345\220\210\345\257\271\345\206\262\347\202\271\345\267\256\345\233\276", Q_NULLPTR));
        portfolioHedgedRealSpreadChart->setText(QApplication::translate("Widget", "\345\256\236\346\227\266\347\273\204\345\220\210\345\257\271\345\206\262\347\202\271\345\267\256\345\233\276", Q_NULLPTR));
        portfolioHedgedEnergyChart->setText(QApplication::translate("Widget", "\347\273\204\345\220\210\345\257\271\345\206\262\345\212\277\350\203\275\345\233\276", Q_NULLPTR));
        isNetValueHedged->setText(QApplication::translate("Widget", "\346\230\257\345\220\246\345\207\200\345\200\274\345\257\271\345\206\262", Q_NULLPTR));
        programInfo_Label->setText(QApplication::translate("Widget", "------------ \347\250\213\345\272\217\350\277\220\350\241\214\344\277\241\346\201\257 ------------", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("Widget", "\351\200\211\346\213\251\346\227\266\351\227\264,\351\242\221\347\216\207:", Q_NULLPTR));
        label_7->setText(QApplication::translate("Widget", "\345\256\236\346\227\266\347\202\271\345\267\256\345\233\276\346\233\264\346\226\260\351\242\221\347\216\207[\347\247\222]", Q_NULLPTR));
        energyUpdate_label->setText(QApplication::translate("Widget", "\345\256\236\346\227\266\345\212\277\350\203\275\345\233\276\346\233\264\346\226\260\351\242\221\347\216\207[\347\247\222]", Q_NULLPTR));
        endDate_Label->setText(QApplication::translate("Widget", "\347\273\210\346\255\242\346\227\266\351\227\264:", Q_NULLPTR));
        startDate_Label->setText(QApplication::translate("Widget", "\350\265\267\345\247\213\346\227\266\351\227\264:", Q_NULLPTR));
        label_5->setText(QApplication::translate("Widget", "\346\235\203\351\207\215\346\227\266\351\227\264:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
