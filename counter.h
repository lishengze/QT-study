#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>
#include <QDebug>
#include <string>
using std::string;

class Counter:public QObject {
    Q_OBJECT
private:
    int m_value;
    string m_objName;
signals:
    void valueChanged(int value);
public slots:
    void setValue(int value);

public:
    int value();
    Counter(string objName="default");
    ~Counter();
};

#endif // COUNTER_H
