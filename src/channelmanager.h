/*
  Copyright © 2016 Hasan Yavuz Özderya

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
#include <QStringListModel>
#include <QModelIndex>
#include <QVector>

#include "framebuffer.h"

class ChannelManager : public QObject
{
    Q_OBJECT
public:
    explicit ChannelManager(unsigned numberOfChannels, unsigned numberOfSamples, QObject *parent = 0);
    ~ChannelManager();

    unsigned numOfChannels();
    unsigned numOfSamples();
    FrameBuffer* channelBuffer(unsigned channel);
    QStringListModel* channelNames();
    QString channelName(unsigned channel);

signals:
    void numOfChannelsChanged(unsigned value);
    void numOfSamplesChanged(unsigned value);
    void channelNameChanged(unsigned channel, QString name);

public slots:
    void setNumOfChannels(unsigned number);
    void setNumOfSamples(unsigned number);
    void setChannelName(unsigned channel, QString name);
    void addChannelData(unsigned channel, double* data, unsigned size);

private:
    unsigned _numOfChannels;
    unsigned _numOfSamples;
    QList<FrameBuffer*> channelBuffers;
    QStringListModel _channelNames;

    void addChannelName(QString name); ///< appends a new channel name at the end of list

private slots:
    void onChannelNameDataChange(const QModelIndex & topLeft,
                                 const QModelIndex & bottomRight,
                                 const QVector<int> & roles = QVector<int> ());
};

#endif // CHANNELMANAGER_H
