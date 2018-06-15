#ifndef WIDGET_FUNC_H
#define WIDGET_FUNC_H
#include <QTableView>

QMap <int, int> getSelectRows(QTableView* tableView);

QList<QString> getSelectTableData(QTableView* tableView, QMap<int, int> selectInfo);

#endif // WIDGET_FUNC_H
