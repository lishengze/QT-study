#ifndef MONITORREALTIMEDATA_H
#define MONITORREALTIMEDATA_H
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "datastruct.h"
#include "toolfunc.h"
#include "macd.h"
#include "chartdata.h"
#include "database.h"
#include "process_data_func.h"

class MonitorRealTimeData : public QObject
{
  Q_OBJECT
public:

  MonitorRealTimeData(DatabaseParam dbParam, HedgedParam hedgedParam, 
                      QObject *parent = 0);                         

  MonitorRealTimeData(QString dbhost, QString futureName,
                      QObject *parent = 0);

  ~MonitorRealTimeData();

  void initCommonData();
  void initIndexHedgeMetaInfo();
  void initDatabase();
  void initTimer();

  void startTimer();
  void stopTimer();

  void addMacd(MACD initMacdData);

  bool checkPortfolioData(QMap<QString, QStringList> realTimeData);
  QList<double> computeSpreadMACDData();
  QList<double> computeSpreadData();

  void readFutureData();
  void readIndexData();
  void readPortfolioData();

  void checkUpdateTime();

signals:

  void sendPreCloseData_signal(double);

  void sendTradeOver();

  void sendRealtimeSpreadMACDData_signal(QList<double>, QMap<QString, double>);
  void sendRealtimeSpreadData_signal(QList<double>, QMap<QString, double>);

  void sendRealtimeIndexData_signal(QList<double>);
  void sendRealtimeFutureData_signal(QList<double>);

  void sendRealTimeData(ChartData data);
  void sendHistFutureData_signal(QList<double>);

  void sendRealTimeIndexData_signal(QStringList, QStringList);
  void sendRealTimeIndexDataError_signal(QString);

public slots:
  void getPreCloseData_slot();
  void getRealtimeData_slot();

  void getHistFutureData_slot();

  void readRealtimeData_timer_slot();
  void processRealtimeDataComplete_slot();

private:
    bool                            m_isTest;

    HedgedParam                     m_hedgedParam;
    DatabaseParam                   m_dbParam;

    bool                            m_isFuture;
    bool                            m_isInitTimer;

    Database *                      m_database;
    QString                         m_futureName;

    QMap<QString, QStringList>      m_realTimeData;
    int                             m_monitorTime;
    QTimer *                        m_pTimer;
    
    QMap<QString, double>           m_indexPriceMap;

    QMap<QString, QList<double>>    m_vot;
    QMap<QString, QList<QString>>   m_time;
    QList<MACD>                     m_macdData;

    QDateTime                       m_lastTime;
    QDateTime                       m_currTime;
    int                             m_updateMSecs;
};
#endif // MONITORREALTIMEDATA_H
