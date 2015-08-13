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

FrameBuffer::FrameBuffer(size_t size) :
    data(size, 0.)
{

}

void FrameBuffer::resize(size_t size)
{
    size_t old_size = this->size();
    size_t new_size = size;

    if (new_size < old_size)
    {
        data.remove(0, old_size - new_size);
    }
    else if (new_size > old_size)
    {
        // This is seriously inefficient!
        for (size_t i = old_size; i < new_size; i++)
        {
            data.prepend(0);
        }
    }
}

void FrameBuffer::addSamples(QVector<double> samples)
{
    int offset = size() - samples.size();

    if (offset < 0)
    {
        // new samples exceed the size of frame buffer
        // excess part (from beginning) of the input will be ignored
        for (unsigned int i = 0; i < size(); i++)
        {
            data[i] = samples[i - offset];
        }
    }
    else if (offset == 0) // input is the same size as the framebuffer
    {
        data = samples;
    }
    else // regular case; input is smaller than framebuffer
    {
        // shift old samples
        int shift = samples.size();
        for (int i = 0; i < offset; i++)
        {
            data[i] = data[i + shift];
        }
        // place new samples
        for (int i = 0; i < samples.size(); i++)
        {
            data[offset + i] = samples[i];
        }
    }
}

void FrameBuffer::clear()
{
    data.fill(0);
}

size_t FrameBuffer::size() const
{
    return (size_t) data.size();
}

QPointF FrameBuffer::sample(size_t i) const
{
    return QPointF(i, data[i]);
}

QRectF FrameBuffer::boundingRect() const
{
    return qwtBoundingRect(*this);
}
