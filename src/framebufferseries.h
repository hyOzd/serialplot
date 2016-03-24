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

#ifndef FRAMEBUFFERSERIES_H
#define FRAMEBUFFERSERIES_H

#include <QPointF>
#include <QRectF>
#include <qwt_series_data.h>

#include "framebuffer.h"

/**
 * This class provides an interface for actual FrameBuffer
 * object. That way we can keep our data structures relatively
 * isolated from Qwt. Otherwise QwtPlotCurve owns FrameBuffer
 * structures.
 */
class FrameBufferSeries : public QwtSeriesData<QPointF>
{
public:
    FrameBufferSeries(FrameBuffer* buffer);

    // QwtSeriesData implementations
    size_t size() const;
    QPointF sample(size_t i) const;
    QRectF boundingRect() const;

private:
    FrameBuffer* _buffer;
};

#endif // FRAMEBUFFERSERIES_H
