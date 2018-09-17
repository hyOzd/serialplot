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

#ifndef READONLYBUFFER_H
#define READONLYBUFFER_H

#include "framebuffer.h"

/// A read only frame buffer used for storing snapshot data. Main advantage of
/// this compared to `RingBuffer` is that reading data should be somewhat
/// faster.
class ReadOnlyBuffer : public FrameBuffer
{
public:
    /// Creates a buffer with data copied from `source`. Source buffer cannot be
    /// empty.
    ReadOnlyBuffer(const FrameBuffer* source);

    /// Creates a buffer from a slice of the `source`.
    ///
    /// @param start start of the slice
    /// @param n number of samples
    ///
    /// @important (start + n) should be smaller or equal than `source->size()`,
    /// otherwise it's an error.
    ReadOnlyBuffer(const FrameBuffer* source, unsigned start, unsigned n);

    /// Creates a buffer with data copied from an array
    ReadOnlyBuffer(const double* source, unsigned ssize);

    ~ReadOnlyBuffer();

    virtual unsigned size() const;
    virtual double sample(unsigned i) const;
    virtual Range limits() const;

private:
    double* data;    ///< data storage
    unsigned _size;  ///< data size
    Range _limits;   ///< limits cache

    // TODO: duplicate with `RingBuffer`
    void updateLimits(); ///< Updates limits cache
};

#endif // READONLYBUFFER_H
