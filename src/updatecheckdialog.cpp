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

#include "updatecheckdialog.h"
#include "ui_updatecheckdialog.h"

UpdateCheckDialog::UpdateCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateCheckDialog)
{
    ui->setupUi(this);

    connect(&updateChecker, &UpdateChecker::checkFailed,
            [this](QString errorMessage)
            {
                ui->label->setText(QString("Update check failed.\n") + errorMessage);
            });

    connect(&updateChecker, &UpdateChecker::checkFinished,
            [this](bool found, QString newVersion, QString downloadUrl)
            {
                QString text;
                if (!found)
                {
                    text = "There is no update yet.";
                }
                else
                {
                    text = QString("Found update to version %1. Click to <a href=\"%2\">download</a>.")\
                        .arg(newVersion).arg(downloadUrl);
                    qDebug() << text;
                }

                ui->label->setText(text);
            });
}

UpdateCheckDialog::~UpdateCheckDialog()
{
    delete ui;
}

void UpdateCheckDialog::showEvent(QShowEvent *event)
{
    updateChecker.checkUpdate();
    ui->label->setText("Checking update...");
}

void UpdateCheckDialog::closeEvent(QShowEvent *event)
{
    if (updateChecker.isChecking()) updateChecker.cancelCheck();
}
