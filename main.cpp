#include <QCoreApplication>
#include "counter.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Counter a("a"), b("b"), c("c");
    QObject::connect (&a, SIGNAL(valueChanged (int)), &b, SLOT(valueChanged (int)));
    QObject::connect (&a, SIGNAL(valueChanged (int)), &c, SLOT(setValue (int)));
    qDebug() << "First : b.value is" << b.value () ;
    a.setValue (12);
    qDebug() << "Second : b.value is" << b.value () ;
    qDebug() << "Second : b.value is" << c.value () ;

    return app.exec();
}
