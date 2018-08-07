#ifndef BASECHART_H
#define BASECHART_H

#include <QWidget>
#include <QMainWindow>
#include "qmychartview.h"

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class BaseChart : public QWidget
{
    Q_OBJECT
public:
    explicit BaseChart(QWidget *parent = 0);

    void initTableContextMenu();

    virtual void initExtractKeyValueList() = 0;
    virtual QList<QMyChartView*> getChartViewList() = 0;
    virtual QString getExcelFileName(QStringList keyValueList, QString fileDir) = 0;
    virtual QList<QStringList> getExcelData(QStringList keyValueList) = 0;

    virtual void initLayout() = 0;
    virtual void initTheme() = 0;
    virtual void initChartView() = 0;

    virtual void updateAxis() = 0;
    virtual void updateSeries() = 0;
    virtual void updateMousePos() = 0;

    virtual void setPropertyValue(int index) = 0;
    virtual void mouseMoveEvenFunc(QObject *watched, QEvent *event) = 0;
    virtual void mouseButtonReleaseFunc(QObject *watched, QEvent *event) = 0;
    virtual void KeyReleaseFunc(QEvent *event) = 0;
    virtual void moveMouse(int step) = 0;
    virtual double getPointXDistance() = 0;

signals:

public slots:
    void showContentMenu_slot(QPoint);
    void showExtractDialog_slot();
    void getChoosenInfo_slot(QStringList, QString);

protected:
    virtual bool eventFilter (QObject *watched, QEvent *event);
    virtual void closeEvent(QCloseEvent *event) = 0;

protected:
    QStringList  m_extractKeyValueList;

private:

};

#endif // BASECHART_H
