#-------------------------------------------------
#
# Project created by QtCreator 2018-11-21T13:29:23
#
#-------------------------------------------------

QT       += core gui
QT       += core gui serialport
QT       += widgets printsupport
QT       += network
QT +=  axcontainer
QT       += sql
RC_FILE += icon.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ARM_Controller
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        qcustomplot.cpp
HEADERS += \
        connect.h \
        mainwindow.h \
        hidapi.h \
        qcustomplot.h

FORMS += \
        mainwindow.ui

LIBS += -L$$_PRO_FILE_PWD_/  -lhidapi

DISTFILES += \
    icon.ico \
    icon.rc \
    main.qss

RESOURCES += \
    myicon.qrc
