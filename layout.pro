#-------------------------------------------------
#
# Project created by QtCreator 2017-10-16T15:24:20
#
#-------------------------------------------------

QT       += core gui sql widgets
QT += axcontainer

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
    macd.cpp

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
    macd.h

FORMS    += widget.ui \
    chartform.ui \
    form.ui
QT += charts
