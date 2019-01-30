#ifndef DATAREAD_H
#define DATAREAD_H

#include <QThread>
#include <QList>
#include <QObject>
#include <QDebug>
#include <QMap>
#include "database.h"
#include "process_data_func.h"

class DataRead: public QObject
{
    Q_OBJECT
public:
    DataRead(QString dbhost, QString dbName, QStringList tableNameList,
             QString startDate="", QString endDate="", QStringList keyValueList=EmpytStringList(),
             QObject *parent = Q_NULLPTR);

    ~DataRead();

    QMap<QString, QList<QStringList>> readHistoryData();
    QMap<QString, QList<QStringList>> readRealTimeData();

public slots:
    void getOrinPortfolioData_slot(QString dataType);

signals:
    void sendHistoryData(QMap<QString, QList<QStringList>> historyData);
    void sendRealTimeData(QMap<QString, QList<QStringList>> realtimeData);

private:
    Database* m_database;

    QString m_dbName;
    QStringList m_tableNameList;
    QString m_startDate;
    QString m_endDate;
    QStringList m_keyValueList;

    QMap<QString, QList<QStringList>> m_result;
};

#endif // DATAREAD_H
