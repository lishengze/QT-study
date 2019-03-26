#include <QStandardItemModel>
#include <QStandardItem>
#include <QTime>
#include <QScrollBar>
#include <algorithm>

#include "addwarningform.h"
#include "ui_addwarningform.h"
#include "widget_func.h"
#include "io_func.h"

AddWarningForm::AddWarningForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddWarningForm)
{
    ui->setupUi(this);
}

AddWarningForm::AddWarningForm(WarningParam warningParam, QWidget* parent):
    m_warningParam(warningParam),
    QWidget(parent),
    ui(new Ui::AddWarningForm)
{
    ui->setupUi(this);

    initCommonData();
    initWidget();
}

AddWarningForm::~AddWarningForm()
{
    delete ui;
}

void AddWarningForm::initCommonData()
{
    m_waringItemMap.clear();
    m_waringItemCount = 0;
    m_waringItemIDPos = 0;
}

void AddWarningForm::initWidget()
{
    initChooseWidgets();

    initInfoTable();

    initMenu();
}

void AddWarningForm::initMenu()
{
    m_menu = new QMenu(ui->addInfo_tableView);
    m_deleteAction = new QAction(QString("删除"), ui->addInfo_tableView);
    m_menu->addAction(m_deleteAction);

    connect(m_deleteAction, SIGNAL(triggered()), 
            this,           SLOT(deleteWaringItem_slot()));


    ui->addInfo_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->addInfo_tableView, SIGNAL(customContextMenuRequested(QPoint)),
            this,                  SLOT(show_addInfoTableContextMenu_slot(QPoint)));    
}

void AddWarningForm::initChooseWidgets()
{
    ui->timeType_comboBox->addItems(m_warningParam.m_timeTypeList);
    ui->oriTarget_comboBox->addItems(m_warningParam.m_oriTargetList);
    ui->compTarget_comboBox->addItems(m_warningParam.m_compTargetList);

    QStringList compList;
    compList.append(">");
    compList.append(">=");
    compList.append("==");
    compList.append("<");
    compList.append("<=");
    ui->compare_comboBox->addItems(compList);

    ui->compTarget_radioButton->setChecked(true);
}

void AddWarningForm::initInfoTable()
{
    QStandardItemModel *initModel = new QStandardItemModel();
    initModel->setHorizontalHeaderItem(0, new QStandardItem(QString("时间")));
    initModel->setHorizontalHeaderItem(1, new QStandardItem(QString("告警ID")));
    initModel->setHorizontalHeaderItem(2, new QStandardItem(QString("增加的告警项")));
    ui->addInfo_tableView->setModel(initModel);
    ui->addInfo_tableView->setColumnWidth(0, 100);
    ui->addInfo_tableView->setColumnWidth(1, 100);
    ui->addInfo_tableView->setColumnWidth(2, 300);
    ui->addInfo_tableView->setShowGrid(false);

    QStandardItemModel *warningModel = new QStandardItemModel();
    warningModel->setHorizontalHeaderItem(0, new QStandardItem(QString("时间")));
    warningModel->setHorizontalHeaderItem(1, new QStandardItem(QString("告警ID")));
    warningModel->setHorizontalHeaderItem(2, new QStandardItem(QString("已发生的告警信息")));
    ui->warningInfo_tableView->setModel(warningModel);
    ui->warningInfo_tableView->setColumnWidth(0, 100);
    ui->warningInfo_tableView->setColumnWidth(1, 100);
    ui->warningInfo_tableView->setColumnWidth(2, 300);
    ui->warningInfo_tableView->setShowGrid(false);    

    m_waringItemIDPos = 1;
}

void AddWarningForm::updateAddInfoTable(int id, QString info)
{
    QStandardItemModel* tableMode = dynamic_cast<QStandardItemModel*>(ui->addInfo_tableView->model ());
    int row = tableMode->rowCount();
    tableMode->setItem (row, 0,  new QStandardItem(QTime::currentTime().toString("hh:mm:ss")));
    tableMode->setItem (row, 1,  new QStandardItem(QString("%1").arg(id)));
    tableMode->setItem (row, 2,  new QStandardItem(info));
    QScrollBar* verScrollBar = ui->addInfo_tableView->verticalScrollBar();
    verScrollBar->setSliderPosition(verScrollBar->maximum());    
}

void AddWarningForm::updateWaringInfoTable(int id, QString info)
{
    QStandardItemModel* tableMode = dynamic_cast<QStandardItemModel*>(ui->warningInfo_tableView->model ());
    int row = tableMode->rowCount();
    tableMode->setItem (row, 0, new QStandardItem(QTime::currentTime().toString("hh:mm:ss")));
    tableMode->setItem (row, 1, new QStandardItem(QString("%1").arg(id)));
    tableMode->setItem (row, 2, new QStandardItem(info));
    QScrollBar* verScrollBar = ui->warningInfo_tableView->verticalScrollBar();
    verScrollBar->setSliderPosition(verScrollBar->maximum());    
}

