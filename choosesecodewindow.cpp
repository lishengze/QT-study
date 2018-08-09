#include "choosesecodewindow.h"
#include "ui_choosesecodewindow.h"
#include <QDebug>
#include <QStandardItemModel>
#include "secode_func.h"
#include "widget_func.h"
#include "id_func.h"
#include "io_func.h"
#pragma execution_character_set("utf-8")

ChooseSecodeWindow::ChooseSecodeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChooseSecodeWindow)
{
    ui->setupUi(this);
}

ChooseSecodeWindow::ChooseSecodeWindow(QString dbhost, bool bShowIndex, QWidget *parent) :
    m_dbhost(dbhost), m_bShowIndex(bShowIndex), QMainWindow(parent),
    ui(new Ui::ChooseSecodeWindow)
{
    ui->setupUi(this);
    initMarketInfoMap();
    initDatabase();
    initTreeView();
    initCandidateTableView();
    initSelectTableView();
}

ChooseSecodeWindow::~ChooseSecodeWindow()
{
    qDebug() << "~ChooseSecodeWindow()";
    delete ui;
}

void ChooseSecodeWindow::initMarketInfoMap() {
    m_marketInfoMap.insert("全部A股", "A_Market");
    m_marketInfoMap.insert("上证A股", "A_Market_SH");
    m_marketInfoMap.insert("深证A股", "A_Market_SZ");
    m_marketInfoMap.insert("深证主板A股", "A_Market_SZ_Main");
    m_marketInfoMap.insert("创业板", "A_Market_GEM");
    m_marketInfoMap.insert("中小企业板", "A_Market_SME");
    m_marketInfoMap.insert("中小企业板(含ST)", "A_Market_SME_With_ST");
    m_marketInfoMap.insert("沪深300", "SH000300_SecodeList");
    m_marketInfoMap.insert("中证100", "SZ399903_SecodeList");
    m_marketInfoMap.insert("中证200", "SH000904_SecodeList");
    m_marketInfoMap.insert("中证500", "SH000905_SecodeList");
    m_marketInfoMap.insert("中证800", "SH000906_SecodeList");
    m_marketInfoMap.insert("中证1000", "SH000852_SecodeList");
    m_marketInfoMap.insert("上证50",   "SH000016_SecodeList");
}

void ChooseSecodeWindow::initDatabase() {
    int connectID = getDatabaseConnectId();
    m_database = new Database(QString("%1").arg(connectID), m_dbhost);
}

void ChooseSecodeWindow::initTreeView() {
    QList<QString> treeData;
    treeData << "股票" << "    全部A股" << "    上证A股"
             << "    深证A股" << "    深证主板A股"
             << "    创业板"  << "    中小企业板" << "    中小企业板(含ST)"
             << "    沪深300" << "    中证100"  << "    中证200" << "    中证500"
             << "    中证800" << "    中证1000" << "    上证50";
    if (m_bShowIndex) {
        treeData << "指数" << "    沪深指数" << "    中证指数" << "    上证指数";
    }
    QList<QVariant> rootData;
    rootData.append("分类");

    m_treeModel = new TreeModel(treeData, rootData);
    ui->treeView->setModel(m_treeModel);
}

void ChooseSecodeWindow::initCandidateTableView() {
    QStandardItemModel* currItemModel = dynamic_cast<QStandardItemModel*>(ui->candidate_tableView->model ());
    if (currItemModel != NULL) {
        currItemModel->clear();
    } else {
        currItemModel = new QStandardItemModel;
    }

    currItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("代码 [名称]")));
    ui->candidate_tableView->setModel(currItemModel);
    ui->candidate_tableView->setColumnWidth (0, 300);
    ui->candidate_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ChooseSecodeWindow::initSelectTableView() {
    QStandardItemModel* currItemModel = dynamic_cast<QStandardItemModel*>(ui->selected_tableView->model ());
    if (currItemModel != NULL) {
        currItemModel->clear();
    } else {
        currItemModel = new QStandardItemModel;
    }

    currItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("代码 [名称]")));
    ui->selected_tableView->setModel(currItemModel);
    ui->selected_tableView->setColumnWidth (0, 300);
    ui->selected_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

