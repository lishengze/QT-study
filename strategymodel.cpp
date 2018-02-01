#include "strategymodel.h"
//#include "setdata.h"
#include "toolfunc.h"
#include <QStandardItem>

StrategyModel::StrategyModel()
{

}

StrategyModel::StrategyModel(QString strategyFileDir):
    m_strategyDirName(strategyFileDir),
    m_tableModel(NULL)
{
    m_strategyFileList = getExcelFileName (m_strategyDirName);
    setTableModel();
}

void StrategyModel::setTableModel () {
    m_tableModel = new QStandardItemModel();
    m_tableModel -> setHorizontalHeaderItem (0, new QStandardItem(QObject::tr("组合名称")));
    for (int i = 0; i < m_strategyFileList.size (); ++i) {
        QString strategyFullFileName = m_strategyFileList.at (i);
        QString strategySimpleFileName = removePathName (strategyFullFileName);
        m_tableModel ->setItem (i, 0, new QStandardItem(strategySimpleFileName));
    }
}

QStandardItemModel* StrategyModel::getTableModel() {
    return m_tableModel;
}

QList<QString> StrategyModel::getStrategyFileList () {
    return m_strategyFileList;
}

QString StrategyModel::getStrategyFullFileName(int index) {
    return m_strategyFileList.at (index);
}
