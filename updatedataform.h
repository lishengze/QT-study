#ifndef UPDATEDATAFORM_H
#define UPDATEDATAFORM_H

#include <QWidget>
#include <QThread>
#include "strategymodel.h"
#include "database.h"
#include "excel.h"

#include "realtimedataread.h"

namespace Ui {
class UpdateDataForm;
}

class UpdateDataForm : public QWidget
{
    Q_OBJECT

public:
//    explicit UpdateDataForm(QWidget *parent = 0);
    explicit UpdateDataForm(QString strategyFileDir = "//192.168.211.182/1分钟数据 20160910-20170910",
                   QWidget *parent = 0);

    void connectSignal();

    void setTableView();

signals:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);

    ~UpdateDataForm();

public slots:
    void loginWindFailed(int errcode);
    void loginWindSucc();

private:
    Ui::UpdateDataForm *ui;

    StrategyModel* m_strategyModel;
    QString m_strategyFileDir;

    bool m_loginWind;
    QThread m_windWorkThread;
    RealTimeDataRead* m_readRealTimeData;
    Excel* m_excel;
};

#endif // UPDATEDATAFORM_H
