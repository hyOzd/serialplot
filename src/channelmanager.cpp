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

#include "channelmanager.h"

#include <QStringList>
#include <QModelIndex>

ChannelManager::ChannelManager(unsigned numberOfChannels, unsigned numberOfSamples, QObject *parent) :
    QObject(parent)
{
    _numOfChannels = numberOfChannels;
    _numOfSamples = numberOfSamples;

    QStringList channelNamesList;

    for (unsigned int i = 0; i < numberOfChannels; i++)
    {
        channelBuffers.append(new FrameBuffer(numberOfSamples));
        channelNamesList << QString("Channel %1").arg(i+1);

        // curves.append(new QwtPlotCurve(QString("Channel %1").arg(i+1)));
        // curves[i]->setSamples(channelBuffers[i]);
        // curves[i]->setPen(Plot::makeColor(i));
        // curves[i]->attach(ui->plot);
    }

    _channelNames.setStringList(channelNamesList);

    connect(&_channelNames, &QStringListModel::dataChanged,
            this, &ChannelManager::onChannelNameDataChange);
}

ChannelManager::~ChannelManager()
{
    // TODO: remove all channelBuffers
}

unsigned ChannelManager::numOfChannels()
{
    return channelBuffers.size();
}

unsigned ChannelManager::numOfSamples()
{
    return _numOfSamples;
}

void ChannelManager::setNumOfChannels(unsigned number)
{
    unsigned int oldNum = channelBuffers.size();

    if (number > oldNum)
    {
        // add new channels
        for (unsigned int i = 0; i < number - oldNum; i++)
        {
            channelBuffers.append(new FrameBuffer(_numOfSamples));
            addChannelName(QString("Channel %1").arg(oldNum+i+1));
        }
    }
    else if(number < oldNum)
    {
        // remove channels
        for (unsigned int i = 0; i < oldNum - number; i++)
        {
            // also deletes owned FrameBuffer
            // delete curves.takeLast();
            // TODO: important, remove channelBuffer
            channelBuffers.removeLast();
        }
    }

    emit numOfChannelsChanged(number);
}

void ChannelManager::setNumOfSamples(unsigned number)
{
    _numOfSamples = number;

    for (int ci = 0; ci < channelBuffers.size(); ci++)
    {
        channelBuffers[ci]->resize(_numOfSamples);
    }

    emit numOfSamplesChanged(number);
}

FrameBuffer* ChannelManager::channelBuffer(unsigned channel)
{
    return channelBuffers[channel];
}

QStringListModel* ChannelManager::channelNames()
{
    return &_channelNames;
}

QString ChannelManager::channelName(unsigned channel)
{
    return _channelNames.data(_channelNames.index(channel, 0), Qt::DisplayRole).toString();
}

void ChannelManager::setChannelName(unsigned channel, QString name)
{
    _channelNames.setData(_channelNames.index(channel, 0), QVariant(name), Qt::DisplayRole);
}

void ChannelManager::addChannelName(QString name)
{
    _channelNames.insertRow(_channelNames.rowCount());
    setChannelName(_channelNames.rowCount()-1, name);
}

void ChannelManager::onChannelNameDataChange(const QModelIndex & topLeft,
                                             const QModelIndex & bottomRight,
                                             const QVector<int> & roles)
{
    int start = topLeft.row();
    int end = bottomRight.row();

    // TODO: maybe check `roles` parameter, can't think of a reason for current use case
    for (int i = start; i <= end; i++)
    {
        emit channelNameChanged(i, channelName(i));
    }
}

void ChannelManager::addChannelData(unsigned channel, double* data, unsigned size)
{
    channelBuffer(channel)->addSamples(data, size);
}
