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
    void initWidget();

//    void initDatabase();

    void connectSignal();

    void registeParams();

//    bool setSecodeList();
//    void startCheckSecodeTimer();
//    void stopCheckSecodeTimer();

    ~UpdateRealtimeDataForm();

signals:
    void loginWind_signal();
    void startWsq(QStringList secodeList, int reqID);

public slots:
    void loginWindFailed_slot();

    void startReadRealTimeData_slot();
    void stopReadRealTimeData_slot();

//    void loginWindSucc_slot();
//    void checkSecodeList();

private slots:
//    void on_startGetRealtimeData_clicked();

//    void on_stopGetRealtimeData_clicked();

private:
    Ui::UpdateRealtimeDataForm *ui;

    QString             m_dbHost;
    QString             m_dbName;

    QThread             m_realTimeDataReadThread;
    RealTimeDataRead*   m_realTimeDataRead;

    QThread             m_realTimeDataWriteThread;
    RealTimeDataWrite*  m_realTimeDataWrite;

    QTimer          m_timer;
    int             m_updateTime;
    bool            m_initSecodeList;

    QList<QString>  m_secodeList;
    QList<QString>  m_strategyFileList;
    StrategyModel*  m_strategyModel;

    QString         m_hedgeIndexStrategyFileDir;
    QString         m_buySaleStrategyFileDir;
    Excel           m_excel;
    bool            m_loginWind;
};

#endif // UPDATEREALTIMEDATAFORM_H
