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

#ifndef ASCIIREADERSETTINGS_H
#define ASCIIREADERSETTINGS_H

#include <QWidget>
#include <QSettings>
#include <QChar>

namespace Ui {
class AsciiReaderSettings;
}

class AsciiReaderSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AsciiReaderSettings(QWidget *parent = 0);
    ~AsciiReaderSettings();

    unsigned numOfChannels() const;
    QChar delimiter() const;
    /// Stores settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads settings from a `QSettings`.
    void loadSettings(QSettings* settings);

signals:
    void numOfChannelsChanged(unsigned);
    /// Signaled only with a valid delimiter
    void delimiterChanged(QChar);
    void filterChanged(QString);

private:
    Ui::AsciiReaderSettings *ui;

private slots:
    void delimiterToggled(bool checked);
    void customDelimiterChanged(const QString text);
    void filterTextChanged(const QString filter);
};

#endif // ASCIIREADERSETTINGS_H
