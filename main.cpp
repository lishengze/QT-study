#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QDebug>
//#include "test.h"
//#include "testform.h"
//#include "processfunc.h"
//#include "dialog.h"
//#include "widget.h"

#include "updaterealtimedataform.h"

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}

int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);
//    Widget w;
//    w.setWindowTitle("Client");
//    w.show();

//    Dialog testDialog(8);
//    testDialog.show ();

    UpdateRealtimeDataForm UpDataWindow;
    UpDataWindow.setWindowTitle("UpData");
    UpDataWindow.show();

//    testMain();
    return a.exec();
}

