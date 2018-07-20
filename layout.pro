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

#INCLUDEPATH +=D:\client\public-function
#INCLUDEPATH +=D:\client\public-class


INCLUDEPATH +=../public-function
INCLUDEPATH +=../public-class

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
    ../public-function/excel_func.cpp \
    ../public-function/time_func.cpp \
    ../public-function/widget_func.cpp \
    ../public-function/secode_func.cpp \
    ../public-class/database.cpp \
    ../public-class/realtimedatabase.cpp \
    ../public-function/compute_func.cpp \
    indexchart.cpp \
    ../public-function/process_data_func.cpp \
    ../public-function/io_func.cpp \
    ../public-function/terminal_func.cpp \
    ../public-class/macd.cpp

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
    ../public-function/excel_func.h \
    ../public-function/time_func.h \
    ../public-function/widget_func.h \
    ../public-function/secode_func.h \
    ../public-class/database.h \
    ../public-class/realtimedatabase.h \
    ../public-function/compute_func.h \
    indexchart.h \
    ../public-function/process_data_func.h \
    ../public-function/io_func.h \
    ../public-function/terminal_func.h \
    ../public-class/macd.h

FORMS    += widget.ui \
    chartform.ui \
    dialog.ui \
    announcementform.ui \
#    webviewform.ui
    generateportfolioform.ui \
    futurechart.ui \
    indexchart.ui

include(qtxlsx/src/xlsx/qtxlsx.pri)

RC_ICONS = dev.ico

#RC_ICONS = stable.ico

#RC_ICONS = Wondo.ico

