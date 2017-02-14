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

#include "abstractreader.h"

AbstractReader::AbstractReader(QIODevice* device, ChannelManager* channelMan, QObject *parent) :
    QObject(parent)
{
    _device = device;
    _channelMan = channelMan;

    // initialize sps counter
    sampleCount = 0;
    samplesPerSecond = 0;
    QObject::connect(&spsTimer, &QTimer::timeout,
                     this, &AbstractReader::spsTimerTimeout);
    // TODO: start sps timer when reader is enabled
    spsTimer.start(SPS_UPDATE_TIMEOUT * 1000);
}

void AbstractReader::spsTimerTimeout()
{
    unsigned currentSps = samplesPerSecond;
    samplesPerSecond = (sampleCount/numOfChannels())/SPS_UPDATE_TIMEOUT;
    if (currentSps != samplesPerSecond)
    {
        emit samplesPerSecondChanged(samplesPerSecond);
    }
    sampleCount = 0;
}

void AbstractReader::addData(double* samples, unsigned length)
{
    _channelMan->addData(samples, length);
    sampleCount += length;
}
