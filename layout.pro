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
    windapi.cpp

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
    windapi.h \
    WAPIWrapperCpp.h

FORMS    += widget.ui \
    chartform.ui \
    dialog.ui

LIBS += -LE:/github/work-program/client/client-work -lWAPIWrapperCpp
