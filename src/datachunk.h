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

#ifndef DATACHUNK_H
#define DATACHUNK_H

class DataChunk
{
public:
    DataChunk(size_t start, size_t capacity);
    ~DataChunk();

    size_t start() const;       // index of first element
    size_t end() const;         // index after last element
    size_t size() const;        // fill size
    size_t capacity() const;    // set capacity
    bool isFull() const;
    size_t left() const;

    double min() const;
    double max() const;
    double avg() const;
    double meanSquare() const;

    void addSamples(double* samples, size_t size);
    double sample(size_t i) const;

private:
    size_t _start;
    size_t _capacity;
    size_t _size;

    double _min;
    double _max;
    double _sum;
    double _sumSquare;          // sum of squares

    double* _samples;
};

#endif // DATACHUNK_H
