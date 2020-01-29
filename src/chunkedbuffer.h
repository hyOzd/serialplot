/*
  Copyright © 2020 Hasan Yavuz Özderya

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

#include "datachunk.h"
#include "framebuffer.h"

#define CHUNK_SIZE (1024)

class ChunkedBuffer : public WFrameBuffer
{
public:
    ChunkedBuffer();
    ~ChunkedBuffer();

    // FrameBuffer related implementations
    virtual unsigned size() const;
    virtual double sample(unsigned i) const;
    virtual Range limits() const;
    virtual void resize(unsigned n);
    virtual void addSamples(double* samples, unsigned n);
    virtual void clear();

private:
    size_t _size; // size of `data`
    size_t numChunks;

    QList<DataChunk*> chunks;
    DataChunk* addNewChunk();
};


#endif // CHUNKEDBUFFER_H
