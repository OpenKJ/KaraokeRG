#ifndef ZIPHANDLER_H
#define ZIPHANDLER_H

#include <QObject>
#include <QDir>

class ZipHandler : public QObject
{
    Q_OBJECT
private:
    QString m_zipFile;

public:
    explicit ZipHandler(QString zipFile, QObject *parent = 0);
    explicit ZipHandler(QObject *parent = 0);
    //~KhZip2();
    bool extractMp3(QDir destDir);
    bool extractCdg(QDir destDir);
    QString zipFile() const;
    void setZipFile(const QString &zipFile);
    int getSongDuration();
    bool createZip(QString zipFile, QString cdgFile, QString mp3File);
    bool containsRGMarkerFile();

signals:

public slots:

};

#endif // ZIPHANDLER_H
