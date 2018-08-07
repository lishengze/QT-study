#include "terminal_func.h"
#include <QProcess>
#include <QDebug>

QList<QString> getEXCELPidList() {
    QProcess p(0);
    p.start ("tasklist");
    p.waitForStarted();
    p.waitForFinished();

    QString taskStr = QString::fromLocal8Bit(p.readAllStandardOutput());
    QString processName = "EXCEL.EXE";
    return getProcessPid(taskStr, processName);
}

QList<QString> getProcessPid(QString taskStr, QString processName) {
    QStringList taskList = taskStr.split ("\r\n");
    QList<QString> result;
    for (int i = 0; i < taskList.size (); ++i) {
       if (taskList[i].startsWith(processName)) {
           QStringList tmpStrList = taskList[i].split(" ");
           tmpStrList.removeAll ("");
           result.append (tmpStrList[1]);
       }
    }
    return result;
}

void killProcessByPid(QString pid) {
    QProcess p(0);
    p.start ("TASKKILL /F /T /pid " + pid);
    p.waitForStarted();
    p.waitForFinished();

    QString taskStr = QString::fromLocal8Bit(p.readAllStandardOutput());
}
