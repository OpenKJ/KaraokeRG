#-------------------------------------------------
#
# Project created by QtCreator 2014-07-02T12:09:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KaraokeRG
TEMPLATE = app

# Populate version with version from git describe
VERSION = $$system(git -C $$_PRO_FILE_PWD_ describe --always --tags|tr "-" "." | cut -d"." -f1-3)
message($$VERSION)
DEFINES += GIT_VERSION=\\"\"$$VERSION\\"\"
QMAKE_TARGET_COMPANY = OpenKJ.org
QMAKE_TARGET_PRODUCT = KaraokeRG
QMAKE_TARGET_DESCRIPTION = KaraokeRG mp3+g zip ReplayGain processor

unix: BLDDATE = $$system(date -R)
win32: BLDDATE = $$system(date /t)
DEFINES += BUILD_DATE=__DATE__



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
