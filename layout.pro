#-------------------------------------------------
#
# Project created by QtCreator 2017-10-16T15:24:20
#
#-------------------------------------------------

QT       += core gui sql widgets
QT += axcontainer
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = layout
TEMPLATE = app

INCLUDEPATH += ../public-files/public-function
INCLUDEPATH += ../public-files/public-class

SOURCES  += main.cpp\
            widget.cpp \
            toolfunc.cpp \
            test.cpp \
            chartform.cpp \
            qmychartview.cpp \
            dataread.cpp \
            dataprocess.cpp \
            dialog.cpp \
            monitorrealtimedata.cpp \
            chartdata.cpp \
            historydata.cpp \
            announcementform.cpp \
            generateportfolioform.cpp \
            futurechart.cpp \
            extractdatawindow.cpp \
            indexchart.cpp \
    ../public-files/public-class/basechart.cpp \
    ../public-files/public-class/database.cpp \
    ../public-files/public-class/macd.cpp \
    ../public-files/public-class/realtimedatabase.cpp \
    ../public-files/public-function/compute_func.cpp \
    ../public-files/public-function/excel_func.cpp \
    ../public-files/public-function/id_func.cpp \
    ../public-files/public-function/io_func.cpp \
    ../public-files/public-function/process_data_func.cpp \
    ../public-files/public-function/secode_func.cpp \
    ../public-files/public-function/terminal_func.cpp \
    ../public-files/public-function/time_func.cpp \
    ../public-files/public-function/widget_func.cpp \
    csschartformone.cpp

HEADERS  += widget.h \
            toolfunc.h \
            test.h \
            chartform.h \
            qmychartview.h \
            dataread.h \
            dataprocess.h \
            dialog.h \
            monitorrealtimedata.h \
            chartdata.h \
            historydata.h \
            announcementform.h \
            generateportfolioform.h \
            futurechart.h \
            extractdatawindow.h \
            indexchart.h \
    ../public-files/public-class/basechart.h \
    ../public-files/public-class/database.h \
    ../public-files/public-class/macd.h \
    ../public-files/public-class/realtimedatabase.h \
    ../public-files/public-function/compute_func.h \
    ../public-files/public-function/excel_func.h \
    ../public-files/public-function/id_func.h \
    ../public-files/public-function/io_func.h \
    ../public-files/public-function/process_data_func.h \
    ../public-files/public-function/secode_func.h \
    ../public-files/public-function/terminal_func.h \
    ../public-files/public-function/time_func.h \
    ../public-files/public-function/widget_func.h \
    csschartformone.h

FORMS    += widget.ui \
    chartform.ui \
    dialog.ui \
    announcementform.ui \
#    webviewform.ui
    generateportfolioform.ui \
    futurechart.ui \
    extractdatawindow.ui \
    indexchart.ui \
    csschartformone.ui

include(qtxlsx/src/xlsx/qtxlsx.pri)

RC_ICONS = dev.ico

#RC_ICONS = stable.ico

#RC_ICONS = Wondo.ico

