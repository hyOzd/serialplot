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

#ifndef SAMPLEPACK_H
#define SAMPLEPACK_H

class SamplePack
{
public:
    SamplePack(unsigned ns, unsigned nc, bool x = false);
    ~SamplePack();

    bool hasX() const;
    unsigned numChannels() const;
    unsigned numSamples() const;
    double* xData() const;
    double* data(unsigned channel) const;

private:
    unsigned _numSamples, _numChannels;
    double* _xData;
    double* _yData;
};

#endif // SAMPLEPACK_H
