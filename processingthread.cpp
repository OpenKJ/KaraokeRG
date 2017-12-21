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
    zipPath = settings->zipPath();
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
    QFile marker(tmpDir.path() + QDir::separator() + "ReplayGainProcessed");
    marker.open(QIODevice::WriteOnly);
    QTextStream out(&marker);
    out << "File has been processed through ReplayGain with OpenKJ KaraokeRG";
    marker.close();
//    zipper.createZip(, tmpDir.path() + QDir::separator() + baseName + ".cdg", tmpDir.path() + QDir::separator() + baseName + ".mp3");
    program = zipPath;
    arguments.clear();
    arguments << "-j";
    arguments << "-9";
    arguments << tmpDir.path() + QDir::separator() + info.fileName();
    arguments << tmpDir.path() + QDir::separator() + baseName + ".cdg";
    arguments << tmpDir.path() + QDir::separator() + baseName + ".mp3";
    arguments << tmpDir.path() + QDir::separator() + "ReplayGainProcessed";
    process.start(program,arguments);
    process.waitForFinished();
    qWarning() << process.readAllStandardOutput();
    qWarning () << "Processing - Replacing original file";
    emit stateChanged("Replacing original file");
    QFile::remove(info.absoluteFilePath());
    QFile::copy(tmpDir.path() + QDir::separator() + info.fileName(), info.absoluteFilePath());
    emit stateChanged("Idle");
    qWarning() << "Processing - Complete for file: " << fileName;
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
