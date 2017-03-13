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

#include "framebufferseries.h"

FrameBufferSeries::FrameBufferSeries(FrameBuffer* buffer)
{
    xAsIndex = true;
    _xmin = 0;
    _xmax = 1;
    _buffer = buffer;
}

void FrameBufferSeries::setXAxis(bool asIndex, double xmin, double xmax)
{
    xAsIndex = asIndex;
    _xmin = xmin;
    _xmax = xmax;
}

size_t FrameBufferSeries::size() const
{
    return _buffer->size();
}

QPointF FrameBufferSeries::sample(size_t i) const
{
    if (xAsIndex)
    {
        return QPointF(i, _buffer->sample(i));
    }
    else
    {
        return QPointF(i * (_xmax - _xmin) / size() + _xmin, _buffer->sample(i));
    }
}

QRectF FrameBufferSeries::boundingRect() const
{
    return _buffer->boundingRect();
}
