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

#include <math.h>
#include "framebufferseries.h"

FrameBufferSeries::FrameBufferSeries(FrameBuffer* buffer)
{
    xAsIndex = true;
    _xmin = 0;
    _xmax = 1;
    _buffer = buffer;
    int_index_start = 0;
    int_index_end = 0;
}

void FrameBufferSeries::setXAxis(bool asIndex, double xmin, double xmax)
{
    xAsIndex = asIndex;
    _xmin = xmin;
    _xmax = xmax;
}

size_t FrameBufferSeries::size() const
{
    return int_index_end - int_index_start;
}

QPointF FrameBufferSeries::sample(size_t i) const
{
    i += int_index_start;
    if (xAsIndex)
    {
        return QPointF(i, _buffer->sample(i));
    }
    else
    {
        return QPointF(i * (_xmax - _xmin) / _buffer->size() + _xmin, _buffer->sample(i));
    }
}

QRectF FrameBufferSeries::boundingRect() const
{
    if (xAsIndex)
    {
        return _buffer->boundingRect();
    }
    else
    {
        auto rect = _buffer->boundingRect();
        rect.setLeft(_xmin);
        rect.setRight(_xmax);
        return rect;
    }
}

void FrameBufferSeries::setRectOfInterest(const QRectF& rect)
{
    if (xAsIndex)
    {
        int_index_start = floor(rect.left())-1;
        int_index_end = ceil(rect.right())+1;
    }
    else
    {
        double xsize = _xmax - _xmin;
        size_t bsize = _buffer->size();
        int_index_start =  floor(bsize * (rect.left()-_xmin) / xsize)-1;
        int_index_end = ceil(bsize * (rect.right()-_xmin) / xsize)+1;
    }

    int_index_start = std::max(int_index_start, (size_t) 0);
    int_index_end = std::min(_buffer->size(), int_index_end);
}
