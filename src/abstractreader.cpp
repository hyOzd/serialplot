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

#include "abstractreader.h"

AbstractReader::AbstractReader(QIODevice* device, QObject* parent) :
    QObject(parent)
{
    _device = device;
}

void AbstractReader::pause(bool enabled)
{
    paused = enabled;
}

void AbstractReader::enable(bool enabled)
{
    if (enabled)
    {
        QObject::connect(_device, &QIODevice::readyRead,
                         this, &AbstractReader::onDataReady);
    }
    else
    {
        QObject::disconnect(_device, 0, this, 0);
        disconnectSinks();
    }
}
