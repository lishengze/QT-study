#ifndef SUBWRITECLASS_H
#define SUBWRITECLASS_H

#include <QTableView>
#include "realtimedatabase.h"

class SubWriteClass:public QObject
{
    Q_OBJECT
public:
    SubWriteClass(QMap<QString, QStringList> data, QString connectId, QString dbName, QString dbHost);
    ~SubWriteClass();

private:
     RealTimeDatabase*   m_realtimeDatabase;
     QMap<QString, QStringList> m_oriData;

signals:
     void writeRealTimeResult_signal(QList<QString> result);

public slots:
     void writeRealTimeData_slot();
};

#endif // SUBWRITECLASS_H
