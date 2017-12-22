#include "ziphandler.h"
#include <QDebug>
#include <QTemporaryDir>
#include <fstream>
#include <QFile>
#include <QFileInfo>

#if defined(__GNUC__)
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE 1
#endif
#endif

#include "miniz.h"


ZipHandler::ZipHandler(QString zipFile, QObject *parent) :
    QObject(parent)
{
    m_zipFile = zipFile;
}

ZipHandler::ZipHandler(QObject *parent) :
    QObject(parent)
{
    m_zipFile = "";
}

bool ZipHandler::extractMp3(QDir destDir)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, m_zipFile.toStdString().c_str(), 0))
    {
        qWarning() << "mz_zip_reader_init_file() failed!";
        return false;
    }

    for (int i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
    {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
        {
            qWarning() << "mz_zip_reader_file_stat() failed!";
            mz_zip_reader_end(&zip_archive);
            return false;
        }
        QString filename = file_stat.m_filename;
        if (filename.endsWith(".mp3", Qt::CaseInsensitive))
        {
            QString outFile(destDir.path() + QDir::separator() + "tmp.mp3");
            if (mz_zip_reader_extract_file_to_file(&zip_archive, file_stat.m_filename,outFile.toLocal8Bit().data(),0))
            {
                mz_zip_reader_end(&zip_archive);
                return true;
            }
            else
            {
                qWarning() << "Error extracting file";
            }
        }

    }
    mz_zip_reader_end(&zip_archive);
    return false;
}

bool ZipHandler::extractCdg(QDir destDir)
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, m_zipFile.toLocal8Bit().data(), 0))
    {
        qWarning() << "mz_zip_reader_init_file() failed!";
        //return false;
    }

    for (int i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
    {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
        {
            qWarning() << "mz_zip_reader_file_stat() failed!";
            mz_zip_reader_end(&zip_archive);
            return false;
        }
        QString filename = file_stat.m_filename;
        if (filename.endsWith(".cdg", Qt::CaseInsensitive))
        {
            QString outFile(destDir.path() + QDir::separator() + "tmp.cdg");
            if (mz_zip_reader_extract_file_to_file(&zip_archive, file_stat.m_filename,outFile.toLocal8Bit().data(),0))
            {
                mz_zip_reader_end(&zip_archive);
                return true;
            }
            else
            {
                qWarning() << "Error extracting file";
            }
        }

    }
    mz_zip_reader_end(&zip_archive);
    return false;
}

QString ZipHandler::zipFile() const
{
    return m_zipFile;
}

void ZipHandler::setZipFile(const QString &zipFile)
{
    m_zipFile = zipFile;
}

int ZipHandler::getSongDuration()
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, m_zipFile.toLocal8Bit().data(), 0))
    {
        qWarning() << "mz_zip_reader_init_file() failed!";
        return 0;
    }

    for (int i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
    {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
        {
            qWarning() << "mz_zip_reader_file_stat() failed!";
            mz_zip_reader_end(&zip_archive);
            return 0;
        }
        QString filename = file_stat.m_filename;
        if (filename.endsWith(".cdg", Qt::CaseInsensitive))
        {
            mz_zip_reader_end(&zip_archive);
            return ((file_stat.m_uncomp_size / 96) / 75) * 1000;
        }

    }
    mz_zip_reader_end(&zip_archive);
    return 0;
}

bool ZipHandler::createZip(QString zipFile, QString cdgFile, QString mp3File)
{
    QString mp3FileName = QFileInfo(mp3File).fileName();
    QString cdgFileName = QFileInfo(cdgFile).fileName();
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_writer_init_file(&zip_archive, zipFile.toLocal8Bit().data(),0))
    {
        qWarning() << "Error initializing zip file";
    }
    const char *s_pComment = "File has been processed through ReplayGain with OpenKJ KaraokeRG";
    qWarning() << "ZipHandler::createZip(" << zipFile << ", " << cdgFile << ", " << mp3File << ")";
    qWarning() << "Adding mp3 file";
    if (!mz_zip_writer_add_file(&zip_archive, mp3FileName.toLocal8Bit().data(), mp3File.toLocal8Bit().data(), NULL, 0, MZ_BEST_COMPRESSION))
    {
        qWarning() << "Error adding mp3 file to archive";
        mz_zip_writer_finalize_archive(&zip_archive);
        mz_zip_writer_end(&zip_archive);
        return false;
    }
    qWarning() << "Adding cdg file";
    if (!mz_zip_writer_add_file(&zip_archive, cdgFileName.toLocal8Bit().data(), cdgFile.toLocal8Bit().data(), NULL, 0, MZ_BEST_COMPRESSION))
    {
        qWarning() << "Error adding cdg file to archive";
        mz_zip_writer_finalize_archive(&zip_archive);
        mz_zip_writer_end(&zip_archive);
        return false;
    }
    qWarning() << "Adding marker to file";
    if (!mz_zip_writer_add_mem(&zip_archive,"ReplayGainProcessed", s_pComment, sizeof(s_pComment), MZ_BEST_COMPRESSION))
    {
        qWarning() << "Error adding marker file to archive";
        mz_zip_writer_finalize_archive(&zip_archive);
        mz_zip_writer_end(&zip_archive);
        return false;
    }
    qWarning() << "Files added to archive";
    mz_zip_writer_finalize_archive(&zip_archive);
    mz_zip_writer_end(&zip_archive);
    return true;
}

bool ZipHandler::containsRGMarkerFile()
{
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    if (!mz_zip_reader_init_file(&zip_archive, m_zipFile.toLocal8Bit().data(), 0))
    {
        qWarning() << "mz_zip_reader_init_file() failed!";
        //return false;
    }

    for (int i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
    {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
        {
            qWarning() << "mz_zip_reader_file_stat() failed!";
            mz_zip_reader_end(&zip_archive);
            return false;
        }
        QString filename = file_stat.m_filename;
        if (filename == "ReplayGainProcessed")
        {
            mz_zip_reader_end(&zip_archive);
            return true;
        }
    }
    mz_zip_reader_end(&zip_archive);
    return false;
}
