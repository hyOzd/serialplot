/*
  Copyright © 2018 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QIcon>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegularExpression>
#include <QtDebug>

#include "recordpanel.h"
#include "ui_recordpanel.h"
#include "setting_defines.h"

RecordPanel::RecordPanel(Stream* stream, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordPanel),
    recordToolBar(tr("Record Toolbar")),
    recordAction(QIcon::fromTheme("media-record"), tr("Record"), this),
    recorder(this)
{
    overwriteSelected = false;
    _stream = stream;

    ui->setupUi(this);

    recordToolBar.setObjectName("tbRecord");

    recordAction.setCheckable(true);
    recordToolBar.addAction(&recordAction);
    ui->pbRecord->setDefaultAction(&recordAction);

    connect(ui->pbBrowse, &QPushButton::clicked,
            this, &RecordPanel::selectFile);
    connect(&recordAction, &QAction::triggered,
            this, &RecordPanel::onRecord);

    connect(ui->cbRecordPaused, SIGNAL(toggled(bool)),
            this, SIGNAL(recordPausedChanged(bool)));

    connect(ui->cbDisableBuffering, &QCheckBox::toggled,
            [this](bool enabled)
            {
                recorder.disableBuffering = enabled;
            });

    connect(ui->cbWindowsLE, &QCheckBox::toggled,
            [this](bool enabled)
            {
                recorder.windowsLE = enabled;
            });

    connect(&recordAction, &QAction::toggled, ui->cbWindowsLE, &QWidget::setDisabled);
}

RecordPanel::~RecordPanel()
{
    delete ui;
}

QToolBar* RecordPanel::toolbar()
{
    return &recordToolBar;
}

bool RecordPanel::recordPaused()
{
    return ui->cbRecordPaused->isChecked();
}

bool RecordPanel::selectFile()
{
    QString fileName = QFileDialog::getSaveFileName(
        parentWidget(), tr("Select recording file"));

    if (fileName.isEmpty())
    {
        return false;
    }
    else
    {
        selectedFile = fileName;
        ui->lbFileName->setText(selectedFile);
        overwriteSelected = QFile::exists(fileName);
        return true;
    }
}

void RecordPanel::onRecord(bool start)
{
    if (!start)
    {
        stopRecording();
        return;
    }

    bool canceled = false;
    if (ui->leSeparator->text().isEmpty())
    {
        QMessageBox::critical(this, "Error",
                              "Column separator cannot be empty! Please select a separator.");
        ui->leSeparator->setFocus(Qt::OtherFocusReason);
        canceled = true;
    }

    // check file name
    if (!canceled && selectedFile.isEmpty() && !selectFile())
    {
        canceled = true;
    }

    if (!canceled && !overwriteSelected && QFile::exists(selectedFile))
    {
        if (ui->cbAutoIncrement->isChecked())
        {
            // TODO: should we increment even if user selected to replace?
            canceled = !incrementFileName();
        }
        else
        {
            canceled = !confirmOverwrite(selectedFile);
        }
    }

    if (canceled)
    {
        recordAction.setChecked(false);
    }
    else
    {
        overwriteSelected = false;
        startRecording();
    }
}

bool RecordPanel::incrementFileName(void)
{
    QFileInfo fileInfo(selectedFile);

    QString base = fileInfo.completeBaseName();
    QRegularExpression regex("(.*?)(\\d+)(?!.*\\d)(.*)");
    auto match = regex.match(base);

    if (match.hasMatch())
    {
        bool ok;
        int fileNum = match.captured(2).toInt(&ok);
        base = match.captured(1) + QString::number(fileNum + 1) + match.captured(3);
    }
    else
    {
        base += "_1";
    }

    QString suffix = fileInfo.suffix();;
    if (!suffix.isEmpty())
    {
        suffix = "." + suffix;
    }

    QString autoFileName = fileInfo.path() + "/" + base + suffix;

    // check if auto generated file name exists, ask user another name
    if (QFile::exists(autoFileName))
    {
        if (!confirmOverwrite(autoFileName))
        {
            return false;
        }
    }
    else
    {
        selectedFile = autoFileName;
    }

    ui->lbFileName->setText(selectedFile);
    return true;
}

bool RecordPanel::confirmOverwrite(QString fileName)
{
    // prepare message box
    QMessageBox mb(parentWidget());
    mb.setWindowTitle(tr("File Already Exists"));
    mb.setIcon(QMessageBox::Warning);
    mb.setText(tr("File (%1) already exists. How to continue?").arg(fileName));

    auto bCancel    = mb.addButton(QMessageBox::Cancel);
    auto bOverwrite = mb.addButton(tr("Overwrite"), QMessageBox::DestructiveRole);
    mb.addButton(tr("Select Another File"), QMessageBox::YesRole);

    mb.setEscapeButton(bCancel);

    // show message box
    mb.exec();

    if (mb.clickedButton() == bCancel)
    {
        return false;
    }
    else if (mb.clickedButton() == bOverwrite)
    {
        selectedFile = fileName;
        return true;
    }
    else                    // select button
    {
        return selectFile();
    }
}

void RecordPanel::startRecording(void)
{
    QStringList channelNames;
    if (ui->cbHeader->isChecked())
    {
        channelNames = _stream->infoModel()->channelNames();
    }
    if (recorder.startRecording(selectedFile, getSeparator(), channelNames))
    {
        stream->connectFollower(&recorder);
    }
}

void RecordPanel::stopRecording(void)
{
    recorder.stopRecording();
}

void RecordPanel::onPortClose()
{
    if (recordAction.isChecked() && ui->cbStopOnClose->isChecked())
    {
        stopRecording();
        recordAction.setChecked(false);
    }
}

QString RecordPanel::getSeparator() const
{
    QString sep = ui->leSeparator->text();
    sep.replace("\\t", "\t");
    return sep;
}

void RecordPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Record);
    settings->setValue(SG_Record_AutoIncrement, ui->cbAutoIncrement->isChecked());
    settings->setValue(SG_Record_RecordPaused, ui->cbRecordPaused->isChecked());
    settings->setValue(SG_Record_StopOnClose, ui->cbStopOnClose->isChecked());
    settings->setValue(SG_Record_Header, ui->cbHeader->isChecked());
    settings->setValue(SG_Record_DisableBuffering, ui->cbDisableBuffering->isChecked());
    settings->setValue(SG_Record_Separator, ui->leSeparator->text());
    settings->endGroup();
}

void RecordPanel::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Record);
    ui->cbAutoIncrement->setChecked(
        settings->value(SG_Record_AutoIncrement, ui->cbAutoIncrement->isChecked()).toBool());
    ui->cbRecordPaused->setChecked(
        settings->value(SG_Record_RecordPaused, ui->cbRecordPaused->isChecked()).toBool());
    ui->cbStopOnClose->setChecked(
        settings->value(SG_Record_StopOnClose, ui->cbStopOnClose->isChecked()).toBool());
    ui->cbHeader->setChecked(
        settings->value(SG_Record_Header, ui->cbHeader->isChecked()).toBool());
    ui->cbDisableBuffering->setChecked(
        settings->value(SG_Record_DisableBuffering, ui->cbDisableBuffering->isChecked()).toBool());
    ui->leSeparator->setText(settings->value(SG_Record_Separator, ui->leSeparator->text()).toString());
    settings->endGroup();
}
