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

#include <QtGlobal>
#include <string.h>

#include "readonlybuffer.h"

ReadOnlyBuffer::ReadOnlyBuffer(const FrameBuffer* source) :
    ReadOnlyBuffer(source, 0, source->size())
{
    // intentionally empty, see ↑
}

ReadOnlyBuffer::ReadOnlyBuffer(const FrameBuffer* source, unsigned start, unsigned n)
{
    Q_ASSERT(source->size() > 0);
    Q_ASSERT(start + n <= source->size());

    _size = n;
    data = new double[_size];

    for (unsigned i = 0; i < n; i++)
    {
        data[i] = source->sample(start + i);
    }

    /// if not exact copy of source re-calculate limits
    if (start == 0 && n == source->size())
    {
        _limits = source->limits();
    }
    else
    {
        updateLimits();
    }
}

ReadOnlyBuffer::ReadOnlyBuffer(const double* source, unsigned ssize)
{
    Q_ASSERT(source != nullptr && ssize);

    _size = ssize;
    data = new double[_size];
    memcpy(data, source, sizeof(double) * ssize);
    updateLimits();
}

ReadOnlyBuffer::~ReadOnlyBuffer()
{
    delete[] data;
}

unsigned ReadOnlyBuffer::size() const
{
    return _size;
}

double ReadOnlyBuffer::sample(unsigned i) const
{
    return data[i];
}

Range ReadOnlyBuffer::limits() const
{
    return _limits;
}

void ReadOnlyBuffer::updateLimits()
{
    Q_ASSERT(_size);

    _limits.start = data[0];
    _limits.end = data[0];

    for (unsigned i = 0; i < _size; i++)
    {
        if (data[i] > _limits.end)
        {
            _limits.end = data[i];
        }
        else if (data[i] < _limits.start)
        {
            _limits.start = data[i];
        }
    }
}
