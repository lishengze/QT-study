#include <QApplication>
#include <QTextCodec>
#include <QProcess>

#include "widget.h"
#include "test.h"
#include "testform.h"
#include "processfunc.h"
#include "dialog.h"
#include "generateportfolioform.h"

#include "extractdatawindow.h"
#include "indexchart.h"
#pragma execution_character_set("utf-8")

void setLibPath() {
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath("./lib");
}


int main(int argc, char *argv[])
{
    setLibPath();
    QApplication a(argc, argv);

    Widget spreadWindow;
    spreadWindow.setWindowTitle("点差客户端");
    spreadWindow.show();

    spreadWindow.setDBTableNameMap();

//    GeneratePortfolioForm genePorfolioWindow;
//    genePorfolioWindow.setWindowTitle("生成组合客户端");
//    genePorfolioWindow.show();

//    testMain();

//    int dbConnId = 0;
//    QString dbhost = "192.168.211.162";
//    QString databaseName = "MarketData_15m";
//    QString hedgedSecode = "000300";
//    QString selectSecode = "000913";
//    QString startDate = "20180716";
//    QString endDate = "20180723";
//    int     aveNumb = 32;
//    double  css12Rate = 0.014;
//    double  cssRate1 = 1.0/3.0;
//    double  cssRate2 = 2.0/3.0;
//    double  maxCSS = 300;
//    double  minCSS = -300;
//    IndexChart indexChartWindow(dbConnId, dbhost, databaseName,
//                                selectSecode, hedgedSecode, startDate, endDate,
//                                aveNumb, css12Rate, cssRate1, cssRate2,
//                                maxCSS, minCSS);
//    indexChartWindow.setWindowTitle("测试指数对冲图");
//    indexChartWindow.show();

//    QStringList m_extractKeyValueList;
//    m_extractKeyValueList.append(QString("沪深300指数"));
//    m_extractKeyValueList.append(QString("对冲点差值"));
//    m_extractKeyValueList.append(QString("成交量额"));
//    m_extractKeyValueList.append(QString("DIFF"));
//    m_extractKeyValueList.append(QString("DEA"));
//    m_extractKeyValueList.append(QString("MACD"));

//    ExtractDataWindow extractWindow(m_extractKeyValueList);
//    extractWindow.setWindowTitle("提取数据");
//    extractWindow.show();

//    testMain();

    return a.exec();
}

