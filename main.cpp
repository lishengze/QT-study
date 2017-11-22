#include "widget.h"
#include "chartdialog.h"
#include <QApplication>
#include <test.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

//    chartDialog dialog;
//    dialog.show ();

//    testReadExcelData ();
//    testGetExcelFileName();
    return a.exec();
}
