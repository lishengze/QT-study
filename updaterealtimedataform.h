#ifndef UPDATEREALTIMEDATAFORM_H
#define UPDATEREALTIMEDATAFORM_H

#include <QWidget>
#include <QThread>
#include "strategymodel.h"
#include "database.h"

#include "realtimedataread.h"
#include "realtimedatabase.h"

#include "excel.h"

namespace Ui {
class UpdateRealtimeDataForm;
}

class UpdateRealtimeDataForm : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateRealtimeDataForm(QWidget *parent = 0);

    void initDatabase();
    void initWidget();
    void setSecodeList();
    void connectSignal();
    void setTableView();
    void registeParams();

    ~UpdateRealtimeDataForm();

signals:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);


public slots:
    void loginWindFailed(int errcode);
    void loginWindSucc();
    void stopTimer();

private slots:
    void on_startGetRealtimeData_clicked();

    void on_stopGetRealtimeData_clicked();

private:
    Ui::UpdateRealtimeDataForm *ui;

    QList<QString> m_secodeList;
    QList<QString> m_strategyFileList;
    StrategyModel* m_strategyModel;
    QString m_strategyFileDir;
    Excel m_excel;
     RealTimeDatabase* m_realtimeDatabase;

    bool m_loginWind;
    QThread m_windWorkThread;
    QThread m_getSnapshootDataThread;
    RealTimeDataRead* m_readRealTimeData;

};

#endif // UPDATEREALTIMEDATAFORM_H
