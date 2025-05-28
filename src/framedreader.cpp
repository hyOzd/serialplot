/*
  Copyright © 2025 Hasan Yavuz Özderya

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

#include "framedreader.h"

FramedReader::FramedReader(QIODevice* device, QObject* parent) :
    AbstractReader(device, parent)
{
    paused = false;

    // initial settings
    settingsInvalid = 0;
    _numChannels = _settingsWidget.numOfChannels();
    hasSizeByte = (_settingsWidget.sizeFieldType() != FramedReaderSettings::SizeFieldType::Fixed);
    isSizeField2B = (_settingsWidget.sizeFieldType() == FramedReaderSettings::SizeFieldType::Field2Byte);
    frameSize = _settingsWidget.fixedFrameSize();
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

    connect(&_settingsWidget, &FramedReaderSettings::sizeFieldChanged,
            this, &FramedReader::onSizeFieldChanged);

    connect(&_settingsWidget, &FramedReaderSettings::checksumChanged,
            [this](bool enabled){checksumEnabled = enabled; reset();});

    connect(&_settingsWidget, &FramedReaderSettings::debugModeChanged,
            [this](bool enabled){debugModeEnabled = enabled;});

    // init reader state
    reset();
}

QWidget* FramedReader::settingsWidget()
{
    return &_settingsWidget;
}

unsigned FramedReader::numChannels() const
{
    return _numChannels;
}

void FramedReader::onNumberFormatChanged(NumberFormat numberFormat)
{
    switch(numberFormat)
    {
        case NumberFormat_uint8:
            sampleSize = sizeof(quint8);
            readSample = &FramedReader::readSampleAs<quint8>;
            break;
        case NumberFormat_int8:
            sampleSize = sizeof(qint8);
            readSample = &FramedReader::readSampleAs<qint8>;
            break;
        case NumberFormat_uint16:
            sampleSize = sizeof(quint16);
            readSample = &FramedReader::readSampleAs<quint16>;
            break;
        case NumberFormat_int16:
            sampleSize = sizeof(qint16);
            readSample = &FramedReader::readSampleAs<qint16>;
            break;
        case NumberFormat_uint32:
            sampleSize = sizeof(quint32);
            readSample = &FramedReader::readSampleAs<quint32>;
            break;
        case NumberFormat_int32:
            sampleSize = sizeof(qint32);
            readSample = &FramedReader::readSampleAs<qint32>;
            break;
        case NumberFormat_float:
            sampleSize = sizeof(float);
            readSample = &FramedReader::readSampleAs<float>;
            break;
        case NumberFormat_double:
            sampleSize = sizeof(double);
            readSample = &FramedReader::readSampleAs<double>;
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
    if (!hasSizeByte && (frameSize % (_numChannels * sampleSize) != 0))
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
        _settingsWidget.showMessage("Frame Start is invalid!", true);
    }
    else if (settingsInvalid & FRAMESIZE_INVALID)
    {
        QString errorMessage =
            QString("Payload size must be multiple of %1 (#channels * sample size)!")\
            .arg(_numChannels * sampleSize);

        _settingsWidget.showMessage(errorMessage, true);
    }
    else
    {
        _settingsWidget.showMessage("Settings are okay.");
    }
}

void FramedReader::onNumOfChannelsChanged(unsigned value)
{
    _numChannels = value;
    checkSettings();
    reset();
    updateNumChannels();
    emit numOfChannelsChanged(value);
}

void FramedReader::onSyncWordChanged(QByteArray word)
{
    syncWord = word;
    checkSettings();
    reset();
}

void FramedReader::onSizeFieldChanged(FramedReaderSettings::SizeFieldType fieldType, unsigned size)
{
    if (fieldType == FramedReaderSettings::SizeFieldType::Fixed)
    {
        hasSizeByte = false;
        frameSize = size;
    }
    else
    {
        hasSizeByte = true;
        isSizeField2B = (fieldType == FramedReaderSettings::SizeFieldType::Field2Byte);
    }

    checkSettings();
    reset();
}

unsigned FramedReader::readData()
{
    unsigned numBytesRead = 0;

    if (settingsInvalid) return numBytesRead;

    // loop until we run out of bytes or more bytes is required
    unsigned bytesAvailable;
    while ((bytesAvailable = _device->bytesAvailable()))
    {
        if (!gotSync) // read sync word
        {
            char c;
            _device->getChar(&c);
            numBytesRead++;
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
            // read size field (1 or 2 bytes)
            if (isSizeField2B)
            {
                if (bytesAvailable < 2) break;

                uint16_t frameSize16 = 0;
                _device->read((char*) &frameSize16, sizeof(frameSize16));
                numBytesRead += sizeof(frameSize16);

                if (_settingsWidget.endianness() == LittleEndian)
                {
                    frameSize = qFromLittleEndian(frameSize16);
                }
                else
                {
                    frameSize = qFromBigEndian(frameSize16);
                }
            }
            else
            {
                frameSize = 0;
                _device->getChar((char*) &frameSize);
                numBytesRead++;
            }

            // validate the size field
            if (frameSize == 0)
            {
                qCritical() << "Frame size is read as 0!";
                reset();
            }
            else if (frameSize % (_numChannels * sampleSize) != 0)
            {
                qCritical() <<
                    QString("Payload size is not multiple of %1 (#channels * sample size)!") \
                    .arg(_numChannels * sampleSize);
                reset();
            }
            else
            {
                if (debugModeEnabled) qDebug() << "Payload size:" << frameSize;
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
                numBytesRead += checksumEnabled ? frameSize+1 : frameSize;
                reset();
            }
        }
    }

    return numBytesRead;
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
        _device->read(checksumEnabled ? frameSize+1 : frameSize);
        return;
    }

    // a package is 1 set of samples for all channels
    unsigned numOfPackagesToRead = frameSize / (_numChannels * sampleSize);
    SamplePack samples(numOfPackagesToRead, _numChannels);
    for (unsigned i = 0; i < numOfPackagesToRead; i++)
    {
        for (unsigned int ci = 0; ci < _numChannels; ci++)
        {
            samples.data(ci)[i] = (this->*readSample)();
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
        feedOut(samples);
    }
    else
    {
        qCritical() << "Checksum failed! Received:" << rChecksum << "Calculated:" << calcChecksum;
    }
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
