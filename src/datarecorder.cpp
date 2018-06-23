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

#include "datarecorder.h"

#include <QtDebug>

DataRecorder::DataRecorder(QObject *parent) :
    QObject(parent),
    fileStream(&file)
{
    lastNumChannels = 0;
    disableBuffering = false;
    windowsLE = false;
}

bool DataRecorder::startRecording(QString fileName, QString separator, QStringList channelNames)
{
    Q_ASSERT(!file.isOpen());
    _sep =  separator;

    // open file
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Opening file " << fileName
                    << " for recording failed with error: " << file.error();
        return false;
    }

    // write header line
    if (!channelNames.isEmpty())
    {
        fileStream << channelNames.join(_sep);
        fileStream << le();
        lastNumChannels = channelNames.length();
    }
    return true;
}

void DataRecorder::feedIn(const SamplePack& data)
{
    Q_ASSERT(file.isOpen());    // recorder should be disconnected before stopping recording
    Q_ASSERT(!data.hasX());     // NYI

    // check if number of channels has changed during recording and warn
    unsigned numChannels = data.numChannels();
    if (lastNumChannels != 0 && numChannels != lastNumChannels)
    {
        qWarning() << "Number of channels changed from " << lastNumChannels
                   << " to " << numChannels <<
            " during recording, CSV file is corrupted but no data will be lost.";
    }
    lastNumChannels = numChannels;

    // write data
    unsigned numSamples = data.numSamples();
    for (unsigned int i = 0; i < numSamples; i++)
    {
        for (unsigned ci = 0; ci < numChannels; ci++)
        {
            fileStream << data.data(ci)[i];
            if (ci != numChannels-1) fileStream << _sep;
        }
        fileStream << le();
    }

    if (disableBuffering) fileStream.flush();
}

void DataRecorder::stopRecording()
{
    Q_ASSERT(file.isOpen());

    file.close();
    lastNumChannels = 0;
}

const char* DataRecorder::le() const
{
    return windowsLE ? "\r\n" : "\n";
}
