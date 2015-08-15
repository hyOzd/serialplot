/*
  Copyright © 2015 Hasan Yavuz Özderya

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

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <qwt_series_data.h>
#include <QPointF>
#include <QRectF>

class FrameBuffer : public QwtSeriesData<QPointF>
{
public:
    FrameBuffer(size_t size);
    ~FrameBuffer();

    void resize(size_t size);
    void addSamples(double* samples, size_t size);
    void clear(); // fill 0

    // QwtSeriesData implementations
    size_t size() const;
    QPointF sample(size_t i) const;
    QRectF boundingRect() const;

private:
    size_t _size; // size of `data`
    double* data;
    size_t headIndex; // indicates the actual `0` index of the ring buffer

    double _sample(size_t i) const;
};

#endif // FRAMEBUFFER_H
