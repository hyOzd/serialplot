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

// IMPORTANT NOTE: this file will be renamed to "framebuffer.h" when
// stream work is complete

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

struct Range
{
    double min, max;
};

/// Abstract base class for all frame buffers.
class FrameBuffer
{
public:
    virtual unsigned size() const = 0;
    virtual double sample(unsigned i) const = 0;
    virtual Range limits() const = 0;
};

/// Common base class for index and writable frame buffers
class ResizableBuffer : public FrameBuffer
{
    /// Resize buffer
    virtual void resize(unsigned n) = 0;
};

/// Abstract base class for writable frame buffers
class WFrameBuffer : public ResizableBuffer
{
    /// Add samples to the buffer
    virtual void addSamples(double* samples, unsigned n) = 0;
    /// Reset all data to 0
    virtual void clear() = 0;
};

#endif // FRAMEBUFFER_H
