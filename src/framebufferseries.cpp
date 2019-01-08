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

#include <math.h>
#include "framebufferseries.h"

FrameBufferSeries::FrameBufferSeries(const XFrameBuffer* x, const FrameBuffer* y)
{
    _x = x;
    _y = y;

    int_index_start = 0;
    int_index_end = _y->size();
}

void FrameBufferSeries::setX(const XFrameBuffer* x)
{
    _x = x;
}

size_t FrameBufferSeries::size() const
{
    return int_index_end - int_index_start + 1;
}

QPointF FrameBufferSeries::sample(size_t i) const
{
    i += int_index_start;
    return QPointF(_x->sample(i), _y->sample(i));
}

QRectF FrameBufferSeries::boundingRect() const
{
    QRectF rect;
    auto yLim = _y->limits();
    auto xLim = _x->limits();
    rect.setBottom(yLim.start);
    rect.setTop(yLim.end);
    rect.setLeft(xLim.start);
    rect.setRight(xLim.end);

    return rect.normalized();
}

void FrameBufferSeries::setRectOfInterest(const QRectF& rect)
{
    int_index_start = _x->findIndex(rect.left());
    int_index_end = _x->findIndex(rect.right());

    if (int_index_start == XFrameBuffer::OUT_OF_RANGE)
    {
        int_index_start = 0;
    }
    else if (int_index_start > 0)
    {
        int_index_start -= 1;
    }

    if (int_index_end == XFrameBuffer::OUT_OF_RANGE)
    {
        int_index_end = _x->size()-1;
    }
    else if (int_index_end < (int)_x->size()-1)
    {
        int_index_end += 1;
    }
}
