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

#include <QModelIndex>

#include <QtDebug>

#include "channelmanager.h"
#include "setting_defines.h"

ChannelManager::ChannelManager(unsigned numberOfChannels, unsigned numberOfSamples, QObject *parent) :
    QObject(parent),
    _infoModel(numberOfChannels)
{
    _numOfChannels = numberOfChannels;
    _numOfSamples = numberOfSamples;
    _paused = false;

    for (unsigned int i = 0; i < numberOfChannels; i++)
    {
        channelBuffers.append(new FrameBuffer(numberOfSamples));
    }

    connect(&_infoModel, &ChannelInfoModel::dataChanged,
            this, &ChannelManager::onChannelInfoChanged);
}

ChannelManager::~ChannelManager()
{
    for (auto buffer : channelBuffers)
    {
        delete buffer;
    }
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
        }
    }
    else if(number < oldNum)
    {
        // remove channels
        for (unsigned int i = oldNum-1; i > number-1; i--)
        {
            delete channelBuffers.takeLast();
        }
    }

    _numOfChannels = number;
    _infoModel.setNumOfChannels(number);

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

void ChannelManager::pause(bool paused)
{
    _paused = paused;
}

FrameBuffer* ChannelManager::channelBuffer(unsigned channel)
{
    return channelBuffers[channel];
}

ChannelInfoModel* ChannelManager::infoModel()
{
    return &_infoModel;
}

QString ChannelManager::channelName(unsigned channel)
{
    return _infoModel.data(_infoModel.index(channel, ChannelInfoModel::COLUMN_NAME),
                           Qt::DisplayRole).toString();
}

void ChannelManager::onChannelInfoChanged(const QModelIndex & topLeft,
                                          const QModelIndex & bottomRight,
                                          const QVector<int> & roles)
{
    int start = topLeft.row();
    int end = bottomRight.row();
    int col = topLeft.column();

    for (int ci = start; ci <= end; ci++)
    {
        for (auto role : roles)
        {
            switch (role)
            {
                case Qt::EditRole:
                    if (col == ChannelInfoModel::COLUMN_NAME)
                    {
                        emit channelNameChanged(ci, channelName(ci));
                    }
                    break;
                case Qt::ForegroundRole:
                    if (col == ChannelInfoModel::COLUMN_NAME)
                    {
                        // TODO: emit channel color changed
                    }
                    break;
                case Qt::CheckStateRole:
                    if (col == ChannelInfoModel::COLUMN_VISIBILITY)
                    {
                        // TODO: emit visibility
                    }
                    break;
            }
        }
        // emit channelNameChanged(i, channelName(i));
    }
}

void ChannelManager::addData(double* data, unsigned size)
{
    Q_ASSERT(size % _numOfChannels == 0);

    if (_paused) return;

    int n = size / _numOfChannels;
    for (unsigned ci = 0; ci < _numOfChannels; ci++)
    {
        channelBuffers[ci]->addSamples(&data[ci*n], n);
    }

    emit dataAdded();
}

void ChannelManager::saveSettings(QSettings* settings)
{
    _infoModel.saveSettings(settings);
}

void ChannelManager::loadSettings(QSettings* settings)
{
    _infoModel.loadSettings(settings);
}
