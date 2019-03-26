#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QtCharts/QChart"
#include <QChartView>
#include <QGraphicsSimpleTextItem>
#include <QList>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPointF>
#include <QDialog>
#include <QTableView>
#include <QThread>
#include <QTimer>
#include <QFileInfo>
#include <QListWidget>

#include "WorkProgressDialog.h"
#include "announcementform.h"
#include "database.h"
#include "generateportfolioform.h"
#include "historydata.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class Widget;
}

class Widget : public QWidget
{
  Q_OBJECT

public:
  explicit Widget(QWidget *parent = 0);

  void initCommonData();
  void registSignalParamsType();
  void setDBTableNameMap();
  void initFileDir();

  void initWidegt();
  void initDateTimeWidget();
  void initHedgeTypeWidget();
  void initHedgedWidget();
  void initEnergyWidget();
  void initFutureWidget();
  void initAnnounceWidget();

  void initDatasourceWidget();
  void initProgramWorkInfoWidget();
  void initTableContextMenuWidget();
  void setPortfolioTableWidget();
  void setBuySalePortfolioTableWidget();
  void initWorkProgressDialog();

  void initCSSParamComBox();

  bool checkCodeInDatabase(QString codeName, QString dbhost, 
                           QStringList timeTypeList);

  void checkBuySalePortfolio();

  QStringList getEnergyDataFreq();
  QList<int> getCSSParams();
  QList<int> getMACDParams();
  void getAveParams(QList<int> &aveNumbList, QList<bool> &isEMAList);

  ~Widget();

protected:
  void closeEvent(QCloseEvent *event);

signals:
  void getTableList_signal();

public slots:

  void updateProgramInfo_slot(QString info, bool isWarning);
  
  void windowClose_slot(int windowID, QString windowName);

  void sendTableList_slot(QString timeType, QStringList tableList);

private slots:
  void on_Annoucnement_Button_clicked();

  void on_chooseStrategyDir_Button_clicked();

  void on_choosePortfolioDir_Button_clicked();

  void show_strategyTable_contextMenu(QPoint);

  void show_portfolioTable_contextMenu(QPoint);

  void refresh_strategy_table();

  void refresh_portfolio_table();

  void delete_strategy_file();

  void delete_portfolio_file();

  void on_strategy_table_clicked(const QModelIndex &index);

  void on_buySalePortfolio_table_clicked(const QModelIndex &index);

  void on_strategy_table_doubleClicked(const QModelIndex &index);

  void on_buySalePortfolio_table_doubleClicked(const QModelIndex &index);

  void on_showFutureSpread_Button_clicked();

  void on_dataSource_ComboBox_currentIndexChanged(const QString &arg1);

  void on_indexEnergyChart_clicked();

  void on_portfolioIndexHistSpreadChart_clicked();

  void on_portfolioIndexRealSpreadChart_clicked();

  void on_portfolioIndexEnergyChart_clicked();

  void on_portfolioHedgedEnergyChart_clicked();

  void on_portfolioHedgedHistSpreadChart_clicked();

  void on_portfolioHedgedRealSpreadChart_clicked();

  void on_SpreadEarning_spinBox_editingFinished();

  void on_HedgeEarning_comboBox_activated(const QString &arg1);

  void on_SingleEarning_comboBox_activated(const QString &arg1);

  void on_weightDate_editingFinished();

  void on_SpreadEarning_spinBox_valueChanged(const QString &arg1);

  void on_SingleEarning_radioButton_clicked(bool checked);

  void on_HedgeEarning_radioButton_clicked(bool checked);

  void on_SpreadEarning_radioButton_clicked(bool checked);

private:
  Ui::Widget *                      ui;

  bool                              m_isDev;

  QList<QWidget *>                  m_chartViews;
  QList<QString>                    m_dbhostList;
  QString                           m_currDBHost;

  QString                           m_infoFileName;
  QString                           m_oriPortfolioFileDir;
  QString                           m_hedgePortfolioFileDir;  

  QString                           m_oriPortfolioName;

  QString                           m_hedgedPortfolioName;
  int                               m_hedgedType;
  QString                           m_hedgedString;
  QString                           m_hedgedCode;
  QString                           m_weightDate;

  QString                           m_futureIndexCode;
  QList<QString>                    m_futureList;

  QMap<QString, double>             m_oriPortfolio;
  QMap<QString, double>             m_hedgedPortfolio;
  QString                           m_hedgeIndexCode;
  int                               m_hedgeIndexCount;  

  QList<QFileInfo>                  m_oriPortfolioFileInfoList;
  QList<QFileInfo>                  m_hedgePortfolioFileInfoList;

  AnnouncementForm *                m_announcementView;
  GeneratePortfolioForm *           m_genePortfolioWindow;

  QListWidget *                     m_aveParamListWidget;
  QListWidget *                     m_cssParamListWidget;
  QListWidget *                     m_energyDataFreqListWidget;
  QStringList                       m_timeTypeList;

  QMap<QString, QStringList>        m_databaseTableNameMap;
  WorkProgressDialog *              m_workProgressDialog;
  HistoryData *                     m_histDataWorker;
  QThread                           m_histDataThread;
};

#endif // WIDGET_H
