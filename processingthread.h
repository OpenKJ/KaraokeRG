#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H

#include <QThread>
#include <QStringList>
#include "settings.h"

class ProcessingThread : public QThread
{
    Q_OBJECT
private:
    QStringList *m_files;
    QString m_currentFile;
    bool m_terminate;
    Settings *settings;
    bool m_force;
    QString mp3gainPath;
    QString zipPath;

public:
    explicit ProcessingThread(QObject *parent = 0);
    void setFiles(QStringList *files);
    void processFile(QString fileName);
    void run();

signals:
    void stateChanged(QString);
    void processingFile(QString);
    void progressChanged(int);
    void processingComplete();
    void processingAborted();
    void fileProcessed();

public slots:
    void stopProcessing();

};

#endif // PROCESSINGTHREAD_H
