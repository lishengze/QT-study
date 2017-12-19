#ifndef REALTIMEDATAPROCESS_H
#define REALTIMEDATAPROCESS_H
#include <QObject>
#include <QStringList>
#include <QMap>
#include <QString>

class RealTimeDataProcess:public QObject
{
    Q_OBJECT
public:
    RealTimeDataProcess(QMap<QString, int> seocdebuyCountMap, QObject* parent=0);

public slots:
    void loginWind();
    void startWsq(QStringList secodeList);
private:
    QMap<QString, int> m_seocdebuyCountMap;
    QStringList m_secodeNameList;
};

#endif // REALTIMEDATAPROCESS_H
