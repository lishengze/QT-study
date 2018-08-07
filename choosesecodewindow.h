#ifndef CHOOSESECODEWINDOW_H
#define CHOOSESECODEWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "database.h"
#include "treemodel.h"

namespace Ui {
class ChooseSecodeWindow;
}

class ChooseSecodeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChooseSecodeWindow(QWidget *parent = 0);
     ChooseSecodeWindow(QString dbhost, QWidget *parent = 0);
    ~ChooseSecodeWindow();

    void initMarketInfoMap();
    void initDatabase();
    void initTreeView();
    void initCandidateTableView();
    void initSelectTableView();
    QStringList getCandidateData(QString info);
    void setCandidateTableView(QStringList dataList);


private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_candidate_tableView_doubleClicked(const QModelIndex &index);

    void on_chooseAllCode_pushButton_clicked();

    void on_chooseSecode_pushButton_clicked();

    void on_returnAllCode_pushButton_clicked();

    void on_returnSecode_pushButton_clicked();

    void on_sendCodeList_pushButton_clicked();

signals:

    void get_secodeList_signal(QStringList);

private:
    Ui::ChooseSecodeWindow *ui;

    QMap<QString, QString> m_marketInfoMap;
    QString                m_dbhost;
    QString                m_databaseName;
    Database*              m_database;
    TreeModel*             m_treeModel;

};

#endif // CHOOSESECODEWINDOW_H
