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

#ifndef DATARECORDER_H
#define DATARECORDER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#include "sink.h"

/**
 * Implemented as a `Sink` that writes incoming data to a file. Before
 * connecting a `Source` recording must be started with the `startRecording`
 * method. Also before calling `stopRecording`, recorder should be disconnected
 * from source.
 */
class DataRecorder : public QObject, public Sink
{
    Q_OBJECT
public:
    explicit DataRecorder(QObject *parent = 0);

    /// Disables file buffering
    bool disableBuffering;

    /**
     * Use CR+LF as line ending. `false` by default.
     *
     * @note Toggling this variable during a recording will result in
     * a corrupted file. Care must be taken at higher (UI) levels.
     */
    bool windowsLE;

    /**
     * @brief Starts recording data to a file in CSV format.
     *
     * File is opened and header line (names of channels) is written. After
     * calling this function recorder should be connected to a `Source`.
     *
     * @param fileName name of the recording file
     * @param separator column separator
     * @param channelNames names of the channels for header line, if empty no header line is written
     * @param insertTime enable inserting timestamp
     * @return false if file operation fails (read only etc.)
     */
    bool startRecording(QString fileName, QString separator,
                        QStringList channelNames, bool insertTime);

    /**
     * @brief Adds data to a channel.
     *
     * Multiple rows of data can be added at a time. Each channels
     * data should be ordered consecutively in the `data` array:
     *
     * [CH0_SMP0, CH0_SMP1 ... CH0_SMPN, CH1_SMP0, CH1_SMP1, ... , CHN_SMPN]
     *
     * If `numOfChannels` changes during recording, no data will be
     * lost (ie. it will be written to the file) but this will produce
     * an invalid CSV file. An error message will be written to the
     * console.
     *
     * @param data samples array
     * @param length number of samples in `data`, must be multiple of `numOfChannels`
     * @param numOfChannels how many channels samples this data carries
     */
    void addData(double* data, unsigned length, unsigned numOfChannels);

    /// Stops recording, closes file.
    void stopRecording();

protected:
    virtual void feedIn(const SamplePack& data);

private:
    unsigned lastNumChannels;   ///< used for error message only
    QFile file;
    QTextStream fileStream;
    QString _sep;
    bool timestampEn;

    /// Returns the selected line ending.
    const char* le() const;
};

#endif // DATARECORDER_H
