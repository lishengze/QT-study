#ifndef REALTIMEDATAREAD_H
#define REALTIMEDATAREAD_H
#include <QTableView>
#include <QObject>

class RealTimeDataRead:public QObject
{
    Q_OBJECT
public:
    RealTimeDataRead(QTableView* programInfoTableView,QObject* parent = 0);

public slots:
    void loginWind();
    void startWsq(QStringList secodeList, int reqID);
    void cancelWsqRequest(int reqID);
    void cancelAllWsqRequest();
    void getPreData(QList<QString> secodeList);

signals:
    void loginWindFailed(int errcode);
    void loginWindSucc();
    void startWsqFailed(int errcode, int reqID);
    void startWsqSucc();
    void sendPreData(QMap<QString,QStringList>);

private:
    bool m_login;
    QTableView* m_programInfoTableView;
};

#endif // REALTIMEDATAREAD_H
