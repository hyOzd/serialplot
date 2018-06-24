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

unsigned AbstractReader::numChannels() const
{
    // TODO: an alternative is to never set _numChannels to '0'
    // do not allow '0'
    return _numChannels == 0 ? 1 : _numChannels;
}
