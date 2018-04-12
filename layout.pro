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


SOURCES  += main.cpp\
            widget.cpp \
            toolfunc.cpp \
            test.cpp \
            database.cpp \
            chartform.cpp \
            macd.cpp \
            qmychartview.cpp \
            dataread.cpp \
            dataprocess.cpp \
            dialog.cpp \
            monitorrealtimedata.cpp \
            chartdata.cpp \
            historydata.cpp \
            announcementform.cpp \
            realtimedatabase.cpp \
            generateportfolioform.cpp \
    futurechart.cpp

#    tabledata.cpp \
#    setdata.cpp \
#    callout.cpp \
#    strategy.cpp \
#    strategymodel.cpp \
#    excel.cpp \
#    processfunc.cpp \
#    realtimedataprocess.cpp \
#    realtimedataread.cpp \
#    webviewform.cpp


HEADERS  += widget.h \
            toolfunc.h \
            test.h \
            database.h \
            chartform.h \
            macd.h \
            qmychartview.h \
            dataread.h \
            dataprocess.h \
            dialog.h \
            monitorrealtimedata.h \
            chartdata.h \
            historydata.h \
            announcementform.h \
            realtimedatabase.h \
            generateportfolioform.h \
    futurechart.h

#    tabledata.h \
#    strategy.h \
#    strategymodel.h \
#    excel.h \
#    setdata.h \
#    callout.h \
#    processfunc.h \
#    realtimedataprocess.h \
#    WAPIWrapperCpp.h \
#    realtimedataread.h \
#    webviewform.h

FORMS    += widget.ui \
    chartform.ui \
    dialog.ui \
    announcementform.ui \
#    webviewform.ui
    generateportfolioform.ui \
    futurechart.ui

include(qtxlsx/src/xlsx/qtxlsx.pri)

RC_ICONS = dev.ico

#RC_ICONS = stable.ico

#RC_ICONS = Wondo.ico

