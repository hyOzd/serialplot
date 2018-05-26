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

#ifndef SAMPLECOUNTER_H
#define SAMPLECOUNTER_H

#include <QObject>
#include "sink.h"

/// A `Sink` class for counting and reporting number of samples per second.
class SampleCounter : public QObject, public Sink
{
    Q_OBJECT

public:
    SampleCounter();

protected:
    // implementations for `Sink`
    virtual void feedIn(const SamplePack& data);

signals:
    /// Emitted per second if SPS value has changed.
    void spsChanged(float value);

private:
    qint64 prevTimeMs;
    unsigned count;
};

#endif // SAMPLECOUNTER_H
