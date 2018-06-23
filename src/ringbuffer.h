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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "framebuffer.h"

/// A fast buffer implementation for storing data.
class RingBuffer : public WFrameBuffer
{
public:
    RingBuffer(unsigned n);
    ~RingBuffer();

    virtual unsigned size() const;
    virtual double sample(unsigned i) const;
    virtual Range limits() const;
    virtual void resize(unsigned n);
    virtual void addSamples(double* samples, unsigned n);
    virtual void clear();

private:
    unsigned _size;            ///< size of `data`
    double* data;              ///< storage
    unsigned headIndex;        ///< indicates the actual `0` index of the ring buffer

    mutable bool limInvalid;   ///< Indicates that limits needs to be re-calculated
    mutable Range limCache;    ///< Cache for limits()
    void updateLimits() const; ///< Updates limits cache
};

#endif
