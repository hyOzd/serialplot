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

#include "barplot.h"

BarPlot::BarPlot(ChannelManager* channelMan, QWidget* parent) :
    QwtPlot(parent)
{
    _channelMan = channelMan;
    barChart.attach(this);

    connect(_channelMan, &ChannelManager::dataAdded, [this]()
            {
                barChart.setSamples(chartData());
                replot();
            });
}

QVector<double> BarPlot::chartData() const
{
    unsigned numChannels = _channelMan->numOfChannels();
    unsigned numOfSamples = _channelMan->numOfSamples();
    QVector<double> data(numChannels);
    for (int i = 0; i < numChannels; i++)
    {
        data[i] = _channelMan->channelBuffer(i)->sample(numOfSamples-1);
    }
    return data;
}
