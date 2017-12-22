#include "processingthread.h"
#include "ziphandler.h"
#include <QTemporaryDir>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QMutex>

ProcessingThread::ProcessingThread(QObject *parent) :
    QThread(parent)
{
    m_terminate = false;
    settings = new Settings(this);
    m_force = settings->forceReprocessing();
    mp3gainPath = settings->mp3GainPath();
}

void ProcessingThread::setFiles(QStringList *files)
{
    m_files = files;
}

void ProcessingThread::processFile(QString fileName)
{
    qWarning() << "Processing - File: " << fileName;
    QFileInfo info(fileName);
    QString baseName = info.completeBaseName();
    emit stateChanged("Extracting file");
    QTemporaryDir tmpDir;
    ZipHandler zipper;
    zipper.setZipFile(fileName);
    if ((zipper.containsRGMarkerFile()) && (!m_force))
    {
        qWarning() << "File contains marker, already processed.  Skipping";
        emit stateChanged("Skipping file, already processed");
        return;
    }
    qWarning() << "Processing - Extracting mp3 file";
    if (!zipper.extractMp3(QDir(tmpDir.path())))
    {
        qWarning() << "Bad file contents - error extracting mp3";
        emit stateChanged("Error extracting file");
        return;
    }
    QFile::copy(tmpDir.path() + QDir::separator() + "tmp.mp3", "/storage/KaraokeRGTest/PreRG.mp3");
    qWarning() << "Processing - Extracting cdg file";
    if (!zipper.extractCdg(QDir(tmpDir.path())))
    {
        qWarning() << "Processing - Bad file contents - error extracting cdg";
        emit stateChanged("Error extracting file");
        return;
    }
    emit stateChanged("Processing - Doing ReplayGain analasis and adjustment");
    QString program = mp3gainPath;
    QStringList arguments;
    arguments << "-c";
    arguments << "-r";
    arguments << "-T";
    arguments << "-s" << "r";
    arguments << tmpDir.path() + QDir::separator() + "tmp.mp3";
    QProcess process;
    process.start(program, arguments);
    process.waitForFinished();
    QFile::copy(tmpDir.path() + QDir::separator() + "tmp.mp3", "/storage/KaraokeRGTest/PostRG.mp3");
    qWarning() << process.readAllStandardOutput();
    //qWarning() << process.readAllStandardError();
    qWarning() << "Processing - Creating zip file";
    emit stateChanged("Creating zip file");
    QFile::rename(tmpDir.path() + QDir::separator() + "tmp.mp3", tmpDir.path() + QDir::separator() + baseName + ".mp3");
    QFile::rename(tmpDir.path() + QDir::separator() + "tmp.cdg", tmpDir.path() + QDir::separator() + baseName + ".cdg");
    if (zipper.createZip(tmpDir.path() + QDir::separator() + info.fileName(), tmpDir.path() + QDir::separator() + baseName + ".cdg", tmpDir.path() + QDir::separator() + baseName + ".mp3"))
    {
        qWarning () << "Processing - Replacing original file";
        emit stateChanged("Replacing original file");
        if (QFile(tmpDir.path() + QDir::separator() + info.fileName()).exists())
        {
            qWarning() << "Doing QFile::rename(" << info.absoluteFilePath() << ", " << info.absoluteFilePath() + ".tmp" << ");";
            if (QFile::rename(info.absoluteFilePath(), info.absoluteFilePath() + ".tmp"))
            {
                if (QFile::copy(tmpDir.path() + QDir::separator() + info.fileName(), info.absoluteFilePath()))
                {
                    qWarning() << "New file copied into place, deleting old one";
                    if (!QFile::remove(info.absoluteFilePath() + ".tmp"))
                    {
                        qWarning() << "Error deleting old file";
                    }
                }

            }
            else
            {
                qWarning() << "Unable to move existing file to tmp file";
                return;
            }
        }
        else
        {
            qWarning() << "Unexpected error, new processed zip file missing!";
        }
        emit stateChanged("Idle");
        qWarning() << "Processing - Complete for file: " << fileName;
    }
    else
    {
        qWarning() << "Failed to create new archive, leaving original file in place";
    }
}
QMutex mutex;
void ProcessingThread::run()
{
    while (!m_terminate)
    {
        mutex.lock();
        if (m_files->size() > 0)
        {
            m_currentFile = m_files->at(0);
            m_files->removeFirst();
            mutex.unlock();
        }
        else
        {
            mutex.unlock();
            emit processingComplete();
            emit processingFile("N/A");
            return;
        }
        emit processingFile(m_currentFile);
        processFile(m_currentFile);
        emit fileProcessed();

    }
    emit processingFile("N/A");
    emit processingAborted();
}

void ProcessingThread::stopProcessing()
{
    m_terminate = true;
}
