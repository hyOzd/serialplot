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

#include <QtDebug>
#include <QtEndian>
#include "floatswap.h"

#include "framedreader.h"

FramedReader::FramedReader(QIODevice* device, ChannelManager* channelMan, QObject *parent) :
    AbstractReader(device, channelMan, parent)
{
    paused = false;

    // initial settings
    settingsInvalid = 0;
    _numOfChannels = _settingsWidget.numOfChannels();
    hasSizeByte = _settingsWidget.frameSize() == 0;
    frameSize = _settingsWidget.frameSize();
    syncWord = _settingsWidget.syncWord();
    checksumEnabled = _settingsWidget.isChecksumEnabled();
    onNumberFormatChanged(_settingsWidget.numberFormat());
    checkSettings();

    connect(&_settingsWidget, &FramedReaderSettings::numberFormatChanged,
            this, &FramedReader::onNumberFormatChanged);

    connect(&_settingsWidget, &FramedReaderSettings::numOfChannelsChanged,
            this, &FramedReader::onNumOfChannelsChanged);

    connect(&_settingsWidget, &FramedReaderSettings::syncWordChanged,
            this, &FramedReader::onSyncWordChanged);

    connect(&_settingsWidget, &FramedReaderSettings::frameSizeChanged,
            this, &FramedReader::onFrameSizeChanged);

    connect(&_settingsWidget, &FramedReaderSettings::checksumChanged,
            [this](bool enabled){checksumEnabled = enabled;});
}

void FramedReader::enable(bool enabled)
{
    if (enabled)
    {
        // TODO
        // QObject::connect(_device, &QIODevice::readyRead,
                         // this, &BinaryStreamReader::onDataReady);
    }
    else
    {
        QObject::disconnect(_device, 0, this, 0);
    }
}

QWidget* FramedReader::settingsWidget()
{
    return &_settingsWidget;
}

unsigned FramedReader::numOfChannels()
{
    return _numOfChannels;
}

void FramedReader::pause(bool enabled)
{
    paused = enabled;
}

void FramedReader::onNumberFormatChanged(NumberFormat numberFormat)
{
    switch(numberFormat)
    {
        case NumberFormat_uint8:
            sampleSize = 1;
            readSample = &FramedReader::readSampleAs<quint8>;
            break;
        case NumberFormat_int8:
            sampleSize = 1;
            readSample = &FramedReader::readSampleAs<qint8>;
            break;
        case NumberFormat_uint16:
            sampleSize = 2;
            readSample = &FramedReader::readSampleAs<quint16>;
            break;
        case NumberFormat_int16:
            sampleSize = 2;
            readSample = &FramedReader::readSampleAs<qint16>;
            break;
        case NumberFormat_uint32:
            sampleSize = 4;
            readSample = &FramedReader::readSampleAs<quint32>;
            break;
        case NumberFormat_int32:
            sampleSize = 4;
            readSample = &FramedReader::readSampleAs<qint32>;
            break;
        case NumberFormat_float:
            sampleSize = 4;
            readSample = &FramedReader::readSampleAs<float>;
            break;
    }

    checkSettings();
}

template<typename T> double FramedReader::readSampleAs()
{
    T data;

    _device->read((char*) &data, sizeof(data));

    // TODO: checksum

    if (_settingsWidget.endianness() == LittleEndian)
    {
        data = qFromLittleEndian(data);
    }
    else
    {
        data = qFromBigEndian(data);
    }

    return double(data);
}

void FramedReader::checkSettings()
{
    // sync word is invalid (empty or missing a nibble at the end)
    if (!syncWord.size())
    {
        settingsInvalid |= SYNCWORD_INVALID;
    }
    else // sync word is valid
    {
        settingsInvalid &= ~SYNCWORD_INVALID;
    }

    // check if fixed frame size is multiple of a sample set size
    if (!hasSizeByte && frameSize % (_numOfChannels * sampleSize) != 0)
    {
        settingsInvalid |= FRAMESIZE_INVALID;
    }
    else
    {
        settingsInvalid &= ~FRAMESIZE_INVALID;
    }

    // show an error message
    if (settingsInvalid & SYNCWORD_INVALID)
    {
        _settingsWidget.showMessage("Sync word is invalid!", true);
    }
    else if (settingsInvalid & FRAMESIZE_INVALID)
    {
        QString errorMessage =
            QString("Frame size must be multiple of %1 (#channels * sample size)!")\
            .arg(_numOfChannels * sampleSize);

        _settingsWidget.showMessage(errorMessage, true);
    }
    else
    {
        _settingsWidget.showMessage("All is well!");
    }
}

void FramedReader::onNumOfChannelsChanged(unsigned value)
{
    _numOfChannels = value;
    checkSettings();
}

void FramedReader::onSyncWordChanged(QByteArray word)
{
    syncWord = word;
    checkSettings();
}

void FramedReader::onFrameSizeChanged(unsigned value)
{
    if (value == 0)
    {
        hasSizeByte = true;
    }
    else
    {
        hasSizeByte = false;
        frameSize = value;
    }
    checkSettings();
}
