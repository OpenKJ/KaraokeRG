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
    ziphandler.cpp \
    miniz.c \
    settings.cpp \
    dlgsettings.cpp

HEADERS  += mainwindow.h \
    processingthread.h \
    ziphandler.h \
    miniz.h \
    settings.h \
    dlgsettings.h

FORMS    += mainwindow.ui \
    dlgsettings.ui

unix:!macx {
    isEmpty(PREFIX) {
      PREFIX=/usr
    }
    iconfiles.files += resources/karaokerg.png
    iconfiles.path = $$PREFIX/share/pixmaps
    desktopfiles.files += resources/karaokerg.desktop
    desktopfiles.path = $$PREFIX/share/applications
    binaryfiles.files += KaraokeRG
    binaryfiles.path = $$PREFIX/bin
    INSTALLS += binaryfiles iconfiles desktopfiles
}
