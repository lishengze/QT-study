#include "widget.h"
#include <QApplication>
#include "test.h"
#include "testform.h"
#include <QTextCodec>
#include <QProcess>

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
//    setCode();
    setLibPath();
    QApplication a(argc, argv);
//    Widget w;
//    w.show();

//    TestForm testForm;
//    testForm.show();

//    chartDialog dialog;
//    dialog.show ();

    testReadExcelData ();
//    testGetExcelFileName();
//    testRemovePathName();
//    testExcel();
//    testReadStrategyDataFromExcel();
//    testSortPointList ();
//    testMergeSortedList();
//    testTableData();
//    testComputeMACD ();
//    testTranstime();
//    testGetNumbList();
//    testConnectDatabase();
//    testDBClass();
    QProcess process;
    process.start("TASKKILL /F /T /im EXCEL.EXE");
    return a.exec();
}

