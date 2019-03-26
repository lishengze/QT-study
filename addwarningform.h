#ifndef ADDWARNINGFORM_H
#define ADDWARNINGFORM_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include "datastruct.h"

namespace Ui {
class AddWarningForm;
}

bool compare(double val1, double val2, QString compare);

class AddWarningForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddWarningForm(QWidget *parent = 0);

    AddWarningForm(WarningParam warningParam,
                   QWidget* parent = 0);

    ~AddWarningForm();

    void initWidget();
    void initMenu();    
    void initChooseWidgets();
    void initInfoTable();

    void initCommonData();
    void initParamMap();

    void updateAddInfoTable(int id, QString info);
    void updateWaringInfoTable(int id, QString info);
    

    void checkWaringItem(QList<double> aveList, QList<double> cssList,
                         int dataID);

private slots:

    void deleteWaringItem_slot();

    void show_addInfoTableContextMenu_slot(QPoint);

    void on_pushButton_clicked();

    void on_compTarget_radioButton_clicked(bool checked);

    void on_compValue_radioButton_clicked(bool checked);

    void on_compTarget_comboBox_activated(const QString &arg1);

    void on_compValue_doubleSpinBox_valueChanged(double arg1);

    void on_compValue_doubleSpinBox_editingFinished();

private:
    Ui::AddWarningForm *            ui;

    WarningParam                    m_warningParam;
    QMap<int, WaringItem>           m_waringItemMap;
    int                             m_waringItemCount;

    int                             m_waringItemIDPos;

    QMenu*                          m_menu;
    QAction*                        m_deleteAction;
};

#endif // ADDWARNINGFORM_H
