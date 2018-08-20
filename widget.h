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
    void initCalendar();
    void initHedgeComboBox();
    void initDataFrequency();
    void initSelectIndexComboBox();
    void initHedgedIndexComboBox();
    void initIndexAveNumb();
    void initEnergyIndexCodeComboBox();

    void initMacdTime();
    void setFutureContractCombox();
    void initDatasourceCombox();
    void initSpreadSpinbox();

    void initTableContextMenu();
    void initProgramWorkInfoTableView();

    void setFutureContractList();
    void setStrategyTable();
    void setBuySalePortfolioTable();

    ~Widget();

protected:
    void closeEvent(QCloseEvent *event);

signals:

public slots:
    void receiveChartCLoseSignal(int chartViewID);

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

    void on_showIndexHedgedPic_pushButton_clicked();

    void on_showAVEEnergy_pushButton_clicked();

private:
    Ui::Widget *             ui;

    QList<QWidget*>          m_chartViews;
    QList<QString>           m_dbhostList;
    QString                  m_currDBHost;
    QString                  m_nativeFileName;
    QString                  m_strategyFileDir;
    QString                  m_currStrategyName;

    QString                  m_buySalePortfolioFileDir;
    QString                  m_currBuySalePortfolioName;
    QString                  m_futureIndexCode;
    QList<QString>           m_futureList;
    bool                     m_isBuySalePortfolio;

    QMap<QString, int>       m_strategyMap;
    QList<QFileInfo>         m_strategyFileInfoList;

    QMap<QString, int>       m_buyStrategyMap;
    QMap<QString, int>       m_saleStrategyMap;
    QList<QFileInfo>         m_buySalePortfolioFileInfoList;
    QMap<QString, QString>   m_portfolioAmount;

    QMap<QString, int>       m_seocdebuyCountMap;
    QStringList              m_secodeNameList;
    QString                  m_hedgeIndexCode;
    int                      m_hedgeIndexCount;

    AnnouncementForm*        m_announcementView;
    GeneratePortfolioForm*   m_genePortfolioWindow;


};

#endif // WIDGET_H