void AddWarningForm::deleteWaringItem_slot()
{
    QMap<int, int> selectMap = getSelectRows(ui->addInfo_tableView);

    // printMap(selectMap, "oriSelectMap");

    for (QMap<int, int>::iterator it = selectMap.begin(); it != selectMap.end(); ++it)
    {
        it.value() = m_waringItemIDPos;
    }
    // printMap(selectMap, "transSelectMap");

    QList<QString> selectData = getSelectTableData(ui->addInfo_tableView, selectMap);

    // printList(selectData, "selectData");

    for (int i = 0; i < selectData.size(); ++i)
    {
        m_waringItemMap.remove(selectData[i].toInt());
    }

    QList<int> rowList = selectMap.keys();
    std::sort(rowList.begin(), rowList.end());
    QAbstractItemModel* itemModel = ui->addInfo_tableView->model ();
    for (int i = rowList.size()-1; i >= 0; --i)
    {
        itemModel->removeRow(rowList[i]);
    }        
}

void AddWarningForm::show_addInfoTableContextMenu_slot(QPoint pos)
{
    m_menu->move(cursor().pos());
    m_menu->show();
    pos;
}

void AddWarningForm::on_compTarget_radioButton_clicked(bool checked)
{
    if (checked == true)
    {
        ui->compValue_radioButton->setChecked(false);
    }
}

void AddWarningForm::on_compValue_radioButton_clicked(bool checked)
{
    if (checked == true)
    {
        ui->compTarget_radioButton->setChecked(false);
    }
}

void AddWarningForm::on_pushButton_clicked()
{
    WaringItem curWaringItem;
    curWaringItem.m_timeType = ui->timeType_comboBox->currentText();
    curWaringItem.m_oriTarget = ui->oriTarget_comboBox->currentText();
    curWaringItem.m_compType = ui->compare_comboBox->currentText();

    if (ui->compTarget_radioButton->isChecked())
    {
        curWaringItem.m_compTarget = ui->compTarget_comboBox->currentText();

        if ((curWaringItem.m_oriTarget.indexOf("指标") >= 0 && curWaringItem.m_compTarget.indexOf("指标") < 0)
          ||(curWaringItem.m_oriTarget.indexOf("指标") < 0 && curWaringItem.m_compTarget.indexOf("指标") >= 0))
        {
            ShowWarningBox("势能指标与收益指标不能比较，请重新选择");
            return;
        }
    }

    if (ui->compValue_radioButton->isChecked())
    {
        curWaringItem.m_compValue = ui->compValue_doubleSpinBox->value();
    }    

    ++m_waringItemCount;

    m_waringItemMap.insert(m_waringItemCount, curWaringItem);
    updateAddInfoTable(m_waringItemCount, curWaringItem.getAddedItemString());
}

void AddWarningForm::checkWaringItem(QList<double> aveList, QList<double> cssList, int dataID)
{
    for (QMap<int, WaringItem>::iterator it = m_waringItemMap.begin();
        it != m_waringItemMap.end(); ++it)
    {
        if (m_warningParam.m_timeTypeList[dataID] != it.value().m_timeType) continue;

        bool isWarning = false;
        if (it.value().m_compTarget !="")
        {
            int index1 = m_warningParam.m_paramMap[it.value().m_oriTarget];
            int index2 = m_warningParam.m_paramMap[it.value().m_compTarget];            
            if (it.value().m_oriTarget.indexOf("指标") >= 0)
            {
                isWarning = compare(cssList[index1], cssList[index2], it.value().m_compType);
                it.value().m_oriTargetValue = cssList[index1];
                it.value().m_compTargetValue = cssList[index2];
            }
            else
            {
                isWarning = compare(aveList[index1], aveList[index2], it.value().m_compType);       
                it.value().m_oriTargetValue = aveList[index1];
                it.value().m_compTargetValue = aveList[index2];                         
            }            
        }
        else
        {
            int index1 = m_warningParam.m_paramMap[it.value().m_oriTarget];
            if (it.value().m_oriTarget.indexOf("指标") >= 0)
            {
                
                isWarning = compare(cssList[index1], it.value().m_compValue, it.value().m_compType);
                it.value().m_oriTargetValue = cssList[index1];
            }
            else
            {
                isWarning = compare(aveList[index1], it.value().m_compValue, it.value().m_compType);
                it.value().m_oriTargetValue = aveList[index1];
            }            
        }
        
        if (isWarning)
        {
            updateWaringInfoTable(it.key(), it.value().getWarningItemString());
            ShowWarningBox(it.value().getWarningItemString());
        }
    }
}

void AddWarningForm::on_compTarget_comboBox_activated(const QString &arg1)
{
    ui->compTarget_radioButton->setChecked(true);
    ui->compValue_radioButton->setChecked(false);
}

void AddWarningForm::on_compValue_doubleSpinBox_valueChanged(double arg1)
{
    ui->compTarget_radioButton->setChecked(false);
    ui->compValue_radioButton->setChecked(true);
}

void AddWarningForm::on_compValue_doubleSpinBox_editingFinished()
{
    ui->compTarget_radioButton->setChecked(false);
    ui->compValue_radioButton->setChecked(true);
}

bool compare(double val1, double val2, QString compare)
{
    if (compare==">")
    {
        return val1 > val2;
    }

    if (compare==">=")
    {
        return val1 >= val2;
    }

    if (compare=="==")
    {
        return val1 == val2;
    }   

    if (compare=="<=")
    {
        return val1 <= val2;
    }
    
    if (compare=="<")
    {
        return val1 < val2;
    }         

    return false;
}
