#include "widget.h"
#include <QApplication>
#include "test.h"
#include "testform.h"
#include <QTextCodec>

void setCode() {
//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForLocale(codec);
//    QTextCodec::codecForName("UTF-8")->toUnicode("中文");

    QTextCodec::codecForName("GBK")->toUnicode("中文");
}

int main(int argc, char *argv[])
{
    setCode();
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
//    testGetNumbList();
    return a.exec();
}

