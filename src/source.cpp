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

#include <QtGlobal>

#include "source.h"

Source::~Source()
{
    for (auto sink : sinks)
    {
        sink->setSource(NULL);
    }
}

void Source::connect(Sink* sink)
{
    Q_ASSERT(!sinks.contains(sink));
    Q_ASSERT(sink->connectedSource() == NULL);

    sinks.append(sink);
    sink->setSource(this);
    sink->setNumChannels(numChannels(), hasX());
}

void Source::disconnect(Sink* sink)
{
    Q_ASSERT(sinks.contains(sink));
    Q_ASSERT(sink->connectedSource() == this);

    sink->setSource(NULL);
    sinks.removeOne(sink);
}

void Source::disconnectSinks()
{
    while (!sinks.isEmpty())
    {
        auto sink = sinks.takeFirst();
        sink->setSource(NULL);
    }
}

void Source::feedOut(const SamplePack& data) const
{
    for (auto sink : sinks)
    {
        sink->feedIn(data);
    }
}

void Source::updateNumChannels() const
{
    for (auto sink : sinks)
    {
        sink->setNumChannels(numChannels(), hasX());
    }
}
