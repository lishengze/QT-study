#include "widget.h"
#include <QApplication>
#include "test.h"
#include "testform.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

//    TestForm testForm;
//    testForm.show();

//    chartDialog dialog;
//    dialog.show ();

//    testReadExcelData ();
//    testGetExcelFileName();
//    testRemovePathName();
//    testExcel();
//    testSortPointList ();
//    testMergeSortedList();
//    testTableData();
//    testComputeMACD ();
//    testTranstime();
    return a.exec();
}