QStringList ChooseSecodeWindow::getCandidateData(QString info) {
    QStringList result;
    if (m_marketInfoMap.contains(info)) {
        QString tableName = m_marketInfoMap[info];
        QList<QStringList> ori_secodeList = m_database->getMarketSecodeList(tableName);
//        printList(ori_secodeList, "ori_secodeList: ");

        for (int i = 0; i < ori_secodeList.size(); ++i) {
            result.append(QString("%1 [%2]").arg(ori_secodeList[i][0]).arg(ori_secodeList[i][1]));
        }
    }

    if (info == "沪深指数") {
        result << "SH000300 [沪深300]" << "SH000908 [沪深300能源]" << "SH000909 [沪深300材料]"
               << "SH000910 [沪深300工业]" << "SH000911 [沪深300可选]" << "SH000912 [沪深300消费]"
               << "SH000913 [沪深300医药]" << "SH000913 [沪深300医药]" << "SH000915 [沪深300信息]"
               << "SH000917 [沪深300公用]" << "SH000951 [300银行]" << "SH000952 [300地产]" << "SH000849 [300非银]";
    }

    if (info == "中证指数") {
        result << "SZ399903 [中证100]" << "SH000904 [中证200]"
               << "SH000905 [中证500]" << "SH000906 [中证800]" << "SH000852 [中证1000]";
    }

    if (info == "上证指数") {
        result << "SH000016 [上证50]";
    }
    return result;
}

void ChooseSecodeWindow::setCandidateTableView(QStringList dataList) {
    QStandardItemModel* currItemModel = dynamic_cast<QStandardItemModel*>(ui->candidate_tableView->model ());
    if (currItemModel != NULL) {
        currItemModel->clear();
    } else {
        currItemModel = new QStandardItemModel;
    }

    for (int i = 0; i<dataList.size(); ++i) {
        currItemModel->setItem(i, 0,  new QStandardItem(dataList[i]));
    }

    currItemModel-> setHorizontalHeaderItem (0, new QStandardItem(QString("代码 [名称]")));
    ui->candidate_tableView->setModel(currItemModel);
    ui->candidate_tableView->setColumnWidth (0, 300);
    ui->candidate_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ChooseSecodeWindow::on_treeView_clicked(const QModelIndex &index)
{
//    QString data = m_treeModel->data(index).toString();
//    qDebug() << "data: " << data;
}

void ChooseSecodeWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString data = m_treeModel->data(index).toString();
    qDebug() << "data: " << data;

    QStringList candidateData = getCandidateData(data);
    setCandidateTableView(candidateData);
}

void ChooseSecodeWindow::on_candidate_tableView_doubleClicked(const QModelIndex &index)
{
    QAbstractItemModel* currModel = ui->candidate_tableView->model ();
    QString selectData = currModel->data(index).toString();
    currModel->removeRow(index.row());
    addTableItem(ui->selected_tableView, selectData);
}

void ChooseSecodeWindow::on_chooseAllCode_pushButton_clicked()
{
    QAbstractItemModel* currModel = ui->candidate_tableView->model ();
    for (int i = 0; i < currModel->rowCount(); ++i) {
        QString curData = currModel->data(currModel->index(i,0)).toString();
        addTableItem(ui->selected_tableView, curData);
    }
    currModel->removeRows(0, currModel->rowCount());
}

void ChooseSecodeWindow::on_chooseSecode_pushButton_clicked()
{
    QMap<int, int> selectRowColumn = getSelectRows(ui->candidate_tableView);
    QStringList selectData = getSelectTableData(ui->candidate_tableView, selectRowColumn);
    for (QString data:selectData) {
        addTableItem(ui->selected_tableView, data);
    }
    QList<int> selectRows = selectRowColumn.keys();
    for(int row:selectRows) {
        ui->candidate_tableView->model ()->removeRow(row);
    }
}

void ChooseSecodeWindow::on_returnAllCode_pushButton_clicked()
{
    QAbstractItemModel* currModel = ui->selected_tableView->model ();
    for (int i = 0; i < currModel->rowCount(); ++i) {
        QString curData = currModel->data(currModel->index(i,0)).toString();
        addTableItem(ui->candidate_tableView, curData);
    }
    currModel->removeRows(0, currModel->rowCount());
}

void ChooseSecodeWindow::on_returnSecode_pushButton_clicked()
{
    QMap<int, int> selectRowColumn = getSelectRows(ui->selected_tableView);
    QStringList selectData = getSelectTableData(ui->selected_tableView, selectRowColumn);
    for (QString data:selectData) {
        addTableItem(ui->candidate_tableView, data);
    }
    QList<int> selectRows = selectRowColumn.keys();
    for(int row:selectRows) {
        ui->selected_tableView->model ()->removeRow(row);
    }
}

void ChooseSecodeWindow::on_sendCodeList_pushButton_clicked()
{
    QStringList result;
    QAbstractItemModel* currModel = ui->selected_tableView->model ();
    for (int i = 0; i < currModel->rowCount(); ++i) {
        QString curData = currModel->data(currModel->index(i,0)).toString();
        QString code = curData.split(" ")[0];
        result.append(code);
    }
    printList(result, "Choosed SecodeList: ");
    emit get_secodeList_signal(result);
    this->close();
}
