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

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "framebuffer.h"

/// A simple frame buffer that simply returns requested index as
/// sample value.
///
/// @note This buffer isn't for storing data.
class IndexBuffer : public XFrameBuffer
{
public:
    IndexBuffer(unsigned n);

    unsigned size() const override;
    double sample(unsigned i) const override;
    Range limits() const override;
    void resize(unsigned n) override;
    int findIndex(double value) const override;

private:
    unsigned _size;
};

#endif
