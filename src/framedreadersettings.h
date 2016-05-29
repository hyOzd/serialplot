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

#ifndef FRAMEDREADERSETTINGS_H
#define FRAMEDREADERSETTINGS_H

#include <QWidget>
#include <QByteArray>

#include "numberformatbox.h"
#include "endiannessbox.h"

namespace Ui {
class FramedReaderSettings;
}

class FramedReaderSettings : public QWidget
{
    Q_OBJECT

public:
    explicit FramedReaderSettings(QWidget *parent = 0);
    ~FramedReaderSettings();

    void showMessage(QString message, bool error = false);

    unsigned numOfChannels();
    NumberFormat numberFormat();
    Endianness endianness();
    QByteArray syncWord();
    unsigned frameSize(); /// If frame bye is enabled `0` is returned
    bool checksumEnabled();

signals:
    /// If sync word is invalid (empty or 1 nibble missing at the end)
    /// signaled with an empty array
    void syncWordChanged(QByteArray);
    /// `0` indicates frame size byte is enabled
    void frameSizeChanged(unsigned);
    void checksumChanged(bool);
    void numOfChannelsChanged(unsigned);
    void numberFormatChanged(NumberFormat);

private:
    Ui::FramedReaderSettings *ui;

private slots:
    void onSyncWordEdited();
};

#endif // FRAMEDREADERSETTINGS_H
