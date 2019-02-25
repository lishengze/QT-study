#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QThread>

#include "widget.h"
#include "test.h"

#pragma execution_character_set("utf-8")

void setLibPath()
{
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}

int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);
    Widget spreadWindow;

    bool isTest = false;
    if (isTest)
    {
        testMain();
    }
    else
    {
        spreadWindow.setWindowTitle("点差客户端");
        spreadWindow.show();
    }
    return a.exec();
}

