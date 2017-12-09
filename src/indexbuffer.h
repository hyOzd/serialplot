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

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

// IMPORTANT TODO: rename to "framebuffer.h" when stream work is done.
#include "framebuffer2.h"

/// A simple frame buffer that simply returns requested index as
/// value.
class IndexBuffer : public ResizableBuffer
{
public:
    IndexBuffer(unsigned n);

    unsigned size() const;
    double sample(unsigned i) const;
    Range limits() const;
    void resize(unsigned n);

private:
    unsigned _size;
};

#endif
