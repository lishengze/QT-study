#include "widget_func.h"
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QModelIndex>

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
