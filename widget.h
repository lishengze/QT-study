#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QtCharts/QChart"
#include <QChartView>
#include "callout.h"
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

#include "announcementform.h"
#include "database.h"
#include "generateportfolioform.h"

//#include "excel.h"
//#include "strategymodel.h"

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    void initCommonData();
    void initFileDir();

    void initWidegt();    

    void initHedgedWidget();
    void initEnergyWidget();
    void initFutureWidget();
    void initAnnounceWidget();
    void initDatasourceWidget();
    void initProgramWorkInfoWidget();
    void initTableContextMenuWidget();
    void setPortfolioTableWidget();
    void setBuySalePortfolioTableWidget();

    void initCSSParamComBox();

    bool checkCodeInDatabase(QString codeName, QString dbhost, QStringList timeTypeList);

    QStringList getEnergyDataFreq();
    QList<int> getCSSParams();
    void getAveParams(QList<int>& aveNumbList, QList<bool>& isEMAList);

    ~Widget();

protected:
    void closeEvent(QCloseEvent *event);

signals:

public slots:
    void windowClose_slot(int windowID);

private slots:
    void on_chooseStartDate_editingFinished();

    void on_Annoucnement_Button_clicked();

    void on_historyHedgeIndexChart_clicked();

    void on_historyBuySaleChart_clicked();

    void on_realTimeHedgeIndexChart_clicked();

    void on_realTimeBuySaleChart_clicked();

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

    void on_showAVEEnergy_pushButton_clicked();

    void on_historyEnergyChart_clicked();

private:
    Ui::Widget *             ui;

    QList<QWidget*>          m_chartViews;
    QList<QString>           m_dbhostList;
    QString                  m_currDBHost;
    QString                  m_nativeFileName;
    QString                  m_strategyFileDir;
    QString                  m_portfolioFileName;

    QString                  m_buySalePortfolioFileDir;
    QString                  m_currBuySalePortfolioName;
    QString                  m_futureIndexCode;
    QList<QString>           m_futureList;
    bool                     m_isBuySalePortfolio;

    QMap<QString, int>       m_portfolioMap;
    QList<QFileInfo>         m_strategyFileInfoList;

    QMap<QString, int>       m_buyStrategyMap;
    QMap<QString, int>       m_saleStrategyMap;
    QList<QFileInfo>         m_buySalePortfolioFileInfoList;
    QMap<QString, QString>   m_portfolioAmount;

    QMap<QString, int>       m_seocdebuyCountMap;
    QStringList              m_secodeNameList;
    QString                  m_hedgeIndexCode;
    int                      m_hedgeIndexCount;

    QMap<QString, QString>   m_indexMap;

    AnnouncementForm*        m_announcementView;
    GeneratePortfolioForm*   m_genePortfolioWindow;

    QListWidget*             m_aveParamListWidget;
    QListWidget*             m_cssParamListWidget;
    QListWidget*             m_energyDataFreqListWidget;
};

//    void initCalendar();
//    void initHedgeComboBox();
//    void initDataFrequency();
//    void initEnergyDataFreq();
//    void initEnergyIndexCodeComboBox();
//    void initMacdTime();
//    void setFutureContractCombox();
//    void setFutureContractList();
//    void initSpreadSpinbox();

#endif // WIDGET_H
