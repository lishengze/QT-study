#ifndef UPDATEREALTIMEDATAFORM_H
#define UPDATEREALTIMEDATAFORM_H

#include <QWidget>
#include <QThread>
#include "strategymodel.h"
#include "database.h"
#include "excel.h"

#include "realtimedataread.h"
#include "realtimedatabase.h"

namespace Ui {
class UpdateRealtimeDataForm;
}

class UpdateRealtimeDataForm : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateRealtimeDataForm(QWidget *parent = 0);

    void connectSignal();
    void setTableView();

    ~UpdateRealtimeDataForm();

signals:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);

public slots:
    void loginWindFailed(int errcode);
    void loginWindSucc();

private slots:
    void on_startGetRealtimeData_clicked();

private:
    Ui::UpdateRealtimeDataForm *ui;

    QString m_strategyFileDir;

    bool m_loginWind;
    QThread m_windWorkThread;
    QThread m_getSnapshootDataThread;
    RealTimeDataRead* m_readRealTimeData;

};

#endif // UPDATEREALTIMEDATAFORM_H
