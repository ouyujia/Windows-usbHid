#-------------------------------------------------
#
# Project created by QtCreator 2023-03-13T15:06:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USBDemo
TEMPLATE = app

LIBS += -lSetupAPI
LIBS += -lhid


DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += CLOUD_CLIENT
DEFINES += _X86_

win32 { QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01
DEFINES += _ATL_XP_TARGETING
}

QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CFLAGS_RELEASE = $$QMAKE_CLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS += -source-charset:utf-8
QMAKE_CXXFLAGS_WARN_ON += -wd4819

SOURCES += main.cpp \
        widget.cpp \
        winHid/myusbrw.cpp \
        winHid/hidapi.c \
        rwusbport.cpp

HEADERS  += widget.h \
        myusbrw.h \
        winHid/hidapi_cfgmgr32.h \
        winHid/hidapi_hidclass.h \
        winHid/hidapi_hidpi.h \
        winHid/hidapi_hidsdi.h \
        winHid/hidapi_winapi.h \
        winHid/myusbrw.h \
        winHid/hidsdi.h \
        winHid/hidpi.h \
        winHid/hidusage.h \
        winHid/hidapi.h \
        rwusbport.h


DISTFILES += \
    winHid/winHid.pri

FORMS    += widget.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/release/ -lsetupapi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/debug/ -lsetupapi

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/release/ -lhid
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/debug/ -lhid

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/lib
