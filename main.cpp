#include "dialog.h"
#include <QApplication>
#include <QDebug>

#include <QDebug>
#include <iostream>
using namespace std;

#include "WAPIWrapperCpp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();
}
