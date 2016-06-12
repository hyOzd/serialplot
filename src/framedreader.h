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

#ifndef FRAMEDREADER_H
#define FRAMEDREADER_H

#include "abstractreader.h"
#include "framedreadersettings.h"

/**
 * Reads data in a customizable framed format.
 */
class FramedReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit FramedReader(QIODevice* device, ChannelManager* channelMan, QObject *parent = 0);
    QWidget* settingsWidget();
    unsigned numOfChannels();
    void enable(bool enabled = true);

public slots:
    void pause(bool);

private:
    /// bit wise fields for `settingsValid` member
    enum SettingInvalidFlag
    {
        SYNCWORD_INVALID = 1,
        FRAMESIZE_INVALID = 2
    };

    // settings related members
    FramedReaderSettings _settingsWidget;
    unsigned _numOfChannels;
    unsigned sampleSize;
    bool paused;
    unsigned settingsInvalid;   /// settings are all valid if this is 0, if not no reading is done
    QByteArray syncWord;
    bool checksumEnabled;
    bool hasSizeByte;
    unsigned frameSize;
    bool debugModeEnabled;

    /// Checks the validity of syncWord and frameSize then shows an
    /// error message. Also updates `settingsInvalid`. If settings are
    /// valid `settingsInvalid` should be `0`.
    void checkSettings();

    // read state related members
    unsigned sync_i; /// sync byte index to be read next
    bool gotSync;    /// indicates if sync word is captured
    bool gotSize;    /// indicates if size is captured, ignored if size byte is disabled (fixed size)
    unsigned calcChecksum;

    void reset();    /// Resets the reading state. Used in case of error or setting change.
    /// points to the readSampleAs function for currently selected number format
    double (FramedReader::*readSample)();
    template<typename T> double readSampleAs();
    /// reads payload portion of the frame, calculates checksum and commits data
    /// @note should be called only if there are enough bytes on device
    void readFrameDataAndCheck();
    // `data` contains i th channels data
    void addChannelData(unsigned int channel, double* data, unsigned size);

private slots:
    void onDataReady();

    void onNumberFormatChanged(NumberFormat numberFormat);
    void onNumOfChannelsChanged(unsigned value);
    void onSyncWordChanged(QByteArray);
    void onFrameSizeChanged(unsigned);
};

#endif // FRAMEDREADER_H
