#include <QApplication>
#include "extractdataform.h"
#pragma execution_character_set("utf-8")

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}


int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);

    ExtractDataForm extractDataWindow;
    extractDataWindow.setWindowTitle("提取数据");
    extractDataWindow.show();

    return a.exec();
}
