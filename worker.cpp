#include "worker.h"

Worker::Worker(QObject *parent):QObject(parent)
{

}

void Worker::processData (QList<int> oriData) {
    QList<int> processedData;
    for (int i = 0; i < oriData.size (); ++i) {
        processedData.append (oriData[i] * 2);
    }
    emit sendProcessedData (processedData);
}

