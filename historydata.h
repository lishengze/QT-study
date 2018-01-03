#ifndef HISTORYDATA_H
#define HISTORYDATA_H

#include "dataread.h"
#include "dataprocess.h"

class HistoryData : public QObject
{
    Q_OBJECT
public:
    explicit HistoryData(QObject *parent = 0);

signals:

public slots:
};

#endif // HISTORYDATA_H
