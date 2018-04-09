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
    updaterealtimedataform.cpp \
    realtimedatabase.cpp \
    database.cpp \
#    realtimedataread.cpp \
#    excel.cpp \
#    strategy.cpp \
#    strategymodel.cpp \
    toolfunc.cpp \
#    realtimedatawrite.cpp \
    subwriteclass.cpp \
    realtimedataread.cpp \
    realtimedatawrite.cpp \
    test.cpp

#    setdata.cpp \
#        widget.cpp \

#    callout.cpp \
#    test.cpp \
#    chartform.cpp \

#    tabledata.cpp \
#    macd.cpp \
#    qmychartview.cpp \
#    processfunc.cpp \
#    dataread.cpp \
#    dataprocess.cpp \
#    dialog.cpp \
#    realtimedataprocess.cpp \
#    monitorrealtimedata.cpp \
#    chartdata.cpp \
#    preclosedatabase.cpp \


HEADERS  += updaterealtimedataform.h \
    realtimedatabase.h \
    realtimedataread.h \
    database.h \
#    excel.h \
#    strategy.h \
#    strategymodel.h \
    toolfunc.h \
    WAPIWrapperCpp.h \
    realtimedatawrite.h \
    subwriteclass.h \
    test.h

#    setdata.h \
#    widget.h \
#    callout.h \
#    test.h \
#    chartform.h \
#    tabledata.h \
#    macd.h \
#    qmychartview.h \
#    processfunc.h \
#    dataread.h \
#    dataprocess.h \
#    dialog.h \
#    realtimedataprocess.h \
#    monitorrealtimedata.h \
#    chartdata.h \
#    preclosedatabase.h \


FORMS    += updaterealtimedataform.ui \
#    widget.ui \
#    chartform.ui \
#    dialog.ui \

#RC_ICONS = stable.ico
#RC_ICONS = lion.ico
RC_ICONS = dev.ico

#LIBS += -LD:/github/workprogram/client-work -lWAPIWrapperCpp
LIBS += -LE:/github/work-program/client/client-updatedata-dev -lWAPIWrapperCpp
