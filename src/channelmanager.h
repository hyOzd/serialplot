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

#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include <QObject>
#include <QStringList>
#include <QModelIndex>
#include <QVector>
#include <QSettings>

#include "framebuffer.h"
#include "channelinfomodel.h"

class ChannelManager : public QObject
{
    Q_OBJECT
public:
    explicit ChannelManager(unsigned numberOfChannels, unsigned numberOfSamples, QObject *parent = 0);
    ~ChannelManager();

    unsigned numOfChannels();
    unsigned numOfSamples();
    FrameBuffer* channelBuffer(unsigned channel);
    QString channelName(unsigned channel);
    /// Stores channel names into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads channel names from a `QSettings`.
    void loadSettings(QSettings* settings);
    /// Returns a model that manages channel information (name, color etc)
    ChannelInfoModel* infoModel();
    /// Returns a list of channel names
    QStringList channelNames();

signals:
    void numOfChannelsChanged(unsigned value);
    void numOfSamplesChanged(unsigned value);
    void channelNameChanged(unsigned channel, QString name);
    void dataAdded(); ///< emitted when data added to channel man.

public slots:
    void setNumOfChannels(unsigned number);
    void setNumOfSamples(unsigned number);
    /**
     * Add data for all channels.
     *
     * All channels data is provided in a single array which contains equal
     * number of samples for all channels. Structure is as shown below:
     *
     * [CH0_SMP0, CH0_SMP1 ... CH0_SMPN, CH1_SMP0, CH1_SMP1, ... , CHN_SMPN]
     *
     * @param data samples for all channels
     * @param size size of `data`, must be multiple of `numOfChannels`
     */
    void addData(double* data, unsigned size);

    /// When paused `addData` does nothing.
    void pause(bool paused);

private:
    unsigned _numOfChannels;
    unsigned _numOfSamples;
    bool _paused;
    QList<FrameBuffer*> channelBuffers;
    // QStringListModel _channelNames;
    ChannelInfoModel _infoModel;

    void addChannelName(QString name); ///< appends a new channel name at the end of list

private slots:
    void onChannelInfoChanged(const QModelIndex & topLeft,
                              const QModelIndex & bottomRight,
                              const QVector<int> & roles = QVector<int> ());
};

#endif // CHANNELMANAGER_H
