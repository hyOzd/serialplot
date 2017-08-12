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

#include "datachunk.h"
#include "chunkedbuffer.h"

TEST_CASE("DataChunk created empty", "[memory]")
{
    DataChunk c(0, 1000);
    REQUIRE(c.size() == 0);
    REQUIRE(c.capacity() == 1000);
    REQUIRE(c.start() == 0);
    REQUIRE(c.end() == 0);
    REQUIRE_FALSE(c.isFull());
    REQUIRE(c.left() == 1000);
}

TEST_CASE("adding data to DataChunk", "[memory]")
{
    DataChunk c(0, 1000);
    double samples[10] = {1,2,3,4,5,6,7,8,9,10};
    c.addSamples(samples, 10);

    REQUIRE(c.size() == 10);
    REQUIRE(c.capacity() == 1000);
    REQUIRE(c.start() == 0);
    REQUIRE(c.end() == 10);
    REQUIRE(c.left() == 990);

    for (int i = 0; i < 10; i++)
    {
        REQUIRE(c.sample(i) == samples[i]);
    }

    REQUIRE(c.min() == 1);
    REQUIRE(c.max() == 10);
    REQUIRE(c.avg() == Approx(5.5));
    REQUIRE(c.meanSquare() == Approx(38.5));
}

TEST_CASE("filling data chunk", "[memory]")
{
    DataChunk c(0, 1000);

    for (int i = 0; i < 1000; i++)
    {
        double sample = i + 1;
        c.addSamples(&sample, 1);
    }

    REQUIRE(c.size() == 1000);
    REQUIRE(c.capacity() == 1000);
    REQUIRE(c.start() == 0);
    REQUIRE(c.end() == 1000);
    REQUIRE(c.left() == 0);

    REQUIRE(c.min() == 1);
    REQUIRE(c.max() == 1000);
    REQUIRE(c.avg() == Approx(500.5));
    REQUIRE(c.meanSquare() == Approx(333833.5));
}

TEST_CASE("ChunkedBuffer created empty", "[memory]")
{
    ChunkedBuffer b;

    REQUIRE(b.size() == 0);
    REQUIRE(b.boundingRect() == QRectF(0,0,0,0));
}

TEST_CASE("ChunkedBuffer adding data and clearing", "[memory]")
{
    ChunkedBuffer b;

    // add some small data
    const int N = 10;
    double samples[N] = {1,2,3,4,5,6,7,8,9,10};
    b.addSamples(samples, N);

    REQUIRE(b.size() == N);
    REQUIRE(b.boundingRect() == QRectF(0,10,N,9));

    // add data to fill the chunk
    double samples2[CHUNK_SIZE-N] = {0};
    b.addSamples(samples2, CHUNK_SIZE-N);
    REQUIRE(b.size() == CHUNK_SIZE);
    REQUIRE(b.boundingRect() == QRectF(0,10,CHUNK_SIZE,10));

    // add data for second chunk
    b.addSamples(samples, N);
    REQUIRE(b.size() == CHUNK_SIZE+N);
    REQUIRE(b.boundingRect() == QRectF(0,10,CHUNK_SIZE+N,10));

    // add more data to make it 4 chunks
    double samples3[CHUNK_SIZE*3-N] = {0};
    b.addSamples(samples3, CHUNK_SIZE*3-N);
    REQUIRE(b.size() == CHUNK_SIZE*4);
    REQUIRE(b.boundingRect() == QRectF(0,10,CHUNK_SIZE*4,10));

    // TODO: clear
    // b.clear();
    // REQUIRE(b.size() == 0);
}

TEST_CASE("ChunkedBuffer accessing data", "[memory]")
{
    ChunkedBuffer b;

    // prepare data
    double samples[CHUNK_SIZE*3];
    samples[0] = 10;
    samples[10] = 20;
    samples[CHUNK_SIZE-1] = 30;
    samples[CHUNK_SIZE] = 40;
    samples[CHUNK_SIZE+1] = 50;
    samples[CHUNK_SIZE*2-1] = 60;
    samples[CHUNK_SIZE*3-1] = 70;

    // test
    b.addSamples(samples, CHUNK_SIZE*3);

    REQUIRE(b.size() == CHUNK_SIZE*3);
    REQUIRE(b.sample(0) == 10);
    REQUIRE(b.sample(10) == 20);
    REQUIRE(b.sample(CHUNK_SIZE-1) == 30);
    REQUIRE(b.sample(CHUNK_SIZE) == 40);
    REQUIRE(b.sample(CHUNK_SIZE+1) == 50);
    REQUIRE(b.sample(CHUNK_SIZE*2-1) == 60);
    REQUIRE(b.sample(CHUNK_SIZE*3-1) == 70);
}
