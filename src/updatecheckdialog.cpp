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

#include "setting_defines.h"
#include "updatecheckdialog.h"
#include "ui_updatecheckdialog.h"

UpdateCheckDialog::UpdateCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateCheckDialog)
{
    ui->setupUi(this);

    // by default start from yesterday, so that we check at first run
    lastCheck = QDate::currentDate().addDays(-1);

    connect(&updateChecker, &UpdateChecker::checkFailed,
            [this](QString errorMessage)
            {
                lastCheck = QDate::currentDate();
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
                    show();
#ifdef UPDATE_TYPE_PKGMAN
                    text = QString("There is a new version: %1. "
                                   "Use your package manager to update"
                                   " or click to <a href=\"%2\">download</a>.")\
                        .arg(newVersion).arg(downloadUrl);
#else
                    text = QString("Found update to version %1. Click to <a href=\"%2\">download</a>.")\
                        .arg(newVersion).arg(downloadUrl);
#endif
                }

                lastCheck = QDate::currentDate();
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

void UpdateCheckDialog::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_UpdateCheck);
    settings->setValue(SG_UpdateCheck_Periodic, ui->cbPeriodic->isChecked());
    settings->setValue(SG_UpdateCheck_LastCheck, lastCheck.toString(Qt::ISODate));
    settings->endGroup();
}

void UpdateCheckDialog::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_UpdateCheck);
    ui->cbPeriodic->setChecked(settings->value(SG_UpdateCheck_Periodic,
                                               ui->cbPeriodic->isChecked()).toBool());
    auto lastCheckS = settings->value(SG_UpdateCheck_LastCheck, lastCheck.toString(Qt::ISODate)).toString();
    lastCheck = QDate::fromString(lastCheckS, Qt::ISODate);
    settings->endGroup();

    // start the periodic update if required
    if (ui->cbPeriodic->isChecked() && lastCheck < QDate::currentDate())
    {
        updateChecker.checkUpdate();
    }
}
