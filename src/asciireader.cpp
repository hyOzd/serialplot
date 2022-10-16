/*
  Copyright © 2020 Hasan Yavuz Özderya

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

#include "asciireader.h"

/// If set to this value number of channels is determined from input
#define NUMOFCHANNELS_AUTO   (0)

AsciiReader::AsciiReader(QIODevice* device, QObject* parent) :
    AbstractReader(device, parent)
{
    paused = false;

    _numChannels = _settingsWidget.numOfChannels();
    autoNumOfChannels = (_numChannels == NUMOFCHANNELS_AUTO);
    delimiter = _settingsWidget.delimiter();
    isHexData = _settingsWidget.isHex();

    connect(&_settingsWidget, &AsciiReaderSettings::numOfChannelsChanged,
            [this](unsigned value)
            {
                _numChannels = value;
                updateNumChannels(); // TODO: setting numchannels = 0, should remove all buffers
                                     // do we want this?
                autoNumOfChannels = (_numChannels == NUMOFCHANNELS_AUTO);
                if (!autoNumOfChannels)
                {
                    emit numOfChannelsChanged(value);
                }
            });

    connect(&_settingsWidget, &AsciiReaderSettings::delimiterChanged,
            [this](QChar d)
            {
                delimiter = d;
            });
    connect(&_settingsWidget, &AsciiReaderSettings::filterChanged,
            [this](AsciiReaderSettings::FilterMode mode, QString prefix)
            {
                filterMode = mode;
                filterPrefix = prefix;
            });
    connect(&_settingsWidget, &AsciiReaderSettings::hexChanged,
            [this](bool hexData)
            {
                isHexData = hexData;
            });
}

QWidget* AsciiReader::settingsWidget()
{
    return &_settingsWidget;
}

unsigned AsciiReader::numChannels() const
{
    // TODO: an alternative is to never set _numChannels to '0'
    // do not allow '0'
    return _numChannels == 0 ? 1 : _numChannels;
}

void AsciiReader::enable(bool enabled)
{
    if (enabled)
    {
        firstReadAfterEnable = true;
    }

    AbstractReader::enable(enabled);
}

unsigned AsciiReader::readData()
{
    unsigned numBytesRead = 0;

    while(_device->canReadLine())
    {
        QByteArray bytes = _device->readLine();
        QString line = QString(bytes);
        numBytesRead += bytes.size();

        // discard only once when we just started reading
        if (firstReadAfterEnable)
        {
            firstReadAfterEnable = false;
            continue;
        }

        // discard data if paused
        if (paused)
        {
            continue;
        }

        // parse data
        line = line.trimmed();

        // Note: When data coming from pseudo terminal is buffered by
        // system CR is converted to LF for some reason. This causes
        // empty lines in the input when the port is just opened.
        if (line.isEmpty())
        {
            continue;
        }

        switch (filterMode)
        {
            // skip lines that match the prefix
            case AsciiReaderSettings::FilterMode::exclude:
                if (line.startsWith(filterPrefix)) continue;
                break;
            // skip lines that doesn't match, and cut off prefix
            case AsciiReaderSettings::FilterMode::include:
                if (!line.startsWith(filterPrefix)) continue;
                line = line.remove(0, filterPrefix.length()).trimmed();
                break;
            case AsciiReaderSettings::FilterMode::disabled:
                break;
        }

        const SamplePack* samples = parseLine(line);
        if (samples != nullptr) {
            // update number of channels if in auto mode
            if (autoNumOfChannels ) {
                unsigned nc = samples->numChannels();
                if (nc != _numChannels) {
                    _numChannels = nc;
                    updateNumChannels();
                    // TODO: is `numOfChannelsChanged` signal still used?
                    emit numOfChannelsChanged(nc);
                }
            }

            Q_ASSERT(samples->numChannels() == _numChannels);

            // commit data
            feedOut(*samples);
            delete samples;
        }
    }

    return numBytesRead;
}

SamplePack* AsciiReader::parseLine(const QString& line) const
{
    auto separatedValues = line.split(delimiter, QString::SkipEmptyParts);
    unsigned numComingChannels = separatedValues.length();

    // check number of channels (skipped if auto num channels is enabled)
    if ((!numComingChannels) || (!autoNumOfChannels && numComingChannels != _numChannels))
    {
        qWarning() << "Line parsing error: invalid number of channels!";
        qWarning() << "Read line: " << line;
        return nullptr;
    }

    // parse data per channel
    auto samples = new SamplePack(1, numComingChannels);
    for (unsigned ci = 0; ci < numComingChannels; ci++)
    {
        bool ok;
        if (isHexData)
        {
            samples->data(ci)[0] = separatedValues[ci].toInt(&ok,16);
        }
        else
        {
            samples->data(ci)[0] = separatedValues[ci].toDouble(&ok);
        }
        if (!ok)
        {
            qWarning() << "Data parsing error for channel: " << ci;
            qWarning() << "Read line: " << line;

            delete samples;
            return nullptr;
        }
    }

    return samples;
}

void AsciiReader::saveSettings(QSettings* settings)
{
    _settingsWidget.saveSettings(settings);
}

void AsciiReader::loadSettings(QSettings* settings)
{
    _settingsWidget.loadSettings(settings);
}
