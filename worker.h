#ifndef WORKER_H
#define WORKER_H
#include <QList>
#include <QStringList>
#include <QObject>

class Worker: public QObject
{
    Q_OBJECT
public:
    Worker(QObject *parent = Q_NULLPTR);

public slots:
    void processData(QList<int> oriData);

signals:
    void sendProcessedData(QList<int> processedData);
};

#endif // WORKER_H
