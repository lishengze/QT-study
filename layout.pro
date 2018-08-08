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
INCLUDEPATH +=../public-function
INCLUDEPATH +=../public-class

SOURCES += main.cpp\
    updaterealtimedataform.cpp \
    toolfunc.cpp \
    subwriteclass.cpp \
    realtimedataread.cpp \
    realtimedatawrite.cpp \
    test.cpp \
    extractmarketdata.cpp \
    readdatabasedata.cpp \
    extractdataform.cpp \
    workprogressdialog.cpp \
    extractweightdata.cpp \
    extractindustrydata.cpp \
    choosesecodewindow.cpp \
    treeitem.cpp \
    treemodel.cpp\
    wind_func.cpp \
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
    ../public-files/public-function/widget_func.cpp
#    ../public-function/excel_func.cpp \
#    ../public-function/time_func.cpp \
#    ../public-function/widget_func.cpp \
#    ../public-class/database.cpp \
#    ../public-class/realtimedatabase.cpp \
#    ../public-function/secode_func.cpp \
#    ../public-function/compute_func.cpp \
#    ../public-function/io_func.cpp \
#    ../public-function/process_data_func.cpp \
#    ../public-function/terminal_func.cpp \
#    ../public-class/macd.cpp \
#    ../public-function/id_func.cpp \


HEADERS  += updaterealtimedataform.h \
    realtimedataread.h \
    toolfunc.h \
    WAPIWrapperCpp.h \
    realtimedatawrite.h \
    subwriteclass.h \
    test.h \
    extractmarketdata.h \
    readdatabasedata.h \
    extractdataform.h \
    workprogressdialog.h \
    extractweightdata.h \
    extractindustrydata.h \
    choosesecodewindow.h \
    wind_func.h \
    treeitem.h \
    treemodel.h \
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
    ../public-files/public-function/widget_func.h
#    ../public-function/excel_func.h \
#    ../public-function/time_func.h \
#    ../public-function/widget_func.h \
#    ../public-class/database.h \
#    ../public-class/realtimedatabase.h \
#    ../public-function/secode_func.h \
#    ../public-function/compute_func.h \
#    ../public-function/io_func.h \
#    ../public-function/process_data_func.h \
#    ../public-function/terminal_func.h \
#    ../public-class/macd.h \
#    ../public-function/id_func.h \




FORMS    += updaterealtimedataform.ui \
    extractdataform.ui \
    workprogressdialog.ui \
    choosesecodewindow.ui

include(qtxlsx/src/xlsx/qtxlsx.pri)

#RC_ICONS = stable.ico
#RC_ICONS = lion.ico     # 更新实时数据;
RC_ICONS = jewel.ico     # 提取数据;

#LIBS += -LD:/github/workprogram/client-work -lWAPIWrapperCpp
LIBS += -LD:/client/upData-dev -lWAPIWrapperCpp
