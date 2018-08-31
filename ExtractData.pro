#-------------------------------------------------
#
# Project created by QtCreator 2018-08-31T13:44:40
#
#-------------------------------------------------

QT += core gui sql widgets
QT += axcontainer
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExtractData
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../public-files/public-function
INCLUDEPATH += ../public-files/public-class

SOURCES += main.cpp\
    readdatabasedata.cpp \
    treeitem.cpp \
    treemodel.cpp \
    workprogressdialog.cpp \
    choosesecodewindow.cpp \
    extractdataform.cpp \
    extractindustrydata.cpp \
    extractmarketdata.cpp \
    extractweightdata.cpp \
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

HEADERS  += \
    readdatabasedata.h \
    treeitem.h \
    treemodel.h \
    workprogressdialog.h \
    choosesecodewindow.h \
    extractdataform.h \
    extractindustrydata.h \
    extractmarketdata.h \
    extractweightdata.h \
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

include(qtxlsx/src/xlsx/qtxlsx.pri)

RC_ICONS = jewel.ico     # 提取数据;

FORMS    += \
    workprogressdialog.ui \
    choosesecodewindow.ui \
    extractdataform.ui
