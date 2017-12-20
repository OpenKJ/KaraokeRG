#include "dlgsettings.h"
#include "ui_dlgsettings.h"
#include <QFileDialog>

DlgSettings::DlgSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSettings)
{
    ui->setupUi(this);
    settings = new Settings(this);
    ui->lblMp3GainPath->setText(settings->mp3GainPath());
}

DlgSettings::~DlgSettings()
{
    delete ui;
}

void DlgSettings::on_btnBrowse_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("mp3gain executable"), "", tr("All Files (*)"));
    if (fileName != "")
    {
        settings->setMp3GainPath(fileName);
        ui->lblMp3GainPath->setText(fileName);
    }
}
