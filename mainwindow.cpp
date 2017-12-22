#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QDirIterator>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_numProcessed = 0;
    m_threadsDone = 0;
    m_threadsAborted = 0;
    ui->setupUi(this);
    QApplication::setApplicationName("KaraokeRG");
    QApplication::setOrganizationName("OpenKJ");
    QApplication::setOrganizationDomain("OpenKJ.org");
    settings = new Settings(this);
    dlgSettings = new DlgSettings(this);
    m_threads = QThread::idealThreadCount();
    if (m_threads == 1)
    {
        ui->groupBox2->hide();
        ui->groupBox3->hide();
        ui->groupBox4->hide();
    }
    else if (m_threads == 2)
    {
        ui->groupBox3->hide();
        ui->groupBox4->hide();
    }
    else if (m_threads == 3)
    {
        ui->groupBox4->hide();
    }
    m_files = new QStringList;
    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(on_actionSettings_activated()));
    ui->checkBoxForce->setChecked(settings->forceReprocessing());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_files;
}

void MainWindow::on_btnBrowse_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this);
    if (fileName != "")
    {
        m_path = fileName;
        ui->labelPath->setText(fileName);
        ui->btnStart->setEnabled(true);
        QApplication::processEvents();
    }
}

void MainWindow::getFiles()
{
    ui->labelStatus1->setText("Scanning for files");
    m_files->clear();
    QDir dir(m_path);
    QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.filePath();
            if (filename.endsWith(".zip",Qt::CaseInsensitive))
                m_files->append(filename);
        }
        QApplication::processEvents();
    }
    qWarning() << "Files found: " << m_files->size();
    ui->progressBar->setMaximum(m_files->size());
    ui->progressBar->setValue(0);
    ui->labelStatus1->setText("Idle");
}

void MainWindow::on_btnStart_clicked()
{
    if (!QFile::exists(settings->mp3GainPath()))
    {
        QMessageBox msgBox;
        msgBox.setText("mp3gain not found!  Please set the mp3gain path in settings.");
        msgBox.exec();
        return;
    }
    getFiles();
    m_numProcessed = 0;
    if (m_files->size() == 0)
        return;
    m_thread1 = new ProcessingThread(this);
    m_thread2 = new ProcessingThread(this);
    m_thread3 = new ProcessingThread(this);
    m_thread4 = new ProcessingThread(this);

    connect(m_thread1, SIGNAL(processingFile(QString)), ui->labelCurrentFile1, SLOT(setText(QString)));
    connect(m_thread2, SIGNAL(processingFile(QString)), ui->labelCurrentFile2, SLOT(setText(QString)));
    connect(m_thread3, SIGNAL(processingFile(QString)), ui->labelCurrentFile3, SLOT(setText(QString)));
    connect(m_thread4, SIGNAL(processingFile(QString)), ui->labelCurrentFile4, SLOT(setText(QString)));

    connect(m_thread1, SIGNAL(stateChanged(QString)), ui->labelStatus1, SLOT(setText(QString)));
    connect(m_thread2, SIGNAL(stateChanged(QString)), ui->labelStatus2, SLOT(setText(QString)));
    connect(m_thread3, SIGNAL(stateChanged(QString)), ui->labelStatus3, SLOT(setText(QString)));
    connect(m_thread4, SIGNAL(stateChanged(QString)), ui->labelStatus4, SLOT(setText(QString)));

    connect(m_thread1, SIGNAL(processingComplete()), this, SLOT(threadDone()));
    connect(m_thread2, SIGNAL(processingComplete()), this, SLOT(threadDone()));
    connect(m_thread3, SIGNAL(processingComplete()), this, SLOT(threadDone()));
    connect(m_thread4, SIGNAL(processingComplete()), this, SLOT(threadDone()));

    connect(m_thread1, SIGNAL(processingAborted()), this, SLOT(threadAborted()));
    connect(m_thread2, SIGNAL(processingAborted()), this, SLOT(threadAborted()));
    connect(m_thread3, SIGNAL(processingAborted()), this, SLOT(threadAborted()));
    connect(m_thread4, SIGNAL(processingAborted()), this, SLOT(threadAborted()));

    connect(m_thread1, SIGNAL(fileProcessed()), this, SLOT(fileProcessed()));
    connect(m_thread2, SIGNAL(fileProcessed()), this, SLOT(fileProcessed()));
    connect(m_thread3, SIGNAL(fileProcessed()), this, SLOT(fileProcessed()));
    connect(m_thread4, SIGNAL(fileProcessed()), this, SLOT(fileProcessed()));

    connect(ui->btnStop, SIGNAL(clicked()), m_thread1, SLOT(stopProcessing()));
    connect(ui->btnStop, SIGNAL(clicked()), m_thread2, SLOT(stopProcessing()));
    connect(ui->btnStop, SIGNAL(clicked()), m_thread3, SLOT(stopProcessing()));
    connect(ui->btnStop, SIGNAL(clicked()), m_thread4, SLOT(stopProcessing()));

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->btnBrowse->setEnabled(false);

    m_thread1->setFiles(m_files);
    m_thread1->start();

    if (m_threads >= 2)
    {
        m_thread2->setFiles(m_files);
        m_thread2->start();
    }
    if (m_threads >= 3)
    {
        m_thread3->setFiles(m_files);
        m_thread3->start();
    }
    if (m_threads >=4)
    {
        m_thread4->setFiles(m_files);
        m_thread4->start();
    }

}

void MainWindow::threadDone()
{
    m_threadsDone++;
    int total = m_threadsDone + m_threadsAborted;
    if ((total >= m_threads) || (total >= m_files->size()))
    {
        ui->btnStop->setEnabled(false);
        ui->btnBrowse->setEnabled(true);
        ui->btnStart->setEnabled(true);
        ui->labelCurrentFile1->setText("Idle");
        ui->labelCurrentFile2->setText("Idle");
        ui->labelCurrentFile3->setText("Idle");
        ui->labelCurrentFile4->setText("Idle");
        ui->labelStatus1->setText("N/A");
        ui->labelStatus2->setText("N/A");
        ui->labelStatus3->setText("N/A");
        ui->labelStatus4->setText("N/A");
    }
}

void MainWindow::threadAborted()
{
    m_threadsAborted++;
    int total = m_threadsDone + m_threadsAborted;
    if ((total >= m_threads) || (total >= m_files->size()))
    {
        ui->btnStop->setEnabled(false);
        ui->btnBrowse->setEnabled(true);
        ui->btnStart->setEnabled(true);
        ui->labelCurrentFile1->setText("Aborted");
        ui->labelCurrentFile2->setText("Aborted");
        ui->labelCurrentFile3->setText("Aborted");
        ui->labelCurrentFile4->setText("Aborted");
        ui->labelStatus1->setText("N/A");
        ui->labelStatus2->setText("N/A");
        ui->labelStatus3->setText("N/A");
        ui->labelStatus4->setText("N/A");
    }
}

void MainWindow::fileProcessed()
{
    qWarning() << "MainWindow received signal fileProcessed()";
    m_numProcessed++;
    ui->progressBar->setValue(m_numProcessed);
}

void MainWindow::on_actionSettings_activated()
{
    dlgSettings->show();
}

void MainWindow::on_checkBoxForce_toggled(bool checked)
{
    settings->setForceReprocessing(checked);
}
