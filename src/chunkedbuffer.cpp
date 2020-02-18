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

// TODO: we should have a max size for chunkedbuffer
void ChunkedBuffer::addSamples(double* samples, unsigned n)
{
    size_t i = 0;

    while (i < n)
    {
        // select chunk to add data
        auto chunk = chunks.last();
        if (chunk->isFull())
        {
            chunk = addNewChunk(); // create a new chunk
        }

        // add data to chunk
        size_t c = std::min(chunk->left(), (n - i));
        chunk->addSamples(&samples[i], c);
        i += c;
    }

    _size += n;
}

void ChunkedBuffer::clear()
{
    // delete all chunks
    for (auto chunk : chunks)
    {
        delete chunk;
    }
    chunks.clear();

    numChunks = 0;
    _size = 0;

    // create first chunk
    addNewChunk();
}

DataChunk* ChunkedBuffer::addNewChunk()
{
    auto chunk = new DataChunk(_size, CHUNK_SIZE);
    chunks.append(chunk);
    return chunk;
}

unsigned ChunkedBuffer::size() const
{
    return _size;
}

Range ChunkedBuffer::limits() const
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

    return {ymin, ymax};
}

double ChunkedBuffer::sample(unsigned i) const
{
    Q_ASSERT(i < _size);

    int chunk_index = i / CHUNK_SIZE;
    int chunk_offset = i % CHUNK_SIZE;
    return chunks[chunk_index]->sample(chunk_offset);
}

void ChunkedBuffer::resize(unsigned n)
{
    // TODO what to do for ChunkedBuffer::resize
}
