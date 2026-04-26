#-------------------------------------------------
#
# Project created by QtCreator 2012-01-14T13:28:09
#
#-------------------------------------------------

QT       += core gui widgets svg charts

TARGET = AreaPixAnalizer
TEMPLATE = app

CONFIG += c++17 static
DEFINES += QT_STATIC

TRANSLATIONS += lang/en.ts \
                lang/uk.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    qmygraphicsscene.cpp \
    setscaledialog.cpp \
    setforcedialog.cpp

HEADERS  += mainwindow.h \
    qmygraphicsscene.h \
    setscaledialog.h \
    setforcedialog.h

FORMS    += mainwindow.ui \
    setscaledialog.ui \
    setforcedialog.ui

RESOURCES += \
    AreaPixAnalizer.qrc
