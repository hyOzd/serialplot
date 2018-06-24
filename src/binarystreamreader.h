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

#ifndef BINARYSTREAMREADER_H
#define BINARYSTREAMREADER_H

#include <QSettings>

#include "abstractreader.h"
#include "binarystreamreadersettings.h"

/**
 * Reads a simple stream of samples in binary form from the
 * device. There is no means of synchronization other than a button
 * that should be manually triggered by user.
 */
class BinaryStreamReader : public AbstractReader
{
    Q_OBJECT
public:
    explicit BinaryStreamReader(QIODevice* device, QObject *parent = 0);
    QWidget* settingsWidget();
    unsigned numChannels() const;
    void enable(bool enabled = true);
    /// Stores settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads settings from a `QSettings`.
    void loadSettings(QSettings* settings);

private:
    BinaryStreamReaderSettings _settingsWidget;
    unsigned _numChannels;
    unsigned sampleSize;
    bool skipByteRequested;
    bool skipSampleRequested;

    /// points to the readSampleAs function for currently selected number format
    double (BinaryStreamReader::*readSample)();

    /**
     * Reads 1 sample from the device in given format.
     *
     * @note Device should already have enough bytes present before
     * calling this function.
     */
    template<typename T> double readSampleAs();

private slots:
    void onNumberFormatChanged(NumberFormat numberFormat);
    void onNumOfChannelsChanged(unsigned value);
    void onDataReady();
};

#endif // BINARYSTREAMREADER_H
