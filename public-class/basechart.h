#ifndef BASECHART_H
#define BASECHART_H

#include <QWidget>
#include <QMainWindow>
#include <QCategoryAxis>
#include "qmychartview.h"

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class BaseChart : public QWidget
{
    Q_OBJECT
public:
    explicit BaseChart(int windowID, QWidget *parent = 0);

    virtual void initTableContextMenu();

    virtual void initLayout();
    virtual void initTheme();
    virtual void initChartView();

    virtual void initExtractKeyValueList() ;
    virtual QList<QMyChartView*> getChartViewList() ;
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir);
    virtual QList<QStringList> getExcelData(QStringList keyValueList);

    virtual void updateAxis() ;
    virtual void updateSeries() ;
    virtual void updateMousePos() ;

    virtual void setPropertyValue(int index);
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event);
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event) ;
    virtual void KeyReleaseFunc(QEvent *event);
    virtual void moveMouse(int step);
    virtual double getPointXDistance();

    QCategoryAxis* getTimeAxisX (QList<QString> timeList, int tickCount);

signals:
    void windowClose_signal(int windowID);

public slots:
    void showContentMenu_slot(QPoint);
    void showExtractDialog_slot();
    virtual void getChoosenInfo_slot(QStringList, QString, bool);

protected:
    virtual bool eventFilter (QObject *watched, QEvent *event);
    virtual void closeEvent(QCloseEvent *event);

protected:
    QStringList  m_extractKeyValueList;

private:
    int         m_windowID;
};

#endif // BASECHART_H
