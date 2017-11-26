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

#include "samplepack.h"

SamplePack::SamplePack(unsigned ns, unsigned nc, bool x)
{
    Q_ASSERT(ns > 0 && nc > 0);

    _numSamples = ns;
    _numChannels = nc;

    _yData = new double[_numSamples * _numChannels]();
    if (x)
    {
        _xData = new double[_numSamples]();
    }
    else
    {
        _xData = nullptr;
    }
}

SamplePack::~SamplePack()
{
    delete[] _yData;
    if (_xData != nullptr)
    {
        delete[] _xData;
    }
}

bool SamplePack::hasX() const
{
    return _xData != nullptr;
}

unsigned SamplePack::numChannels() const
{
    return _numChannels;
}

unsigned SamplePack::numSamples() const
{
    return _numSamples;
}

double* SamplePack::xData() const
{
    Q_ASSERT(_xData != nullptr);

    return _xData;
}

double* SamplePack::data(unsigned channel) const
{
    Q_ASSERT(channel < _numChannels);

    return &_yData[channel * _numSamples];
}
