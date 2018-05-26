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

#include <QDateTime>
#include "samplecounter.h"

SampleCounter::SampleCounter()
{
    prevTimeMs = QDateTime::currentMSecsSinceEpoch();
    count = 0;
}

#include <QtDebug>

void SampleCounter::feedIn(const SamplePack& data)
{
    count += data.numSamples();

    qint64 current = QDateTime::currentMSecsSinceEpoch();
    auto diff = current - prevTimeMs;
    if (diff > 1000) // 1sec
    {
        emit spsChanged(1000 * float(count) / diff);

        prevTimeMs = current;
        count = 0;
    }
}
