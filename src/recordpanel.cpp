/*
  Copyright © 2017 Hasan Yavuz Özderya

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

#include "recordpanel.h"
#include "ui_recordpanel.h"

#include <QIcon>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegularExpression>

#include <QtDebug>

RecordPanel::RecordPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordPanel),
    recordToolBar(tr("Record Toolbar")),
    recordAction(QIcon::fromTheme("media-record"), tr("Record"), this)
{
    overwriteSelected = false;

    ui->setupUi(this);

    recordToolBar.setObjectName("tbRecord");

    recordAction.setCheckable(true);
    recordToolBar.addAction(&recordAction);
    ui->pbRecord->setDefaultAction(&recordAction);

    connect(ui->pbBrowse, &QPushButton::clicked,
            this, &RecordPanel::selectFile);
    connect(&recordAction, &QAction::triggered,
            this, &RecordPanel::record);
}

RecordPanel::~RecordPanel()
{
    delete ui;
}

QToolBar* RecordPanel::toolbar()
{
    return &recordToolBar;
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

void RecordPanel::record(bool start)
{
    if (selectedFile.isEmpty() && !selectFile())
    {
        return;
    }

    if (!overwriteSelected && QFile::exists(selectedFile))
    {
        if (ui->cbAutoIncrement->isChecked())
        {
            // TODO: should we increment even if user selected to replace?
            incrementFileName();
        }
        else
        {
            selectFile();
        }
    }

    overwriteSelected = false;

    // TODO: implement recording
}

bool RecordPanel::incrementFileName(void) {
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

    // TODO: check if new name exists as well!
    QString suffix = fileInfo.suffix();;
    if (!suffix.isEmpty())
    {
        suffix = "." + suffix;
    }

    QString autoFileName = fileInfo.path() + "/" + base + suffix;

    // check if auto generated file name exists, ask user another name
    if (QFile::exists(autoFileName))
    {
        QMessageBox mb(parentWidget());
        mb.setWindowTitle(tr("File Already Exists"));
        mb.setIcon(QMessageBox::Warning);
        mb.setText(tr("Auto generated file name (%1) already exists. How to continue?").arg(autoFileName));

        auto bCancel = mb.addButton(QMessageBox::Cancel);
        auto bOverwrite = mb.addButton(tr("Overwrite"), QMessageBox::DestructiveRole);
        auto bSelect = mb.addButton(tr("Select Another File"), QMessageBox::YesRole);

        mb.setEscapeButton(bCancel);

        mb.exec();

        if (mb.clickedButton() == bCancel)
        {
            return false;
        }
        else if (mb.clickedButton() == bOverwrite)
        {
            selectedFile = autoFileName;
        }
        else                    // bSelect
        {
            selectFile();       // TODO: return false if user cancels
        }
    }
    else
    {
        selectedFile = autoFileName;
    }

    ui->lbFileName->setText(selectedFile);
    return true;
}
