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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "samplepack.h"
#include "source.h"
#include "indexbuffer.h"
#include "linindexbuffer.h"
#include "ringbuffer.h"
#include "readonlybuffer.h"

TEST_CASE("samplepack with no X", "[memory]")
{
    SamplePack pack(100, 3, false);

    REQUIRE_FALSE(pack.hasX());
    REQUIRE(pack.numChannels() == 3);
    REQUIRE(pack.numSamples() == 100);

    double* chan0 = pack.data(0);
    double* chan1 = pack.data(1);
    double* chan2 = pack.data(2);

    REQUIRE(chan0 == chan1 - 100);
    REQUIRE(chan1 == chan2 - 100);
}

TEST_CASE("samplepack with X", "[memory]")
{
    SamplePack pack(100, 3, true);

    REQUIRE(pack.hasX());
    REQUIRE(pack.numChannels() == 3);
    REQUIRE(pack.numSamples() == 100);
    REQUIRE(pack.xData() != nullptr);
}

class TestSink : public Sink
{
public:
    int totalFed;
    int _numChannels;
    bool _hasX;

    TestSink()
        {
            totalFed = 0;
            _numChannels = 0;
            _hasX = false;
        };

    void feedIn(const SamplePack& data)
        {
            REQUIRE(data.numChannels() == numChannels());

            totalFed += data.numSamples();

            Sink::feedIn(data);
        };

    void setNumChannels(unsigned nc, bool x)
        {
            _numChannels = nc;
            _hasX = x;

            Sink::setNumChannels(nc, x);
        };

    virtual unsigned numChannels() const
        {
            return _numChannels;
        };

    virtual bool hasX() const
        {
            return _hasX;
        };
};

TEST_CASE("sink", "[memory, stream]")
{
    TestSink sink;
    SamplePack pack(100, 3, false);

    sink.setNumChannels(3, false);
    REQUIRE(sink.numChannels() == 3);

    sink.feedIn(pack);
    REQUIRE(sink.totalFed == 100);
    sink.feedIn(pack);
    REQUIRE(sink.totalFed == 200);

    TestSink follower;

    sink.connectFollower(&follower);
    REQUIRE(follower.numChannels() == 3);
    REQUIRE(follower.hasX() == false);

    sink.feedIn(pack);
    REQUIRE(sink.totalFed == 300);
    REQUIRE(follower.totalFed == 100);

    sink.setNumChannels(2, true);
    REQUIRE(follower.numChannels() == 2);
    REQUIRE(follower.hasX() == true);
}

TEST_CASE("sink must be created unconnected", "[memory, stream]")
{
    TestSink sink;
    REQUIRE(sink.connectedSource() == NULL);
}

class TestSource : public Source
{
public:
    int _numChannels;
    bool _hasX;

    TestSource(unsigned nc, bool x)
        {
            _numChannels = nc;
            _hasX = x;
        };

    virtual unsigned numChannels() const
        {
            return _numChannels;
        };

    virtual bool hasX() const
        {
            return _hasX;
        };

    void _feed(const SamplePack& data) const
        {
            feedOut(data);
        };

    void _setNumChannels(unsigned nc, bool x)
        {
            _numChannels = nc;
            _hasX = x;

            updateNumChannels();
        };
};

TEST_CASE("source", "[memory, stream]")
{
    TestSink sink;

    TestSource source(3, false);

    REQUIRE(source.numChannels() == 3);
    REQUIRE(source.hasX() == false);

    source.connect(&sink);
    REQUIRE(sink.numChannels() == 3);
    REQUIRE(sink.hasX() == false);

    source._setNumChannels(5, true);
    REQUIRE(sink.numChannels() == 5);
    REQUIRE(sink.hasX() == true);

    SamplePack pack(100, 5, true);
    source._feed(pack);
    REQUIRE(sink.totalFed == 100);

    source.disconnect(&sink);
    source._feed(pack);
    REQUIRE(sink.totalFed == 100);
}

TEST_CASE("source must set/unset sink 'source'", "[memory, stream]")
{
    TestSink sink;
    TestSource source(3, false);

    source.connect(&sink);
    REQUIRE(sink.connectedSource() == &source);

    source.disconnect(&sink);
    REQUIRE(sink.connectedSource() == NULL);
}

TEST_CASE("IndexBuffer", "[memory, buffer]")
{
    IndexBuffer buf(10);

    REQUIRE(buf.size() == 10);
    for (unsigned i = 0; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == i);
    }
    auto l = buf.limits();
    REQUIRE(l.start == 0);
    REQUIRE(l.end == 9);

    buf.resize(20);
    REQUIRE(buf.size() == 20);
    REQUIRE(buf.sample(15) == 15);
    l = buf.limits();
    REQUIRE(l.start == 0);
    REQUIRE(l.end == 19);
}

