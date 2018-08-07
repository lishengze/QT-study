#ifndef EXTRACTDATAFORM_H
#define EXTRACTDATAFORM_H

#include <QMap>
#include <QMainWindow>
#include <QFileInfo>
#include <QPoint>
#include <QThread>
#include <QListWidget>
#include <QLineEdit>
#include <QListWidgetItem>
#include "choosesecodewindow.h"
#include "extractmarketdata.h"
#include "extractweightdata.h"
#include "extractindustrydata.h"

namespace Ui {
class ExtractDataForm;
}

class ExtractDataForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExtractDataForm(QWidget *parent = 0);
    void initCommonData();
    void initFileDir();
    void initDataTypeMap();
    void initKeyValueMap();
    void initIndexCodeMap();
    void initIndustryList();

    void initWidget();
    void initDate();
    void initDataSourceCombox();
    void initDataTypeCombox();
    void initKeyValueCombox();
    void initMarketKeyComboBox();
    void initIndexCodeComboBox();
    void initIndustryComboBox();
    void checkAllIndustry();
    void initProgramTable();

    void setOriFileTable();
    void setDesFileTable();

    void initTableContextMenu();

    void basicTest();
    QStringList checkMarketSecodeList(QStringList oriSecodeList, QString dbhost, QString dbName);
    QStringList getCurrKeyValueList();    
    QStringList getCurrIndexCodeList();
    QStringList getCurrIndustryList();
    ~ExtractDataForm();

public slots:
    void extractDataComplete_slot(QStringList);
    void get_secodeList_slot(QStringList);

private slots:

    void on_chooseOriFileDir_pushButton_clicked();

    void on_chooseDesFileDir_pushButton_clicked();

    void show_oriFileTable_contextMenu(QPoint pos);

    void show_desFileTable_contextMenu(QPoint pos);

    void refresh_oriFileTable();

    void refresh_desFileTable();

    void delete_oriFile();

    void delete_desFile();

    void open_oriDir();

    void open_desDir();

    void on_oriFile_tableView_clicked(const QModelIndex &index);

    void on_oriFile_tableView_doubleClicked(const QModelIndex &index);

    void on_desFile_tableView_doubleClicked(const QModelIndex &index);

    void stateChanged(int state);

    void industryComboxStateChange(int state);

    void on_extractWeightData_pushButton_clicked();

    void on_extractIndustryData_pushButton_clicked();

    void on_extractMarketData_pushButton_clicked();

    void on_chooseSecodeListFromExcel_pushButton_clicked();

    void on_chooseSecodeListFromTable_pushButton_clicked();

private:
    Ui::ExtractDataForm *       ui;

    int                         m_extractThreadCount;
    QList<QString>              m_dbhostList;
    QStringList                 m_marketSecodeList;
    QStringList                 m_keyValueList;
    QStringList                 m_indexCodeList;
    QStringList                 m_industryList;

    QList<ExtractMarketData*>   m_extractMarketDataObjList;
    QList<ExtractWeightData*>   m_extractWeightDataObjList;
    QList<ExtractIndustryData*> m_extractIndustryDataObjList;

    QMap<QString, QString>      m_dataTypeMap;
    QMap<QString, QString>      m_keyValueMap;
    QMap<QString, QString>      m_indexCodeMap;


    QString                     m_nativeFileName;
    QString                     m_oriFileDir;
    QList<QFileInfo>            m_oriFileList;
    QString                     m_currOriFile;
    QString                     m_desFileDir;
    QList<QFileInfo>            m_desFileList;

    int                         m_extractTimes;

    QListWidget*                m_marketKeyListWidget;
    QListWidget*                m_weightIndexListWidget;
    QListWidget*                m_industryListWidget;

    bool                        m_setIndustryCombox;

    ChooseSecodeWindow*         m_chooseSecodeWindow;
};

#endif // EXTRACTDATAFORM_H
