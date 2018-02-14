#include "widget.h"
#include <QApplication>
#include "test.h"
#include "testform.h"
#include <QTextCodec>
#include <QProcess>
#include "processfunc.h"
#include "dialog.h"
#include "generateportfolioform.h"

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

    GeneratePortfolioForm genePorfolioWindow;
    genePorfolioWindow.setWindowTitle("生成组合");
    genePorfolioWindow.show();

//    Dialog testDialog(8);
//    testDialog.show ();

//    testMain();
    return a.exec();
}

