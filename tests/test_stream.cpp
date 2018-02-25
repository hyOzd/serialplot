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

#include "catch.hpp"
#include "test_helpers.h"

TEST_CASE("construction of stream with default values", "[memory, stream]")
{
    // default values are an empty stream with no channels
    Stream s;

    REQUIRE(s.numChannels() == 0);
    REQUIRE(!s.hasX());
    REQUIRE(s.numSamples() == 0);
}

TEST_CASE("construction of stream with parameters", "[memory, stream]")
{
    Stream s(4, true, 100);

    REQUIRE(s.numChannels() == 4);
    REQUIRE(s.hasX());
    REQUIRE(s.numSamples() == 100);

    for (unsigned i = 0; i < 4; i++)
    {
        const StreamChannel* c = s.channel(i);
        REQUIRE(c != NULL);
        REQUIRE(c->index() == i);
    }
}

TEST_CASE("changing stream number of channels via sink", "[memory, stream, sink]")
{
    Stream s;
    TestSource so(3, false);
    so.connect(&s);

    // nc=3, x= false
    REQUIRE(s.numChannels() == 3);
    REQUIRE(!s.hasX());
    for (unsigned i = 0; i < 3; i++)
    {
        const StreamChannel* c = s.channel(i);
        REQUIRE(c != NULL);
        REQUIRE(c->index() == i);
    }

    // increase nc value, add X
    so._setNumChannels(5, true);

    REQUIRE(s.numChannels() == 5);
    REQUIRE(s.hasX());

    for (unsigned i = 0; i < 5; i++)
    {
        const StreamChannel* c = s.channel(i);
        REQUIRE(c != NULL);
        REQUIRE(c->index() == i);
    }

    // reduce nc value, remove X
    so._setNumChannels(1, false);

    REQUIRE(s.numChannels() == 1);
    REQUIRE(!s.hasX());

    for (unsigned i = 0; i < 1; i++)
    {
        const StreamChannel* c = s.channel(i);
        REQUIRE(c != NULL);
        REQUIRE(c->index() == i);
    }
}

TEST_CASE("adding data to a stream with no X", "[memory, stream, data, sink]")
{
    Stream s(3, false, 10);

    // prepare data
    SamplePack pack(5, 3, false);
    for (unsigned ci = 0; ci < 3; ci++)
    {
        for (unsigned i = 0; i < 5; i++)
        {
            pack.data(ci)[i] = i;
        }
    }

    TestSource so(3, false);
    so.connect(&s);

    // test
    so._feed(pack);

    for (unsigned ci = 0; ci < 3; ci++)
    {
        const StreamChannel* c = s.channel(ci);
        const FrameBuffer* y = c->yData();

        for (unsigned i = 0; i < 5; i++)
        {
            REQUIRE(y->sample(i) == 0);
        }
        for (unsigned i = 5; i < 10; i++)
        {
            REQUIRE(y->sample(i) == i-5);
        }
    }
}

TEST_CASE("adding data to a stream with X", "[memory, stream, data, sink]")
{
    Stream s(3, true, 10);

    // prepare data
    SamplePack pack(5, 3, true);
    for (unsigned ci = 0; ci < 3; ci++)
    {
        for (unsigned i = 0; i < 5; i++)
        {
            pack.data(ci)[i] = i;
        }
    }
    // x data
    for (unsigned i = 0; i < 5; i++)
    {
        pack.xData()[i] = i+10;
    }

    TestSource so(3, true);
    so.connect(&s);

    // test
    so._feed(pack);

    for (unsigned ci = 0; ci < 3; ci++)
    {
        const StreamChannel* c = s.channel(ci);
        const FrameBuffer* y = c->yData();

        for (unsigned i = 0; i < 5; i++)
        {
            REQUIRE(y->sample(i) == 0);
        }
        for (unsigned i = 5; i < 10; i++)
        {
            REQUIRE(y->sample(i) == i-5);
        }
    }

    // check x
    const FrameBuffer* x = s.channel(0)->xData();
    for (unsigned i = 0; i < 5; i++)
    {
        REQUIRE(x->sample(i) == 0);
    }
    for (unsigned i = 5; i < 10; i++)
    {
        REQUIRE(x->sample(i) == (i-5)+10);
    }
}
