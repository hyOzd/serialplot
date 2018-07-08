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

#include "stream.h"
#include "ringbuffer.h"
#include "indexbuffer.h"

Stream::Stream(unsigned nc, bool x, unsigned ns) :
    _infoModel(nc)
{
    _numSamples = ns;
    _paused = false;

    // create xdata buffer
    _hasx = x;
    if (x)
    {
        xData = new RingBuffer(ns);
    }
    else
    {
        xData = new IndexBuffer(ns);
    }

    // create channels
    for (unsigned i = 0; i < nc; i++)
    {
        auto c = new StreamChannel(i, xData, new RingBuffer(ns), &_infoModel);
        channels.append(c);
    }
}

Stream::~Stream()
{
    for (auto ch : channels)
    {
        delete ch;
    }
    delete xData;
}

bool Stream::hasX() const
{
    return _hasx;
}

unsigned Stream::numChannels() const
{
    return channels.length();
}

unsigned Stream::numSamples() const
{
    return _numSamples;
}

const StreamChannel* Stream::channel(unsigned index) const
{
    Q_ASSERT(index < numChannels());
    return channels[index];
}

StreamChannel* Stream::channel(unsigned index)
{
    return const_cast<StreamChannel*>(static_cast<const Stream&>(*this).channel(index));
}

const ChannelInfoModel* Stream::infoModel() const
{
    return &_infoModel;
}

ChannelInfoModel* Stream::infoModel()
{
    return const_cast<ChannelInfoModel*>(static_cast<const Stream&>(*this).infoModel());
}

void Stream::setNumChannels(unsigned nc, bool x)
{
    unsigned oldNum = numChannels();
    if (oldNum == nc && x == _hasx) return;

    // adjust the number of channels
    if (nc > oldNum)
    {
        for (unsigned i = oldNum; i < nc; i++)
        {
            auto c = new StreamChannel(i, xData, new RingBuffer(_numSamples), &_infoModel);
            channels.append(c);
        }
    }
    else if (nc < oldNum)
    {
        for (unsigned i = oldNum-1; i > nc-1; i--)
        {
            delete channels.takeLast();
        }
    }

    // change the xdata
    if (x != _hasx)
    {
        if (x)
        {
            xData = new RingBuffer(_numSamples);
        }
        else
        {
            xData = new IndexBuffer(_numSamples);
        }

        for (auto c : channels)
        {
            c->setX(xData);
        }
        _hasx = x;
    }

    if (nc != oldNum)
    {
        _infoModel.setNumOfChannels(nc);
        // TODO: how about X change?
        emit numChannelsChanged(nc);
    }

    Sink::setNumChannels(nc, x);
}

void Stream::feedIn(const SamplePack& data)
{
    Q_ASSERT(data.numChannels() == numChannels() &&
             data.hasX() == hasX());

    if (_paused) return;

    unsigned ns = data.numSamples();
    if (_hasx)
    {
        static_cast<RingBuffer*>(xData)->addSamples(data.xData(), ns);
    }
    for (unsigned ci = 0; ci < numChannels(); ci++)
    {
        // TODO: check for gainEn and offsetEn
        // apply gain and offset
        auto rdata = data.data(ci);
        auto mdata = new double[sizeof(double) * ns];
        // TODO: add gain&offset access methods to `StreamChannel`
        double gain = channels[ci]->info()->gain(ci);
        double offset = channels[ci]->info()->offset(ci);
        for (int i = 0; i < ns; i++)
        {
            mdata[i] = rdata[i] * gain + offset;
        }

        static_cast<RingBuffer*>(channels[ci]->yData())->addSamples(mdata, ns);
        delete[] mdata;
    }

    // TODO: emit modified (gain&offset) data
    Sink::feedIn(data);

    emit dataAdded();
}

void Stream::pause(bool paused)
{
    _paused = paused;
}

void Stream::clear()
{
    for (auto c : channels)
    {
        static_cast<RingBuffer*>(c->yData())->clear();
    }
}

void Stream::setNumSamples(unsigned value)
{
    if (value == _numSamples) return;
    _numSamples = value;

    xData->resize(value);
    for (auto c : channels)
    {
        static_cast<RingBuffer*>(c->yData())->resize(value);
    }
}

void Stream::saveSettings(QSettings* settings) const
{
    _infoModel.saveSettings(settings);
}

void Stream::loadSettings(QSettings* settings)
{
    _infoModel.loadSettings(settings);
}
