#include "subwriteclass.h"
#include "toolfunc.h"
#include <QThread>
#include <QDebug>


SubWriteClass::SubWriteClass(QMap<QString, QStringList> data, QString connectId, QString dbName, QString dbhost):
    QObject(0), m_realtimeDatabase(NULL), m_oriData(data)
{
    m_realtimeDatabase = new RealTimeDatabase(connectId, dbName, dbhost);
}

SubWriteClass::~SubWriteClass() {

    if (NULL != m_realtimeDatabase) {
        delete m_realtimeDatabase;
        m_realtimeDatabase = NULL;
    }
//    qDebug() << "Release database: " << m_realtimeDatabase->getDatabaseName();
}

void SubWriteClass::writeRealTimeData_slot() {
//    qDebug() << "SubWriteClass::writeRealTimeData_slot, thread: " << QThread::currentThreadId();
//    qDebug() << m_oriData;
    QList<QString> result;
    int succNumb = 0;
    for (QMap<QString, QStringList>::iterator it = m_oriData.begin();
         it != m_oriData.end(); ++it) {
        QString msg = m_realtimeDatabase->insertData(it.key(), it.value());
        if (msg != "SUCCESS") {
            result.append(msg);
        } else {
            ++succNumb;
        }
    }
//    result.append(QString("%1").arg(m_oriData.size()));
    result.append(QString("%1").arg(succNumb));
    emit writeRealTimeResult_signal(result);
}
