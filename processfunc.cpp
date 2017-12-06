#include <QProcess>
#include <QDebug>
#include "toolfunc.h"
#include "process.h"

void basicCmd() {
    QProcess p(0);
    p.start("cmd", QStringList()<<"/c"<<"ping www.baidu.com");
    p.waitForStarted();
    p.waitForFinished();
    QString strResult=QString::fromLocal8Bit(p.readAllStandardOutput());
//    QString strTemp=p.readAllStandardOutput();

    qDebug () << strResult;
}

void printTaskList()  {
    QProcess p(0);
//    p.start("cmd", QStringList()<<"/c"<<"ping www.baidu.com");
//    p.start ("tasklist|find /i EXCEL.EXE");
//    p.start("tasklist", QStringList()<<"|find"<<"/i"<<"EXCEL.EXE");
    p.start ("tasklist");
    p.waitForStarted();
    p.waitForFinished();

    QString taskStr = QString::fromLocal8Bit(p.readAllStandardOutput());
//    qDebug() << taskStr;
    QString processName = "EXCEL.EXE";
    qDebug() << getProcessPid(taskStr, processName);
}
