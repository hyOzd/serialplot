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

#include <QtDebug>

#include "asciireader.h"

/// If set to this value number of channels is determined from input
#define NUMOFCHANNELS_AUTO   (0)

AsciiReader::AsciiReader(QIODevice* device, QObject* parent) :
    AbstractReader(device, parent)
{
    paused = false;
    discardFirstLine = true;

    _numChannels = _settingsWidget.numOfChannels();
    autoNumOfChannels = (_numChannels == NUMOFCHANNELS_AUTO);
    delimiter = _settingsWidget.delimiter();

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

    connect(device, &QIODevice::aboutToClose, [this](){discardFirstLine=true;});
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
        disconnectSinks();
    }
}

void AsciiReader::onDataReady()
{
    while(_device->canReadLine())
    {
        QString line = QString(_device->readLine());

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

        auto separatedValues = line.split(delimiter, QString::SkipEmptyParts);

        unsigned numReadChannels; // effective number of channels to read
        unsigned numComingChannels = separatedValues.length();

        if (autoNumOfChannels)
        {
            // did number of channels changed?
            if (numComingChannels != _numChannels)
            {
                _numChannels = numComingChannels;
                updateNumChannels();
                emit numOfChannelsChanged(numComingChannels);
            }
            numReadChannels = numComingChannels;
        }
        else if (numComingChannels >= _numChannels)
        {
            numReadChannels = _numChannels;
        }
        else // there is missing channel data
        {
            numReadChannels = separatedValues.length();
            qWarning() << "Incoming data is missing data for some channels!";
            qWarning() << "Read line: " << line;
        }

        // parse read line
        unsigned numDataBroken = 0;
        SamplePack samples(1, _numChannels);
        for (unsigned ci = 0; ci < numReadChannels; ci++)
        {
            bool ok;
            samples.data(ci)[0] = separatedValues[ci].toDouble(&ok);
            if (!ok)
            {
                qWarning() << "Data parsing error for channel: " << ci;
                qWarning() << "Read line: " << line;
                samples.data(ci)[0] = 0;
                numDataBroken++;
            }
        }

        if (numReadChannels > numDataBroken)
        {
            // commit data
            feedOut(samples);
        }
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
