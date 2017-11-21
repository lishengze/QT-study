#include "widget.h"
#include "chartdialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

//    chartDialog dialog;
//    dialog.show ();

    return a.exec();
}
