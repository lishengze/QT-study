#ifndef STRATEGYMODEL_H
#define STRATEGYMODEL_H

#include <QString>
#include <QList>
#include <QStandardItemModel>
#include "strategy.h"

class StrategyModel
{
public:
    StrategyModel();
    StrategyModel(QString strategyFileDir);
    void setTableModel();
    QStandardItemModel* getTableModel();
    QList<QString> getStrategyFileList();
    QString getStrategyFullFileName(int index);
private:
    QString m_strategyDirName;
    QList<QString> m_strategyFileList;
    QStandardItemModel* m_tableModel;
};

#endif // STRATEGYMODEL_H
