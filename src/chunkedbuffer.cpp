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

#include "chunkedbuffer.h"

ChunkedBuffer::ChunkedBuffer()
{
    numChunks = 0;
    _size = 0;

    // create first chunk
    addNewChunk();
}

ChunkedBuffer::~ChunkedBuffer()
{
    for (auto chunk : chunks)
    {
        delete chunk;
    }
}

void ChunkedBuffer::addSamples(double* samples, size_t size)
{
    size_t i = 0;

    while (i < size)
    {
        // select chunk to add data
        auto chunk = chunks.last();
        if (chunk->isFull())
        {
            chunk = addNewChunk(); // create a new chunk
        }

        // add data to chunk
        size_t c = std::min(chunk->left(), (size - i));
        chunk->addSamples(&samples[i], c);
        i += c;
    }

    _size += size;
}

DataChunk* ChunkedBuffer::addNewChunk()
{
    auto chunk = new DataChunk(_size, CHUNK_SIZE);
    chunks.append(chunk);
    return chunk;
}

size_t ChunkedBuffer::size() const
{
    return _size;
}

QRectF ChunkedBuffer::boundingRect() const
{
    // TODO: it should be possible to cache boundingRect and only
    // update on 'addSamples' and when dropping chunks

    // find ymin and ymax
    double ymin = chunks.first()->min();
    double ymax = chunks.first()->max();
    for (auto c : chunks)
    {
        ymin = std::min(ymin, c->min());
        ymax = std::max(ymax, c->max());
    }

    return QRectF(0, ymax, _size, (ymax-ymin));
}
