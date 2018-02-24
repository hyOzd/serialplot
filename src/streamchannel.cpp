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

#include "streamchannel.h"

StreamChannel::StreamChannel(unsigned i, const FrameBuffer* x,
              FrameBuffer* y, ChannelInfoModel* info)
{
    _index = i;
    _x = x;
    _y = y;
    _info = info;
}

StreamChannel::~StreamChannel()
{
    delete _y;
}

unsigned StreamChannel::index() const {return _index;}
QString StreamChannel::name() const {return _info->name(_index);};
QColor StreamChannel::color() const {return _info->color(_index);};
bool StreamChannel::visible() const {return _info->isVisible(_index);};
const FrameBuffer* StreamChannel::xData() const {return _x;}
const FrameBuffer* StreamChannel::yData() const {return _y;}
FrameBuffer* StreamChannel::yData() {return _y;}
const ChannelInfoModel* StreamChannel::info() const {return _info;}
void StreamChannel::setX(const FrameBuffer* x) {_x = x;};
