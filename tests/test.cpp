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

TEST_CASE("IndexBuffer", "[memory, buffer]")
{
    IndexBuffer buf(10);

    REQUIRE(buf.size() == 10);
    for (unsigned i = 0; i < 10; i++)
    {
        REQUIRE(buf.sample(i) == i);
    }
    auto l = buf.limits();
    REQUIRE(l.min == 0);
    REQUIRE(l.max == 9);

    buf.resize(20);
    REQUIRE(buf.size() == 20);
    REQUIRE(buf.sample(15) == 15);
    l = buf.limits();
    REQUIRE(l.min == 0);
    REQUIRE(l.max == 19);
}
