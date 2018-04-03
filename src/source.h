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

#ifndef SOURCE_H
#define SOURCE_H

#include <QList>

#include "sink.h"
#include "samplepack.h"

class Source
{
public:
    /// Virtual destructor. Must be called by implementors to notify sinks.
    virtual ~Source();

    /// Returns true if source has X data
    virtual bool hasX() const = 0;

    /// Returns number of channels
    virtual unsigned numChannels() const = 0;

    /// Connects a sink to this source. Trying to connect an already
    /// connected sink is an error.
    void connect(Sink* sink);

    /// Disconnects an already connected sink. Trying to disconnect an
    /// unconnected sink is an error.
    void disconnect(Sink* sink);

    /// Disconnects all connected sinks.
    void disconnectSinks();

protected:
    /// Feeds "in" given data to connected sinks
    virtual void feedOut(const SamplePack& data) const;

    /// Updates "number of channels" of connected sinks. Must be
    /// called when num. channels or hasX changes.
    void updateNumChannels() const;

private:
    QList<Sink*> sinks;
};

#endif // SOURCE_H
