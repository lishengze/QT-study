﻿#include <QApplication>
#include <QTextCodec>
#include <QProcess>

#include "widget.h"
#include "test.h"
#include "testform.h"
#include "processfunc.h"
#include "dialog.h"
#include "generateportfolioform.h"
#pragma execution_character_set("utf-8")

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}

int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);

//    Widget spreadWindow;
//    spreadWindow.setWindowTitle("Client");
//    spreadWindow.show();

    GeneratePortfolioForm genePorfolioWindow;
    genePorfolioWindow.setWindowTitle("生成组合客户端");
    genePorfolioWindow.show();

//    testMain();

    return a.exec();
}

