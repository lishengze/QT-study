#include "basechart.h"
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include "extractdatawindow.h"
#include "excel_func.h"
#include "process_data_func.h"
#include <QDebug>
#pragma execution_character_set("utf-8")

BaseChart::BaseChart(QWidget *parent) : QWidget(parent)
{

}

void BaseChart::initTableContextMenu() {
    QList<QMyChartView*> chartViewList = getChartViewList();
    for (int i = 0;i < chartViewList.size(); ++i) {
        chartViewList[i]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(chartViewList[i], SIGNAL(customContextMenuRequested(QPoint)),
                this, SLOT(showContentMenu_slot(QPoint)));
    }
}

bool BaseChart::eventFilter (QObject *watched, QEvent *event) {
    if (event->type () == QEvent::MouseMove) {
        mouseMoveEvenFunc(watched, event);
    }
    if (event->type() == QEvent::KeyRelease) {
        KeyReleaseFunc(event);
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        mouseButtonReleaseFunc(watched, event);
//        QChartView::mouseReleaseEvent((QMouseEvent *)event);
    }
    return QWidget::eventFilter (watched, event);
}

void BaseChart::showContentMenu_slot(QPoint pos) {
    QMenu *menu = new QMenu();
    QAction *extractData = new QAction(QString("提取数据"));

    connect (extractData,SIGNAL(triggered()),this,SLOT(showExtractDialog_slot()));

    menu->addAction(extractData);
    menu->move (cursor().pos ());
    menu->show ();
    pos;
}

void BaseChart::showExtractDialog_slot() {
    qDebug() << "showExtractDialog_slot";
    ExtractDataWindow* currExtractWindow = new ExtractDataWindow(m_extractKeyValueList,this);

    connect(currExtractWindow, SIGNAL(getChoosenInfo_signal(QStringList, QString)),
            this, SLOT(getChoosenInfo_slot(QStringList, QString)));

    currExtractWindow->show();
}

void BaseChart::getChoosenInfo_slot(QStringList choosenKeyValueList, QString fileDir) {
    QList<QStringList> excelData = getExcelData(choosenKeyValueList);
    QString fileName = getExcelFileName(choosenKeyValueList, fileDir);
    qDebug() << "fileName: " << fileName;
    int result = writeMatrixData(fileName, excelData, "Sheet1", true);
    qDebug() << "writeExcelData: " << result;
    if (result == 1) {
        QMessageBox::information(NULL, "信息", QString("文件 %1 写入成功").arg(fileName));
    } else {
        QMessageBox::critical(NULL, "错误", QString("文件 %1 写入失败").arg(fileName));
    }
}

QCategoryAxis* BaseChart::getTimeAxisX (QList<QString> timeList, int tickCount) {
    QCategoryAxis* axisX = new QCategoryAxis;
    axisX->setStartValue(0);
    QList<int> axisXPosList = getNumbList(timeList.size(), tickCount);
    for (int i = 0; i < axisXPosList.size(); ++i) {
        int xpos = axisXPosList[i];
        axisX->append (timeList[xpos], xpos);
    }
    axisX->setMax(timeList.size()-1);
    return axisX;
}
