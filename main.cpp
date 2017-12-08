#include <QCoreApplication>
#include "test.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    test testObj;
    testObj.testMain ();

    return a.exec();
}
