#ifndef REALTIMEDATAREAD_H
#define REALTIMEDATAREAD_H

#include <QObject>

class RealTimeDataRead:public QObject
{
    Q_OBJECT
public:
    RealTimeDataRead(QObject* parent = 0);
public slots:
    void loginWind();
    void startWsq(QStringList secodeList);

signals:
    void loginWindFailed(int errcode);
    void loginWindSucc();
    void startWsqFailed(int errcode);
    void startWsqSucc();

private:
    bool m_login;
};

#endif // REALTIMEDATAREAD_H
