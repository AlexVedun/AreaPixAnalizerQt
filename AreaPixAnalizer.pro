#-------------------------------------------------
#
# Project created by QtCreator 2012-01-14T13:28:09
#
#-------------------------------------------------

QT       += core gui widgets svg

TARGET = AreaPixAnalizer
TEMPLATE = app

CONFIG += c++17 qwt
# Qwt was compiled as static, but it will be linked via include/lib paths
INCLUDEPATH += C:/Qwt-6.3.0-qt6/include
LIBS += -LC:/Qwt-6.3.0-qt6/lib -lqwt


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








