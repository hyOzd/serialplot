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

#ifndef BARCHART_H
#define BARCHART_H

#include <qwt_plot_barchart.h>
#include <qwt_column_symbol.h>
#include "channelmanager.h"

class BarChart : public QwtPlotBarChart
{
public:
    explicit BarChart(ChannelManager* channelMan);

    void resample();
    QwtColumnSymbol* specialSymbol(int sampleIndex, const QPointF&) const;

private:
    ChannelManager* _channelMan;

    QVector<double> chartData() const;
};


#endif // BARCHART_H
