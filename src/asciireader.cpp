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

#include "asciireader.h"

AsciiReader::AsciiReader(QIODevice* device, ChannelManager* channelMan, QObject *parent) :
    AbstractReader(device, channelMan, parent)
{
    paused = false;
    sampleCount = 0;

    _numOfChannels = _settingsWidget.numOfChannels();
    connect(&_settingsWidget, &AsciiReaderSettings::numOfChannelsChanged,
            this, &AsciiReader::numOfChannelsChanged);
    connect(&_settingsWidget, &AsciiReaderSettings::numOfChannelsChanged,
            [this](unsigned value){_numOfChannels = value;});
}

QWidget* AsciiReader::settingsWidget()
{
    return &_settingsWidget;
}

unsigned AsciiReader::numOfChannels()
{
    return _numOfChannels;
}

// TODO: this could be a part of AbstractReader
void AsciiReader::enable(bool enabled)
{
    if (enabled)
    {
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

        // discard data if paused
        if (paused)
        {
            return;
        }

        line = line.trimmed();
        auto separatedValues = line.split(',');

        int numReadChannels; // effective number of channels to read
        if (separatedValues.length() >= int(_numOfChannels))
        {
            numReadChannels = _numOfChannels;
        }
        else // there is missing channel data
        {
            numReadChannels = separatedValues.length();
            qWarning() << "Incoming data is missing data for some channels!";
        }

        // parse read line
        for (int ci = 0; ci < numReadChannels; ci++)
        {
            bool ok;
            double channelSample = separatedValues[ci].toDouble(&ok);
            if (ok)
            {
                _channelMan->addChannelData(ci, &channelSample, 1);
                sampleCount++;
            }
            else
            {
                qWarning() << "Data parsing error for channel: " << ci;
            }
        }
        emit dataAdded();
    }
}
