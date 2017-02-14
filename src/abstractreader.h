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

#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include <QObject>
#include <QIODevice>
#include <QWidget>
#include <QTimer>

#include "channelmanager.h"
#include "datarecorder.h"

/**
 * All reader classes must inherit this class.
 */
class AbstractReader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractReader(QIODevice* device, ChannelManager* channelMan,
                            DataRecorder* recorder, QObject* parent = 0);

    bool recording;                 /// is recording started

    /**
     * Returns a widget to be shown in data format panel when reader
     * is selected.
     */
    virtual QWidget* settingsWidget() = 0;

    /**
     * Number of channels being read.
     *
     * This number may be user selected or automatically determined
     * from incoming stream.
     */
    virtual unsigned numOfChannels() = 0;

    /// Reader should only read when enabled. Default state should be
    /// 'disabled'.
    virtual void enable(bool enabled = true) = 0;

    /**
     * @brief Starts sending data to recorder.
     *
     * @note recorder must have been started!
     */
    void startRecording();

    /// Stops recording.
    void stopRecording();

signals:
    void numOfChannelsChanged(unsigned);
    void samplesPerSecondChanged(unsigned);

public slots:
    /**
     * Pauses the reading.
     *
     * Reader should actually continue reading to keep the
     * synchronization but shouldn't commit data.
     */
    virtual void pause(bool) = 0;

protected:
    QIODevice* _device;

    /// Should be called with read data
    void addData(double* samples, unsigned length);

private:
    const int SPS_UPDATE_TIMEOUT = 1;  // second

    unsigned sampleCount;
    unsigned samplesPerSecond;

    ChannelManager* _channelMan;
    DataRecorder* _recorder;
    QTimer spsTimer;

private slots:
    void spsTimerTimeout();
};

#endif // ABSTRACTREADER_H
