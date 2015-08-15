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

#include "framebuffer.h"

FrameBuffer::FrameBuffer(size_t size)
{
    _size = size;
    data = new double[_size]();
    headIndex = 0;
}

FrameBuffer::~FrameBuffer()
{
    delete data;
}

void FrameBuffer::resize(size_t size)
{
    int offset = size - _size;
    if (offset == 0) return;

    double* newData = new double[size];

    // move data to new array
    int fill_start = offset > 0 ? offset : 0;

    for (int i = fill_start; i < int(size); i++)
    {
        newData[i] = _sample(i - offset);
    }

    // fill the beginning of the new data
    if (fill_start > 0)
    {
        for (int i = 0; i < fill_start; i++)
        {
            newData[i] = 0;
        }
    }

    // data is ready, clean and re-point
    delete data;
    data = newData;
    headIndex = 0;
    _size = size;
}

void FrameBuffer::addSamples(double* samples, size_t size)
{
    unsigned shift = size;
    if (shift < _size)
    {
        unsigned x = _size - headIndex; // distance of `head` to end

        if (shift <= x) // there is enough room at the end of array
        {
            for (size_t i = 0; i < shift; i++)
            {
                data[i+headIndex] = samples[i];
            }

            if (shift == x) // we used all the room at the end
            {
                headIndex = 0;
            }
            else
            {
                headIndex += shift;
            }
        }
        else // there isn't enough room
        {
            for (size_t i = 0; i < x; i++) // fill the end part
            {
                data[i+headIndex] = samples[i];
            }
            for (size_t i = 0; i < (shift-x); i++) // continue from the beginning
            {
                data[i] = samples[i+x];
            }
            headIndex = shift-x;
        }
    }
    else // number of new samples equal or bigger than current size
    {
        int x = shift - _size;
        for (size_t i = 0; i < _size; i++)
        {
            data[i] = samples[i+x];
        }
        headIndex = 0;
    }
}

void FrameBuffer::clear()
{
    for (size_t i=0; i < _size; i++) data[i] = 0.;
}

size_t FrameBuffer::size() const
{
    return _size;
}

QPointF FrameBuffer::sample(size_t i) const
{
    return QPointF(i, _sample(i));
}

QRectF FrameBuffer::boundingRect() const
{
    return qwtBoundingRect(*this);
}

double FrameBuffer::_sample(size_t i) const
{
    size_t index = headIndex + i;
    if (index >= _size) index -= _size;
    return data[index];
}
