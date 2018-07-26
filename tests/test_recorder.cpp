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

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include <QDir>
#include "datarecorder.h"
#include "test_helpers.h"

#define TEST_FILE_NAME   "sp_test_recording.csv"

TEST_CASE("test recording single channel", "[recorder]")
{
    DataRecorder rec;
    TestSource source(1, false);

    // temporary file, remove if exists
    auto fileName = QDir::tempPath() + QString("/" TEST_FILE_NAME);
    if (QFile::exists(fileName)) QFile::remove(fileName);

    // connect source → sink
    source.connectSink(&rec);

    // prepare data
    QStringList channelNames({"Channel 1"});
    SamplePack samples(5, 1);
    for (int i = 0; i < 5; i++)
    {
        samples.data(0)[i] = i+1;
    }

    // test
    rec.startRecording(fileName, ",", channelNames, false);
    source._feed(samples);
    rec.stopRecording();

    // read file contents back
    QFile recordFile(fileName);
    REQUIRE(recordFile.open(QIODevice::ReadOnly | QIODevice::Text));
    // NOTE: mind the extra parantheses, otherwise 'catch' macros fail to compile
    REQUIRE((recordFile.readLine() == "Channel 1\n"));
    for (int i = 0; i < 5; i++)
        REQUIRE((recordFile.readLine() == QString("%1\n").arg(i+1)));

    // cleanup
    if (QFile::exists(fileName)) QFile::remove(fileName);
}

TEST_CASE("test recording multiple channels", "[recorder]")
{
    DataRecorder rec;
    TestSource source(3, false);

    // temporary file, remove if exists
    auto fileName = QDir::tempPath() + QString("/" TEST_FILE_NAME);
    if (QFile::exists(fileName)) QFile::remove(fileName);

    // connect source → sink
    source.connectSink(&rec);

    // prepare data
    QStringList channelNames({"Channel 1", "Channel 2", "Channel 3"});
    SamplePack samples(5, 3);
    for (int ci = 0; ci < 3; ci++)
    {
        for (int i = 0; i < 5; i++)
        {
            samples.data(ci)[i] = (ci+1)*(i+1);
        }
    }

    // test
    rec.startRecording(fileName, ",", channelNames, false);
    source._feed(samples);
    rec.stopRecording();

    // read file contents back
    QFile recordFile(fileName);
    REQUIRE(recordFile.open(QIODevice::ReadOnly | QIODevice::Text));
    // NOTE: mind the extra parantheses, otherwise 'catch' macros fail to compile
    REQUIRE((recordFile.readLine() == "Channel 1,Channel 2,Channel 3\n"));
    REQUIRE((recordFile.readLine() == "1,2,3\n"));
    REQUIRE((recordFile.readLine() == "2,4,6\n"));
    REQUIRE((recordFile.readLine() == "3,6,9\n"));
    REQUIRE((recordFile.readLine() == "4,8,12\n"));
    REQUIRE((recordFile.readLine() == "5,10,15\n"));

    // cleanup
    if (QFile::exists(fileName)) QFile::remove(fileName);
}
