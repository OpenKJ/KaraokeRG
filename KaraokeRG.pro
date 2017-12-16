#-------------------------------------------------
#
# Project created by QtCreator 2014-07-02T12:09:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KaraokeRG
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    processingthread.cpp \
    ziphandler.cpp

HEADERS  += mainwindow.h \
    processingthread.h \
    ziphandler.h

FORMS    += mainwindow.ui
