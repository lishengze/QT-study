#ifndef DATABASE_FUNC_H
#define DATABASE_FUNC_H

#include <QList>
#include <QString>
#include <QDate>

QList<int> getOneDayTimeList(QString host, QString database_name);

QDate getRealtimeDate(QString host);

#endif // DATABASE_FUNC_H
