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

#include <math.h>

#include "demoreader.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DemoReader::DemoReader(QIODevice* device, ChannelManager* channelMan, QObject *parent) :
    AbstractReader(device, channelMan, parent)
{
    paused = false;
    _numOfChannels = 1;
    count = 0;
    timer.setInterval(100);
    QObject::connect(&timer, &QTimer::timeout,
                     this, &DemoReader::demoTimerTimeout);
}

QWidget* DemoReader::settingsWidget()
{
    return NULL;
}

void DemoReader::enable(bool enabled)
{
    if (enabled)
    {
        timer.start();
    }
    else
    {
        timer.stop();
    }
}

unsigned DemoReader::numOfChannels()
{
    return _numOfChannels;
}

void DemoReader::setNumOfChannels(unsigned value)
{
    _numOfChannels = value;
}

void DemoReader::pause(bool enabled)
{
    paused = enabled;
}

void DemoReader::demoTimerTimeout()
{
    const double period = 100;
    count++;
    if (count >= 100) count = 0;

    if (!paused)
    {
        for (unsigned ci = 0; ci < _numOfChannels; ci++)
        {
            // we are calculating the fourier components of square wave
            double value = 4*sin(2*M_PI*double((ci+1)*count)/period)/((2*(ci+1))*M_PI);
            _channelMan->addChannelData(ci, &value, 1);
            sampleCount++;
        }
        emit dataAdded();
    }
}
