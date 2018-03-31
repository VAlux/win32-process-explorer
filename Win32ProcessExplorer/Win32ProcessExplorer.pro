#-------------------------------------------------
#
# Project created by QtCreator 2013-05-12T13:41:38
#
#-------------------------------------------------

QT       -= gui

TARGET = Win32ProcessExplorer
TEMPLATE = lib

DEFINES += WIN32PROCESSEXPLORER_LIBRARY

SOURCES += win32processexplorer.cpp

HEADERS += win32processexplorer.h\
        win32processexplorer_global.h

LIBS += -lpsapi

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
