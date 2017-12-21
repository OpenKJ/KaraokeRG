#include "settings.h"
#include <QCoreApplication>
#include <QDir>

Settings::Settings(QObject *parent) : QObject(parent)
{
    settings = new QSettings(this);
}

QString Settings::mp3GainPath()
{
#ifdef Q_OS_WINDOWS
    QString appDir = QCoreApplication::applicationDirPath();
    return settings->value("mp3gainPath", appDir + QDir::separator() + "mp3gain.exe").toString();
#else
    return settings->value("mp3gainPath", "/usr/bin/mp3gain").toString();
#endif
}

void Settings::setMp3GainPath(QString path)
{
    settings->setValue("mp3gainPath", path);
}

bool Settings::forceReprocessing()
{
    return settings->value("forceReprocessing", false).toBool();
}

void Settings::setForceReprocessing(bool enabled)
{
    settings->setValue("forceReprocessing", enabled);
}

