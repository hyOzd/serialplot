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

#include <QtEndian>
#include <QtDebug>

#include "binarystreamreader.h"

BinaryStreamReader::BinaryStreamReader(QIODevice* device, QObject* parent) :
    AbstractReader(device, parent)
{
    paused = false;
    skipByteRequested = false;
    skipSampleRequested = false;

    _numChannels = _settingsWidget.numOfChannels();
    connect(&_settingsWidget, &BinaryStreamReaderSettings::numOfChannelsChanged,
                     this, &BinaryStreamReader::onNumOfChannelsChanged);

    // initial number format selection
    onNumberFormatChanged(_settingsWidget.numberFormat());
    connect(&_settingsWidget, &BinaryStreamReaderSettings::numberFormatChanged,
            this, &BinaryStreamReader::onNumberFormatChanged);

    // enable skip byte and sample buttons
    connect(&_settingsWidget, &BinaryStreamReaderSettings::skipByteRequested,
            [this]()
            {
                skipByteRequested = true;
            });
    connect(&_settingsWidget, &BinaryStreamReaderSettings::skipSampleRequested,
            [this]()
            {
                skipSampleRequested = true;
            });
}

QWidget* BinaryStreamReader::settingsWidget()
{
    return &_settingsWidget;
}

unsigned BinaryStreamReader::numChannels() const
{
    return _numChannels;
}

void BinaryStreamReader::onNumberFormatChanged(NumberFormat numberFormat)
{
    switch(numberFormat)
    {
        case NumberFormat_uint8:
            sampleSize = sizeof(quint8);
            readSample = &BinaryStreamReader::readSampleAs<quint8>;
            break;
        case NumberFormat_int8:
            sampleSize = sizeof(qint8);
            readSample = &BinaryStreamReader::readSampleAs<qint8>;
            break;
        case NumberFormat_uint16:
            sampleSize = sizeof(quint16);
            readSample = &BinaryStreamReader::readSampleAs<quint16>;
            break;
        case NumberFormat_int16:
            sampleSize = sizeof(qint16);
            readSample = &BinaryStreamReader::readSampleAs<qint16>;
            break;
        case NumberFormat_uint32:
            sampleSize = sizeof(quint32);
            readSample = &BinaryStreamReader::readSampleAs<quint32>;
            break;
        case NumberFormat_int32:
            sampleSize = sizeof(qint32);
            readSample = &BinaryStreamReader::readSampleAs<qint32>;
            break;
        case NumberFormat_float:
            sampleSize = sizeof(float);
            readSample = &BinaryStreamReader::readSampleAs<float>;
            break;
        case NumberFormat_double:
            sampleSize = sizeof(double);
            readSample = &BinaryStreamReader::readSampleAs<double>;
            break;
        case NumberFormat_INVALID:
            Q_ASSERT(1); // never
            break;
    }
}

void BinaryStreamReader::onNumOfChannelsChanged(unsigned value)
{
    _numChannels = value;
    updateNumChannels();
    emit numOfChannelsChanged(value);
}

unsigned BinaryStreamReader::readData()
{
    // a package is a set of channel data like {CHAN0_SAMPLE, CHAN1_SAMPLE...}
    unsigned packageSize = sampleSize * _numChannels;
    unsigned bytesAvailable = _device->bytesAvailable();
    unsigned totalRead = 0;

    // skip 1 byte if requested
    if (skipByteRequested && bytesAvailable > 0)
    {
        _device->read(1);
        totalRead++;
        skipByteRequested = false;
        bytesAvailable--;
    }

    // skip 1 sample (channel) if requested
    if (skipSampleRequested && bytesAvailable >= sampleSize)
    {
        _device->read(sampleSize);
        totalRead += sampleSize;
        skipSampleRequested = false;
        bytesAvailable -= sampleSize;
    }

    if (bytesAvailable < packageSize) return totalRead;

    unsigned numOfPackagesToRead =
        (bytesAvailable - (bytesAvailable % packageSize)) / packageSize;
    unsigned numBytesToRead = numOfPackagesToRead * packageSize;

    totalRead += numBytesToRead;

    if (paused)
    {
        // read and discard data
        _device->read(numBytesToRead);
        return totalRead;
    }

    // actual reading
    SamplePack samples(numOfPackagesToRead, _numChannels);
    for (unsigned i = 0; i < numOfPackagesToRead; i++)
    {
        for (unsigned ci = 0; ci < _numChannels; ci++)
        {
            samples.data(ci)[i] = (this->*readSample)();
        }
    }
    feedOut(samples);

    return totalRead;
}

template<typename T> double BinaryStreamReader::readSampleAs()
{
    T data;

    _device->read((char*) &data, sizeof(data));

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

void BinaryStreamReader::saveSettings(QSettings* settings)
{
    _settingsWidget.saveSettings(settings);
}

void BinaryStreamReader::loadSettings(QSettings* settings)
{
    _settingsWidget.loadSettings(settings);
}
