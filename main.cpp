#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include "test.h"
#include "updaterealtimedataform.h"
#include "extractdataform.h"

#pragma execution_character_set("utf-8")

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}

bool isDatabaseWorking() {
//    QString dbName ="MarketData_RealTime";
    QString dbName ="Test";
    QString dbHost = "192.168.211.165";
    QString dbConnID = "0";
    RealTimeDatabase* realtimeDatabase = new RealTimeDatabase(dbConnID, dbName, dbHost);
    QString programInfoTableName = "ProgramInfo";

    if (1 == realtimeDatabase->getDatabaseWorkingFlag(programInfoTableName)) {
        return true;
    } else {
        return false;
    }
}

int main(int argc, char *argv[])
{

    setLibPath();
    QApplication a(argc, argv);

//    if (isDatabaseWorking()) {
//        QMessageBox::warning(NULL, "Error", "已有客户端在更新数据");
//        return a.exit(0);
//    } else {
//        UpdateRealtimeDataForm UpDataWindow;
//        UpDataWindow.setWindowTitle("UpData");
//        UpDataWindow.show();
//        return a.exec();
//    }

    UpdateRealtimeDataForm UpDataWindow;
    UpDataWindow.setWindowTitle("更新实时数据");
    UpDataWindow.show();

//    ExtractDataForm extractDataWindow;
//    extractDataWindow.setWindowTitle("提取数据");
//    extractDataWindow.show();

//    Test test_obj;

    return a.exec();
}

