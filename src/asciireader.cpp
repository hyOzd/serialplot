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

#include "asciireader.h"

/// If set to this value number of channels is determined from input
#define NUMOFCHANNELS_AUTO   (0)

AsciiReader::AsciiReader(QIODevice* device, ChannelManager* channelMan, QObject *parent) :
    AbstractReader(device, channelMan, parent)
{
    paused = false;
    discardFirstLine = true;
    sampleCount = 0;

    _numOfChannels = _settingsWidget.numOfChannels();
    autoNumOfChannels = (_numOfChannels == NUMOFCHANNELS_AUTO);

    connect(&_settingsWidget, &AsciiReaderSettings::numOfChannelsChanged,
            [this](unsigned value)
            {
                _numOfChannels = value;
                autoNumOfChannels = (_numOfChannels == NUMOFCHANNELS_AUTO);
                if (!autoNumOfChannels)
                {
                    emit numOfChannelsChanged(value);
                }
            });

    connect(device, &QIODevice::aboutToClose, [this](){discardFirstLine=true;});
}

QWidget* AsciiReader::settingsWidget()
{
    return &_settingsWidget;
}

unsigned AsciiReader::numOfChannels()
{
    // do not allow '0'
    if (_numOfChannels == 0)
    {
        return 1;
    }
    else
    {
        return _numOfChannels;
    }
}

// TODO: this could be a part of AbstractReader
void AsciiReader::enable(bool enabled)
{
    if (enabled)
    {
        discardFirstLine = true;
        QObject::connect(_device, &QIODevice::readyRead,
                         this, &AsciiReader::onDataReady);
    }
    else
    {
        QObject::disconnect(_device, 0, this, 0);
    }
}

void AsciiReader::pause(bool enabled)
{
    paused = enabled;
}

void AsciiReader::onDataReady()
{
    while(_device->canReadLine())
    {
        QByteArray line = _device->readLine();

        // discard only once when we just started reading
        if (discardFirstLine)
        {
            discardFirstLine = false;
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

        auto separatedValues = line.split(',');

        unsigned numReadChannels; // effective number of channels to read
        unsigned numComingChannels = separatedValues.length();

        if (autoNumOfChannels)
        {
            // did number of channels changed?
            if (numComingChannels != _numOfChannels)
            {
                _numOfChannels = numComingChannels;
                emit numOfChannelsChanged(numComingChannels);
            }
            numReadChannels = numComingChannels;
        }
        else if (numComingChannels >= _numOfChannels)
        {
            numReadChannels = _numOfChannels;
        }
        else // there is missing channel data
        {
            numReadChannels = separatedValues.length();
            qWarning() << "Incoming data is missing data for some channels!";
        }

        // parse read line
        double* channelSamples = new double[_numOfChannels]();
        for (unsigned ci = 0; ci < numReadChannels; ci++)
        {
            bool ok;
            channelSamples[ci] = separatedValues[ci].toDouble(&ok);
            if (!ok)
            {
                qWarning() << "Data parsing error for channel: " << ci;
                channelSamples[ci] = 0;
            }
        }

        // commit data
        _channelMan->addData(channelSamples, _numOfChannels);
        sampleCount += numReadChannels;
        emit dataAdded();

        delete[] channelSamples;
    }
}

void AsciiReader::saveSettings(QSettings* settings)
{
    _settingsWidget.saveSettings(settings);
}

void AsciiReader::loadSettings(QSettings* settings)
{
    _settingsWidget.loadSettings(settings);
}
