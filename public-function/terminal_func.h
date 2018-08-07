#ifndef TERMINAL_FUNC_H
#define TERMINAL_FUNC_H
#include <QString>
#include <QList>

QList<QString> getEXCELPidList();

QList<QString> getProcessPid(QString taskList, QString processName);

void killProcessByPid(QString pid);

#endif // TERMINAL_FUNC_H
