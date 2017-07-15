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

#ifndef CHUNKEDBUFFER_H
#define CHUNKEDBUFFER_H

#include <QPointF>
#include <QRectF>
#include <QVector>

#define CHUNK_SIZE (1024)

class ChunkedBuffer
{
public:
    ChunkedBuffer();
    ~ChunkedBuffer();

    void addSamples(double* samples, size_t size);
    void clear();

    // QwtSeriesData related implementations
    size_t size() const;
    QRectF boundingRect() const;
    double sample(size_t i) const;

private:
    size_t _size; // size of `data`
    size_t numChunks;

    QList<DataChunk*> chunks;
};


#endif // CHUNKEDBUFFER_H
