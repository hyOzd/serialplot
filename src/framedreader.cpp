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

#include <QtDebug>
#include <QtEndian>
#include "floatswap.h"

#include "framedreader.h"

FramedReader::FramedReader(QIODevice* device, ChannelManager* channelMan,
                           DataRecorder* recorder, QObject* parent) :
    AbstractReader(device, channelMan, recorder, parent)
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
    debugModeEnabled = _settingsWidget.isDebugModeEnabled();
    checkSettings();

    // init setting connections
    connect(&_settingsWidget, &FramedReaderSettings::numberFormatChanged,
            this, &FramedReader::onNumberFormatChanged);

    connect(&_settingsWidget, &FramedReaderSettings::numOfChannelsChanged,
            this, &FramedReader::onNumOfChannelsChanged);

    connect(&_settingsWidget, &FramedReaderSettings::syncWordChanged,
            this, &FramedReader::onSyncWordChanged);

    connect(&_settingsWidget, &FramedReaderSettings::frameSizeChanged,
            this, &FramedReader::onFrameSizeChanged);

    connect(&_settingsWidget, &FramedReaderSettings::checksumChanged,
            [this](bool enabled){checksumEnabled = enabled; reset();});

    connect(&_settingsWidget, &FramedReaderSettings::debugModeChanged,
            [this](bool enabled){debugModeEnabled = enabled;});

    // init reader state
    reset();
}

void FramedReader::enable(bool enabled)
{
    if (enabled)
    {
        connect(_device, &QIODevice::readyRead,
                this, &FramedReader::onDataReady);
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
        case NumberFormat_INVALID:
            Q_ASSERT(1); // never
            break;
    }

    checkSettings();
    reset();
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
    reset();
    emit numOfChannelsChanged(value);
}

void FramedReader::onSyncWordChanged(QByteArray word)
{
    syncWord = word;
    checkSettings();
    reset();
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
    reset();
}

void FramedReader::onDataReady()
{
    if (settingsInvalid) return;

    // loop until we run out of bytes or more bytes is required
    unsigned bytesAvailable;
    while ((bytesAvailable = _device->bytesAvailable()))
    {
        if (!gotSync) // read sync word
        {
            char c;
            _device->getChar(&c);
            if (c == syncWord[sync_i]) // correct sync byte?
            {
                sync_i++;
                if (sync_i == (unsigned) syncWord.length())
                {
                    gotSync = true;
                }
            }
            else
            {
                if (debugModeEnabled) qCritical() << "Missed " << sync_i+1 << "th sync byte.";
            }
        }
        else if (hasSizeByte && !gotSize) // skipped if fixed frame size
        {
            frameSize = 0;
            _device->getChar((char*) &frameSize);

            if (frameSize == 0) // check size
            {
                qCritical() << "Frame size is 0!";
                reset();
            }
            else if (frameSize % (_numOfChannels * sampleSize) != 0)
            {
                qCritical() <<
                    QString("Frame size is not multiple of %1 (#channels * sample size)!") \
                    .arg(_numOfChannels * sampleSize);
                reset();
            }
            else
            {
                if (debugModeEnabled) qDebug() << "Frame size:" << frameSize;
                gotSize = true;
            }
        }
        else // read data bytes
        {
            // have enough data bytes? (+1 for checksum)
            if (bytesAvailable < (checksumEnabled ? frameSize+1 : frameSize))
            {
                break;
            }
            else // read data bytes and checksum
            {
                readFrameDataAndCheck();
                reset();
            }
        }
    }
}

void FramedReader::reset()
{
    sync_i = 0;
    gotSync = false;
    gotSize = false;
    if (hasSizeByte) frameSize = 0;
    calcChecksum = 0;
}

// Important: this function assumes device has enough bytes to read a full frames data and checksum
void FramedReader::readFrameDataAndCheck()
{
    // if paused just read and waste data
    if (paused)
    {
        _device->read((checksumEnabled ? frameSize+1 : frameSize));
        return;
    }

    // a package is 1 set of samples for all channels
    unsigned numOfPackagesToRead = frameSize / (_numOfChannels * sampleSize);
    double* channelSamples = new double[numOfPackagesToRead * _numOfChannels];

    for (unsigned i = 0; i < numOfPackagesToRead; i++)
    {
        for (unsigned int ci = 0; ci < _numOfChannels; ci++)
        {
            channelSamples[ci*numOfPackagesToRead+i] = (this->*readSample)();
        }
    }

    // read checksum
    unsigned rChecksum = 0;
    bool checksumPassed = false;
    if (checksumEnabled)
    {
        _device->read((char*) &rChecksum, 1);
        calcChecksum &= 0xFF;
        checksumPassed = (calcChecksum == rChecksum);
    }

    if (!checksumEnabled || checksumPassed)
    {
        // commit data
        addData(channelSamples, numOfPackagesToRead*_numOfChannels);
    }
    else
    {
        qCritical() << "Checksum failed! Received:" << rChecksum << "Calculated:" << calcChecksum;
    }

    delete[] channelSamples;
}

template<typename T> double FramedReader::readSampleAs()
{
    T data;

    _device->read((char*) &data, sizeof(data));

    if (checksumEnabled)
    {
        for (unsigned i = 0; i < sizeof(data); i++)
        {
            calcChecksum += ((unsigned char*) &data)[i];
        }
    }

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

void FramedReader::saveSettings(QSettings* settings)
{
    _settingsWidget.saveSettings(settings);
}

void FramedReader::loadSettings(QSettings* settings)
{
    _settingsWidget.loadSettings(settings);
}
