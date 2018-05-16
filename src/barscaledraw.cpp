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

#include "barscaledraw.h"

#include <QtDebug>

BarScaleDraw::BarScaleDraw(const Stream* stream)
{
    _stream = stream;
    enableComponent(Backbone, false);
    setLabelRotation(-90);
    setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QObject::connect(_stream, &Stream::channelNameChanged,
            [this]()
            {
                invalidateCache();
            });
}

QwtText BarScaleDraw::label(double value) const
{
    int index = value;
    unsigned numChannels = _stream->numChannels();

    if (index >=0 && index < (int) numChannels)
    {
        return _stream->channel(index)->name();
    }
    else
    {
        return QString("");
    }
}
