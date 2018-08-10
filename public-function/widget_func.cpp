#include "widget_func.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QDateTime>
#include <QModelIndexList>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QCheckBox>

QMap <int, int> getSelectRows(QTableView* tableView)
{
    QItemSelectionModel *selections =  tableView->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    QMap<int, int> result;
    foreach (QModelIndex index, selected)
    {
        result.insert(index.row(),index.column());
    }
   return result;
}

QList<QString> getSelectTableData(QTableView* tableView, QMap<int, int> selectInfo) {
    QList<QString> result;
    QAbstractItemModel* itemModel = tableView->model ();
    for (QMap<int, int>::iterator it = selectInfo.begin();
         it != selectInfo.end(); ++it) {
        QModelIndex indextemp = itemModel->index(it.key(), it.value());
        QString data = itemModel->data(indextemp).toString();
        result.append(data);
    }
    return result;
}

void addTableItem(QTableView* tableView,QString message) {
    QStandardItemModel* tableModel = dynamic_cast<QStandardItemModel*>(tableView->model ());
    if (NULL != tableModel) {
        int row = tableModel->rowCount ();
        tableModel->setItem (row, 0,  new QStandardItem(message));
    }
}

void updateProgramInfo(QTableView* programInfoTableView, QString message, QString remark) {
    QStandardItemModel* testMode = dynamic_cast<QStandardItemModel*>(programInfoTableView->model ());
    if (NULL != testMode) {
        QStringList messageList = message.split("\n");
        messageList.removeAll("");
        messageList.removeAll(" ");
        for (QString msg:messageList) {
            QString datetime = QDateTime::currentDateTime ().toString ("yyyy/MM/dd hh:mm:ss");
            int row = testMode->rowCount ();
            testMode->setItem (row, 0,  new QStandardItem(datetime));
            testMode->setItem (row, 1,  new QStandardItem(msg));
            testMode->setItem (row, 2,  new QStandardItem(remark));
            QScrollBar* verScrollBar = programInfoTableView->verticalScrollBar();
            verScrollBar->setSliderPosition(verScrollBar->maximum());
        }
    }
}

QStandardItemModel* getStandardItemModel(QList<QString> valueList) {
    QStandardItemModel* standardItemModel = new QStandardItemModel();
    for (int i = 0; i < valueList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(valueList[i]));
    }
    return standardItemModel;
}

QStandardItemModel* getStandardItemModel(QList<QFileInfo> fileInfoList) {
    QStandardItemModel* standardItemModel = new QStandardItemModel();
    for (int i = 0; i < fileInfoList.size(); ++i) {
        standardItemModel->setItem(i, 0, new QStandardItem(fileInfoList[i].fileName()));
    }
    return standardItemModel;
}

QStringList getComboBoxKeyList(QListWidget* currWidget) {
    QStringList result;
    for (int i = 0; i < currWidget->count(); ++i)
    {
        QListWidgetItem *pItem = currWidget->item(i);
        QWidget *pWidget = currWidget->itemWidget(pItem);
        QCheckBox *pCheckBox = (QCheckBox *)pWidget;
        if (pCheckBox->isChecked())
        {
            QString strText = pCheckBox->text();
            result.append(strText);
        }
    }
    return result;
}
