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

#include <QtGlobal>

#include "indexbuffer.h"

IndexBuffer::IndexBuffer(unsigned n)
{
    _size = n;
}

unsigned IndexBuffer::size() const
{
    return _size;
}

void IndexBuffer::resize(unsigned n)
{
    _size = n;
}

double IndexBuffer::sample(unsigned i) const
{
    Q_ASSERT(i < _size);

    return i;
}

Range IndexBuffer::limits() const
{
    return Range{0, _size-1.};
}
