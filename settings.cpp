#include "settings.h"

Settings::Settings(QObject *parent) : QObject(parent)
{
    settings = new QSettings(this);
}

QString Settings::mp3GainPath()
{
    return settings->value("mp3gainPath", "/usr/bin/mp3gain").toString();
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

