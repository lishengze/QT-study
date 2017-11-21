#-------------------------------------------------
#
# Project created by QtCreator 2017-10-16T15:24:20
#
#-------------------------------------------------

QT       += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = layout
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    setdata.cpp \
    callout.cpp \
    chartdialog.cpp \
    toolfunc.cpp

HEADERS  += widget.h \
    setdata.h \
    callout.h \
    chartdialog.h \
    toolfunc.h

FORMS    += widget.ui \
    chartdialog.ui
QT += charts
