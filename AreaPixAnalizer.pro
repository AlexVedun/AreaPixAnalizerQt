#-------------------------------------------------
#
# Project created by QtCreator 2012-01-14T13:28:09
#
#-------------------------------------------------

QT       += core gui

TARGET = AreaPixAnalizer
TEMPLATE = app

CONFIG += static qwt
INCLUDEPATH += C:/Qt/Qwt-static-6.0.1/include
LIBS += -LC:/Qt/Qwt-static-6.0.1/lib -lqwt


SOURCES += main.cpp\
        mainwindow.cpp \
    qmygraphicsscene.cpp \
    setscaledialog.cpp \
    setforcedialog.cpp \
    setelementdialog.cpp

HEADERS  += mainwindow.h \
    qmygraphicsscene.h \
    setscaledialog.h \
    setforcedialog.h \
    setelementdialog.h

FORMS    += mainwindow.ui \
    setscaledialog.ui \
    setforcedialog.ui \
    setelementdialog.ui

RESOURCES += \
    AreaPixAnalizer.qrc








