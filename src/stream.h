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

#ifndef STREAM_H
#define STREAM_H

#include <QObject>
#include <QModelIndex>
#include <QVector>
#include <QSettings>

#include "sink.h"
#include "source.h"
#include "channelinfomodel.h"
#include "streamchannel.h"
#include "framebuffer2.h"

/**
 * Main waveform storage class. It consists of channels. Channels are
 * synchronized with each other.
 *
 * Implements `Sink` class for data entry. It's expected to be
 * connected to a `Device` source.
 */
class Stream : public QObject, public Sink
{
    Q_OBJECT

public:
    /**
     * @param nc number of channels
     * @param x has X data input
     * @param ns number of samples
     */
    Stream(unsigned nc = 0, bool x = false, unsigned ns = 0);
    ~Stream();

    // implementations for `Source`
    virtual bool hasX() const;
    virtual unsigned numChannels() const;

    unsigned numSamples() const;
    const StreamChannel* channel(unsigned index) const;
    StreamChannel* channel(unsigned index);
    const ChannelInfoModel* infoModel() const;

    /// Saves channel information
    void saveSettings(QSettings* settings) const;
    /// Load channel information
    void loadSettings(QSettings* settings);

protected:
    // implementations for `Sink`
    virtual void setNumChannels(unsigned nc, bool x);
    virtual void feedIn(const SamplePack& data);

signals:
    void numChannelsChanged(unsigned value);
    void numSamplesChanged(unsigned value);
    void channelAdded(const StreamChannel* chan);
    void channelNameChanged(unsigned channel, QString name); // TODO: does it stay?
    void dataAdded(); ///< emitted when data added to channel man.

public slots:
    // TODO: these won't be public
    // void setNumChannels(unsigned number);
    void setNumSamples(unsigned value);

    /// When paused data feed is ignored
    void pause(bool paused);

private:
    unsigned _numSamples;
    bool _paused;

    bool _hasx;
    ResizableBuffer* xData;
    QList<StreamChannel*> channels;

    ChannelInfoModel _infoModel;
};


#endif // STREAM_H
