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

#ifndef UPDATECHECKDIALOG_H
#define UPDATECHECKDIALOG_H

#include <QDialog>
#include <QDate>
#include <QSettings>
#include "updatechecker.h"

namespace Ui {
class UpdateCheckDialog;
}

class UpdateCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateCheckDialog(QWidget *parent = 0);
    ~UpdateCheckDialog();

    /// Stores update settings into a `QSettings`.
    void saveSettings(QSettings* settings);
    /// Loads update settings from a `QSettings`.
    void loadSettings(QSettings* settings);

private:
    Ui::UpdateCheckDialog *ui;
    UpdateChecker updateChecker;
    QDate lastCheck;

    void showEvent(QShowEvent *event);
    void closeEvent(QShowEvent *event);
};

#endif // UPDATECHECKDIALOG_H
