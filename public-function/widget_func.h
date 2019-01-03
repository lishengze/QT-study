﻿#ifndef WIDGET_FUNC_H
#define WIDGET_FUNC_H
#include <QTableView>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QListWidget>
//#include <QCategoryAxis>

QMap <int, int> getSelectRows(QTableView* tableView);

QList<QString> getSelectTableData(QTableView* tableView, QMap<int, int> selectInfo);

void addTableItem(QTableView* tableView,QString message);

void updateProgramInfo(QTableView* programInfoTableView, QString message, QString remark="");

QStandardItemModel* getStandardItemModel(QList<QString> valueList);

QStandardItemModel* getStandardItemModel(QList<QFileInfo> valueList);

QStringList getComboBoxKeyList(QListWidget* currWidget);

//QCategoryAxis* getTimeAxisX(QList<QString> timeList, int tickCount);

#endif // WIDGET_FUNC_H