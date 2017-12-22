#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
private:
    QSettings *settings;
public:
    explicit Settings(QObject *parent = 0);
    QString mp3GainPath();
    void setMp3GainPath(QString path);
    bool forceReprocessing();
    void setForceReprocessing(bool enabled);

signals:

public slots:
};

#endif // SETTINGS_H
