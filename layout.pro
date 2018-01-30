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


SOURCES += main.cpp\
        widget.cpp \
    setdata.cpp \
    callout.cpp \
    toolfunc.cpp \
    test.cpp \
    strategy.cpp \
    strategymodel.cpp \
    database.cpp \
    chartform.cpp \
    excel.cpp \
    tabledata.cpp \
    macd.cpp \
    qmychartview.cpp \
    processfunc.cpp \
    dataread.cpp \
    dataprocess.cpp \
    dialog.cpp \
    realtimedataprocess.cpp \
    realtimedataread.cpp \
    monitorrealtimedata.cpp \
    chartdata.cpp \
    realtimedatabase.cpp \
    preclosedatabase.cpp \
#    updatedataform.cpp
#    testdiaform.cpp \
#    updatedataform.cpp
    updaterealtimedataform.cpp

HEADERS  += widget.h \
    setdata.h \
    callout.h \
    toolfunc.h \
    test.h \
    strategy.h \
    strategymodel.h \
    database.h \
    chartform.h \
    excel.h \
    tabledata.h \
    macd.h \
    qmychartview.h \
    processfunc.h \
    dataread.h \
    dataprocess.h \
    dialog.h \
    realtimedataprocess.h \
    WAPIWrapperCpp.h \
    realtimedataread.h \
    monitorrealtimedata.h \
    chartdata.h \
    realtimedatabase.h \
    preclosedatabase.h \
#    updatedataform.h
#    testdiaform.h \
#    updatedataform.h
    updaterealtimedataform.h

FORMS    += widget.ui \
    chartform.ui \
    dialog.ui \
#    updatedataform.ui
#    testdiaform.ui \
#    updatedataform.ui
    updaterealtimedataform.ui

#LIBS += -LD:/github/workprogram/client-work -lWAPIWrapperCpp
LIBS += -LE:/github/work-program/client/client-work-changeRefresh -lWAPIWrapperCpp
