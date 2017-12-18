#ifndef TEST_H
#define TEST_H
#include <QObject>
#include <QWidget>

class Test: public QObject
{
    Q_OBJECT
public:
    Test(QObject* parent=0);
    bool m_login;

public slots:
    void login();
    int testWset();
    int testWsd ();
    int testWsq();
private:
    unsigned long long m_reqid;
};

#endif // TEST_H