TEST_CASE("LinIndexBuffer", "[memory, buffer]")
{
    LinIndexBuffer buf(10, 0., 3.0);

    REQUIRE(buf.size() == 10);
    REQUIRE(buf.sample(0) == 0.);
    REQUIRE(buf.sample(9) == 3.0);
    REQUIRE(buf.sample(4) == Approx(1+1/3.));

    auto l = buf.limits();
    REQUIRE(l.start == 0.);
    REQUIRE(l.end == 3.);

    buf.resize(20);
    REQUIRE(buf.size() == 20);
    REQUIRE(buf.sample(0) == 0.);
    REQUIRE(buf.sample(9) == Approx(9.*3./19.));
    REQUIRE(buf.sample(4) == Approx(4.*3./19.));
    REQUIRE(buf.sample(19) == 3.0);

    l = buf.limits();
    REQUIRE(l.start == 0.);
    REQUIRE(l.end == 3.0);

    buf.setLimits({-5., 5.});
    l = buf.limits();
    REQUIRE(l.start == -5.0);
    REQUIRE(l.end == 5.0);

    REQUIRE(buf.sample(0) == -5.0);
    REQUIRE(buf.sample(19) == 5.0);
}

TEST_CASE("RingBuffer sizing", "[memory, buffer]")
{
    RingBuffer buf(10);

    REQUIRE(buf.size() == 10);

    buf.resize(5);
    REQUIRE(buf.size() == 5);

    buf.resize(15);
    REQUIRE(buf.size() == 15);
}

TEST_CASE("RingBuffer initial values should be 0", "[memory, buffer]")
{
    RingBuffer buf(10);

    for (unsigned i = 0; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == 0.);
    }
}

TEST_CASE("RingBuffer data access", "[memory, buffer]")
{
    RingBuffer buf(10);
    double values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    buf.addSamples(values, 10);

    REQUIRE(buf.size() == 10);
    for (unsigned i = 0; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == values[i]);
    }

    buf.addSamples(values, 5);

    REQUIRE(buf.size() == 10);
    for (unsigned i = 0; i < 5; i++)
    {
        REQUIRE(buf.sample(i) == values[i+5]);
    }
    for (unsigned i = 5; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == values[i-5]);
    }
}

TEST_CASE("making RingBuffer bigger should keep end values", "[memory, buffer]")
{
    RingBuffer buf(5);
    double values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    buf.addSamples(values, 5);
    buf.resize(10);

    REQUIRE(buf.size() == 10);
    for (unsigned i = 0; i < 5; i++)
    {
        REQUIRE(buf.sample(i) == 0);
    }
    for (unsigned i = 5; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == values[i-5]);
    }
}

TEST_CASE("making RingBuffer smaller should keep end values", "[memory, buffer]")
{
    RingBuffer buf(10);
    double values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    buf.addSamples(values, 10);
    buf.resize(5);

    REQUIRE(buf.size() == 5);
    for (unsigned i = 0; i < 5; i++)
    {
        REQUIRE(buf.sample(i) == values[i+5]);
    }
}

TEST_CASE("RingBuffer limits", "[memory, buffer]")
{
    RingBuffer buf(10);
    double values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto lim = buf.limits();
    REQUIRE(lim.start == 0.);
    REQUIRE(lim.end == 0.);

    buf.addSamples(values, 10);
    lim = buf.limits();
    REQUIRE(lim.start == 1.);
    REQUIRE(lim.end == 10.);

    buf.addSamples(&values[9], 1);
    lim = buf.limits();
    REQUIRE(lim.start == 2.);
    REQUIRE(lim.end == 10.);

    buf.addSamples(values, 9);
    buf.addSamples(values, 1);
    lim = buf.limits();
    REQUIRE(lim.start == 1.);
    REQUIRE(lim.end == 9.);
}

TEST_CASE("RingBuffer clear", "[memory, buffer]")
{
    RingBuffer buf(10);
    double values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    buf.addSamples(values, 10);
    buf.clear();

    REQUIRE(buf.size() == 10);
    for (unsigned i = 0; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == 0.);
    }
    auto lim = buf.limits();
    REQUIRE(lim.start == 0.);
    REQUIRE(lim.end == 0.);
}

TEST_CASE("ReadOnlyBuffer", "[memory, buffer]")
{
    IndexBuffer source(10);

    ReadOnlyBuffer buf(&source);

    REQUIRE(buf.size() == 10);
    auto lim = buf.limits();
    REQUIRE(lim.start == 0.);
    REQUIRE(lim.end == 9.);
    for (unsigned i = 0; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == i);
    }
}

TEST_CASE("ReadOnlyBuffer sliced constructor", "[memory, buffer]")
{
    IndexBuffer source(10);

    ReadOnlyBuffer buf(&source, 5, 4);

    REQUIRE(buf.size() == 4);
    auto lim = buf.limits();
    REQUIRE(lim.start == 5.);
    REQUIRE(lim.end == 8.);
    for (unsigned i = 0; i < 4; i++)
    {
        REQUIRE(buf.sample(i) == (i + 5));
    }
}
