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

#ifndef SINK_H
#define SINK_H

#include <QList>
#include "samplepack.h"

class Source;

class Sink
{
public:
    /// Connects a sink to get any data that this sink
    /// gets. Connecting an already connected sink is an error.
    void connectFollower(Sink* sink);
    /// Disconnects a follower. Disconnecting an unconnected sink is
    /// an error.
    void disconnectFollower(Sink* sink);

protected:
    /// Entry point for incoming data. Re-implementations should
    /// call this function to feed followers.
    virtual void feedIn(const SamplePack& data);
    /// Is set by connected source. Re-implementations should call
    /// this function to update followers.
    virtual void setNumChannels(unsigned nc, bool x);
    /// Returns true if sink has X data
    virtual bool hasX() const = 0;
    /// Returns number of channels
    virtual unsigned numChannels() const = 0;

    friend Source;

private:
    QList<Sink*> followers;
};

#endif // SINK_H
