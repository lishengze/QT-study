#ifndef TEST_H
#define TEST_H
#include <QObject>

class Test: public QObject
{
    Q_OBJECT
public:
    Test(QObject* parent=0);

public slots:
    void login();
    int testWset();
    int testWsd ();
};

#endif // TEST_H
