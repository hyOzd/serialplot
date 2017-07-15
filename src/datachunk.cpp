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

#include "datachunk.h"

DataChunk::DataChunk(size_t start, size_t capacity)
{
    _start = start;
    _capacity = capacity;
    _size = 0;
    _samples = new double[capacity];

    _min = 0;
    _max = 0;
    _sum = 0;
    _sumSquare = 0;
}

DataChunk::~DataChunk()
{
    delete[] _samples;
}

size_t DataChunk::start() const
{
    return _start;
}

size_t DataChunk::end() const
{
    return _start + _size;
}

bool DataChunk::isFull() const
{
    return left() == 0;
}

size_t DataChunk::size() const
{
    return _size;
}

size_t DataChunk::capacity() const
{
    return _capacity;
}

size_t DataChunk::left() const
{
    return _capacity - _size;
}

double DataChunk::min() const
{
    return _min;
}

double DataChunk::max() const
{
    return _max;
}

double DataChunk::avg() const
{
    return _sum / _size;
}

double DataChunk::meanSquare() const
{
    return _sumSquare / _size;
}

double DataChunk::sample(size_t i) const
{
    Q_ASSERT(i <= _size);
    return _samples[i];
}

void DataChunk::addSamples(double* samples, size_t size)
{
    Q_ASSERT(size > 0 && size <= left());

    // start min&max values from first sample
    if (_size == 0)
    {
        _min = _max = samples[0];
    }

    for (unsigned i = 0; i < size; i++)
    {
        double newSample = samples[i];

        _samples[this->_size + i] = newSample;

        // update min/max and measurements
        if (newSample < _min)
        {
            _min = newSample;
        }
        else if (newSample > _max)
        {
            _max = newSample;
        }
        _sum += newSample;
        _sumSquare += newSample * newSample;
    }

    this->_size += size;

    Q_ASSERT(this->_size <= this->_capacity);
}
