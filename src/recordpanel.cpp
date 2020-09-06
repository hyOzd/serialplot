/*
  Copyright © 2020 Hasan Yavuz Özderya

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
#include <QCompleter>
#include <QFileSystemModel>
#include <QDirModel>
#include <QtDebug>
#include <ctime>

#include "recordpanel.h"
#include "ui_recordpanel.h"
#include "setting_defines.h"
#include "utils.h"

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

    connect(ui->spDecimals, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int decimals)
            {
                recorder.setDecimals(decimals);
            });


    connect(&recordAction, &QAction::toggled, ui->cbWindowsLE, &QWidget::setDisabled);
    connect(&recordAction, &QAction::toggled, ui->cbTimestamp, &QWidget::setDisabled);
    connect(&recordAction, &QAction::toggled, ui->leSeparator, &QWidget::setDisabled);
    connect(&recordAction, &QAction::toggled, ui->pbBrowse, &QWidget::setDisabled);

    QCompleter *completer = new QCompleter(this);
    // TODO: QDirModel is deprecated, use QFileSystemModel (but it doesn't work)
    completer->setModel(new QDirModel(completer));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->leFileName->setCompleter(completer);
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
        setSelectedFile(fileName);
        overwriteSelected = QFile::exists(fileName);
        return true;
    }
}

QString RecordPanel::selectedFile() const
{
    return ui->leFileName->text();
}

void RecordPanel::setSelectedFile(QString f)
{
    ui->leFileName->setText(f);
}

QString RecordPanel::getSelectedFile()
{
    if (selectedFile().isEmpty())
    {
        if (!selectFile()) return QString();
    }

    // assume that file name contains a time format specifier
    if (selectedFile().contains("%"))
    {
        auto ts = formatTimeStamp(selectedFile());
        if (!QFile::exists(ts) || // file doesn't exists
            confirmOverwrite(ts)) // exists but user accepted overwrite
        {
            return ts;
        }
        return QString();
    }

    // if no timestamp and file exists try autoincrement option
    if (!overwriteSelected && QFile::exists(selectedFile()))
    {
        if (ui->cbAutoIncrement->isChecked())
        {
            if (!incrementFileName()) return QString();
        }
        else
        {
            if (!confirmOverwrite(selectedFile()))
                return QString();
        }
    }

    return selectedFile();
}

QString RecordPanel::formatTimeStamp(QString t) const
{
    auto maxSize = t.size() + 1024;
    auto r = new char[maxSize];

    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(r, maxSize, t.toLatin1().data(), timeinfo);

    auto rs = QString(r);
    delete r;
    return rs;
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
    QString fn;
    if (!canceled)
    {
        fn = getSelectedFile();
        canceled = fn.isEmpty();
    }

    if (canceled)
    {
        recordAction.setChecked(false);
    }
    else
    {
        overwriteSelected = false;
        // TODO: show more visible error message when recording fails
        if (!startRecording(fn))
            recordAction.setChecked(false);
    }
}

bool RecordPanel::incrementFileName(void)
{
    QFileInfo fileInfo(selectedFile());

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
        setSelectedFile(autoFileName);
    }

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
        setSelectedFile(fileName);
        return true;
    }
    else                    // select button
    {
        return selectFile();
    }
}

bool RecordPanel::startRecording(QString fileName)
{
    QStringList channelNames;
    if (ui->cbHeader->isChecked())
    {
        channelNames = _stream->infoModel()->channelNames();
    }
    if (recorder.startRecording(fileName, getSeparator(),
                                channelNames, ui->cbTimestamp->isChecked()))
    {
        _stream->connectFollower(&recorder);
        return true;
    }
    else
    {
        return false;
    }
}

void RecordPanel::stopRecording(void)
{
    recorder.stopRecording();
    _stream->disconnectFollower(&recorder);
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
    settings->setValue(SG_Record_Timestamp, ui->cbTimestamp->isChecked());
    settings->setValue(SG_Record_Separator, ui->leSeparator->text());
    settings->setValue(SG_Record_Decimals, ui->spDecimals->text());
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
    ui->cbTimestamp->setChecked(
        settings->value(SG_Record_Timestamp, ui->cbTimestamp->isChecked()).toBool());
    ui->leSeparator->setText(settings->value(SG_Record_Separator, ui->leSeparator->text()).toString());
    ui->spDecimals->setValue(settings->value(SG_Record_Decimals, ui->spDecimals->value()).toInt());
    settings->endGroup();
}
