/*
  Copyright © 2016 Hasan Yavuz Özderya

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

#ifndef BINARYSTREAMREADERSETTINGS_H
#define BINARYSTREAMREADERSETTINGS_H

#include <QWidget>
#include <QSettings>

#include "numberformatbox.h"
#include "endiannessbox.h"

namespace Ui {
class BinaryStreamReaderSettings;
}

class BinaryStreamReaderSettings : public QWidget
{
    Q_OBJECT

public:
    explicit BinaryStreamReaderSettings(QWidget *parent = 0);
    ~BinaryStreamReaderSettings();

    unsigned numOfChannels();
    NumberFormat numberFormat();
    Endianness endianness();

    /// Stores settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads settings from a `QSettings`.
    void loadSettings(QSettings* settings);

signals:
    void numOfChannelsChanged(unsigned);
    void numberFormatChanged(NumberFormat);
    void skipByteRequested();
    void skipSampleRequested();

private:
    Ui::BinaryStreamReaderSettings *ui;
};

#endif // BINARYSTREAMREADERSETTINGS_H
