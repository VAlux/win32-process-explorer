#-------------------------------------------------
#
# Project created by QtCreator 2013-04-26T12:04:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProcessManagerEX
TEMPLATE = app

LIBS += "H:\\[ Univercity ]\\[ Projects ]\\[ Course Progects ]\\build-ProcessManagerEX-Desktop_Qt_5_0_2_MinGW_32bit-Release\\release\\Win32ProcessExplorer.dll"

SOURCES += main.cpp\
        mainwindow.cpp \
    processmodel.cpp \
    processentry.cpp \
    executer.cpp

HEADERS  += mainwindow.h \
    processmodel.h \
    processentry.h \
    win32processexplorer_global.h \
    win32processexplorer.h \
    executer.h

FORMS    += mainwindow.ui \
    executer.ui

RESOURCES += \
    Res.qrc

RC_FILE = appicon.rc

QMAKE_CXXFLAGS += -std=c++0x
