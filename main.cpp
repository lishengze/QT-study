#include "widget.h"
#include <QApplication>
#include "test.h"
#include "testform.h"
#include <QTextCodec>
#include <QProcess>
#include "processfunc.h"

void setCode() {
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::codecForName("UTF-8")->toUnicode("中文");

    QTextCodec::codecForName("GBK")->toUnicode("中文");
}

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}

int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);
//    Widget w;
//    w.show();

    testMain();
    return a.exec();
}

