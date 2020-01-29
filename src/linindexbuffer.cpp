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
#include <algorithm>

#include "linindexbuffer.h"

LinIndexBuffer::LinIndexBuffer(unsigned n, Range lim)
{
    // Note that calculation of _step would cause divide by 0
    Q_ASSERT(n > 1);

    _size = n;
    setLimits(lim);
}

unsigned LinIndexBuffer::size() const
{
    return _size;
}

double LinIndexBuffer::sample(unsigned i) const
{
    return _limits.start + i * _step;
}

Range LinIndexBuffer::limits() const
{
    return _limits;
}

void LinIndexBuffer::resize(unsigned n)
{
    _size = n;
    setLimits(_limits);         // called to update `_step`
}

int LinIndexBuffer::findIndex(double value) const
{
    if (value < _limits.start || value > _limits.end)
    {
        return OUT_OF_RANGE;
    }

    int r = (value - _limits.start) / _step;
    // Note: we are limiting return value because of floating point in-accuracies
    return std::min<int>(std::max<int>(r, 0), (_size-1));
}

void LinIndexBuffer::setLimits(Range lim)
{
    _limits = lim;
    _step = (lim.end - lim.start) / (_size-1);
}
