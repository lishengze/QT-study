#ifndef UPDATEREALTIMEDATAFORM_H
#define UPDATEREALTIMEDATAFORM_H

#include <QWidget>
#include <QThread>
#include <QTimer>
#include "strategymodel.h"
#include "database.h"

#include "realtimedataread.h"
#include "realtimedatabase.h"
#include "realtimedatawrite.h"
#include "excel.h"

namespace Ui {
class UpdateRealtimeDataForm;
}

class UpdateRealtimeDataForm : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateRealtimeDataForm(QWidget *parent = 0);

    void initCommonData();
    void initTableView();
    void initDatasourceCombox();
    void initWidget();
    void initDatabase();


    void initReadWriteObj();
    void registeParams();

    ~UpdateRealtimeDataForm();

signals:
    void loginWind_signal();
    void startWsq(QStringList secodeList, int reqID);

public slots:
    void loginWindFailed_slot();

    void startReadWaitTradeTimer_slot();
    void stopReadWaitTradeTimer_slot();

    void startWriteMonitorTimer_slot();
    void stopWriteMonitorTimer_slot();
    void startReadMonitorTimer_slot();
    void stopReadMonitorTimer_slot();

private slots:
    void on_startDownloadData_Button_clicked();

private:
    Ui::UpdateRealtimeDataForm* ui;

    QList<QString>              m_dbhostList;
    QString                     m_dbHost;
    QString                     m_dbName;
    QString                     m_dbConnID;
    RealTimeDatabase*           m_realtimeDatabase;
    QString                     m_programInfoTableName;
    bool                        m_workingState;

    QThread*                    m_realTimeDataReadThread;
    RealTimeDataRead*           m_realTimeDataRead;

    QThread *                   m_realTimeDataWriteThread;
    RealTimeDataWrite*          m_realTimeDataWrite;
};
//    QTimer                      m_timer;
//    int                         m_updateTime;
//    bool                        m_initSecodeList;

//    QList<QString>              m_secodeList;
//    QList<QString>              m_strategyFileList;
//    StrategyModel*              m_strategyModel;

//    QString                     m_hedgeIndexStrategyFileDir;
//    QString                     m_buySaleStrategyFileDir;
//    Excel                       m_excel;

#endif // UPDATEREALTIMEDATAFORM_H
