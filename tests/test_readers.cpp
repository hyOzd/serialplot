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

// This tells Catch to provide a main() - only do this in one cpp file per executable
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <QSignalSpy>
#include <QBuffer>
#include "binarystreamreader.h"
#include "asciireader.h"
#include "framedreader.h"
#include "demoreader.h"

#include "test_helpers.h"

static const int READYREAD_TIMEOUT = 10; // milliseconds

TEST_CASE("reading data with BinaryStreamReader", "[reader]")
{
    QBuffer bufferDev;
    BinaryStreamReader bs(&bufferDev);
    bs.enable(true);

    TestSink sink;
    bs.connectSink(&sink);

    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);

    bufferDev.open(QIODevice::ReadWrite);
    const char data[] = {0x01, 0x02, 0x03, 0x04};
    bufferDev.write(data, 4);
    bufferDev.seek(0);

    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE(spy.wait(READYREAD_TIMEOUT));
    REQUIRE(sink.totalFed == 4);
}

TEST_CASE("disabled BinaryStreamReader shouldn't read", "[reader]")
{
    QBuffer bufferDev;
    BinaryStreamReader bs(&bufferDev); // disabled by default

    TestSink sink;
    bs.connectSink(&sink);

    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);

    bufferDev.open(QIODevice::ReadWrite);
    const char data[] = {0x01, 0x02, 0x03, 0x04};
    bufferDev.write(data, 4);
    bufferDev.seek(0);

    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    // readyRead isn't signaled because there are no connections to it
    REQUIRE_FALSE(spy.wait(READYREAD_TIMEOUT));
    REQUIRE(sink.totalFed == 0);
}

TEST_CASE("reading data with AsciiReader", "[reader, ascii]")
{
    QBuffer bufferDev;
    AsciiReader reader(&bufferDev);
    reader.enable(true);

    TestSink sink;
    reader.connectSink(&sink);

    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);

    // inject data to the buffer
    bufferDev.open(QIODevice::ReadWrite);
    bufferDev.write("0,1,3\n0,1,3\n0,1,3\n0,1,3\n");
    bufferDev.seek(0);

    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE(spy.wait(READYREAD_TIMEOUT));
    REQUIRE(sink._numChannels == 3);
    REQUIRE(sink._hasX == false);
    REQUIRE(sink.totalFed == 3);
}

TEST_CASE("AsciiReader shouldn't read when disabled", "[reader, ascii]")
{
    QBuffer bufferDev;
    AsciiReader reader(&bufferDev); // disabled by default

    TestSink sink;
    reader.connectSink(&sink);

    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);

    // inject data to the buffer
    bufferDev.open(QIODevice::ReadWrite);
    bufferDev.write("0,1,3\n0,1,3\n0,1,3\n0,1,3\n");
    bufferDev.seek(0);

    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE_FALSE(spy.wait(READYREAD_TIMEOUT));
    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);
    REQUIRE(sink.totalFed == 0);
}

TEST_CASE("reading data with FramedReader", "[reader]")
{
    QBuffer bufferDev;
    FramedReader reader(&bufferDev);
    reader.enable(true);

    TestSink sink;
    reader.connectSink(&sink);

    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);

    bufferDev.open(QIODevice::ReadWrite);
    const uint8_t data[] = {0xAA, 0xBB, 4, 0x01, 0x02, 0x03, 0x04};
    bufferDev.write((const char*) data, 7);
    bufferDev.seek(0);

    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE(spy.wait(READYREAD_TIMEOUT));
    REQUIRE(sink.totalFed == 4);
}

TEST_CASE("FramedReader shouldn't read when disabled", "[reader]")
{
    QBuffer bufferDev;
    FramedReader reader(&bufferDev);

    TestSink sink;
    reader.connectSink(&sink);

    REQUIRE(sink._numChannels == 1);
    REQUIRE(sink._hasX == false);

    bufferDev.open(QIODevice::ReadWrite);
    const uint8_t data[] = {0xAA, 0xBB, 4, 0x01, 0x02, 0x03, 0x04};
    bufferDev.write((const char*) data, 7);
    bufferDev.seek(0);

    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE_FALSE(spy.wait(READYREAD_TIMEOUT));
    REQUIRE(sink.totalFed == 0);
}

TEST_CASE("DemoReader doesn't have a settings widget", "[reader, demo]")
{
    QBuffer bufferDev;          // not actually used
    DemoReader reader(&bufferDev);

    REQUIRE(reader.settingsWidget() == NULL);
}

TEST_CASE("Generating data with DemoReader", "[reader, demo]")
{
    QBuffer bufferDev;          // not actually used
    DemoReader demoReader(&bufferDev);
    demoReader.enable(true);

    TestSink sink;
    demoReader.connectSink(&sink);
    REQUIRE(sink._numChannels == 1);

    demoReader.setNumOfChannels(3);
    REQUIRE(sink._numChannels == 3);

    // we need to wait somehow, we are not actually looking for signals
    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE_FALSE(spy.wait(1000)); // we need some time for demoreader to produce data
    REQUIRE(sink.totalFed >= 9);
}

TEST_CASE("DemoReader shouldn't generate data when paused", "[reader, demo]")
{
    QBuffer bufferDev;          // not actually used
    DemoReader demoReader(&bufferDev); // paused by default

    TestSink sink;
    demoReader.connectSink(&sink);
    REQUIRE(sink._numChannels == 1);

    demoReader.setNumOfChannels(3);
    REQUIRE(sink._numChannels == 3);

    // we need to wait somehow, we are not actually looking for signals
    QSignalSpy spy(&bufferDev, SIGNAL(readyRead()));
    REQUIRE_FALSE(spy.wait(1000)); // we need some time for demoreader to produce data
    REQUIRE(sink.totalFed == 0);
}


// Note: this is added because `QApplication` must be created for widgets
#include <QApplication>
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return result;
}
