#include "widget.h"
#include <QApplication>
#include "test.h"
#include "testform.h"
#include <QTextCodec>
#include <QProcess>
#include "processfunc.h"
#include "dialog.h"
#include "updatedataform.h"
#include <QDebug>

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}

int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);
    qDebug() << "Main";
//    Widget w;
//    w.setWindowTitle("Client");
//    w.show();

//    Dialog testDialog(8);
//    testDialog.show ();

    UpdateDataForm UpDataWindow;
    UpDataWindow.setWindowTitle("UpData");
    UpDataWindow.show();

//    testMain();
    return a.exec();
}

