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

#include "barplot.h"
#include "barscaledraw.h"
#include "utils.h"

BarPlot::BarPlot(Stream* stream, PlotMenu* menu, QWidget* parent) :
    QwtPlot(parent), _menu(menu), barChart(stream)
{
    _stream = stream;
    barChart.attach(this);
    setAxisMaxMinor(QwtPlot::xBottom, 0);
    setAxisScaleDraw(QwtPlot::xBottom, new BarScaleDraw(stream));

    update();
    connect(_stream, &Stream::dataAdded, this, &BarPlot::update);
    connect(_stream, &Stream::numChannelsChanged, this, &BarPlot::update);

    // connect to menu
    connect(&menu->darkBackgroundAction, SELECT<bool>::OVERLOAD_OF(&QAction::toggled),
            this, &BarPlot::darkBackground);
    darkBackground(menu->darkBackgroundAction.isChecked());
}

void BarPlot::update()
{
    // Note: -0.99 is used instead of -1 to handle the case of `numOfChannels==1`
    setAxisScale(QwtPlot::xBottom, 0, _stream->numChannels()-0.99, 1);
    barChart.resample();
    replot();
}

void BarPlot::setYAxis(bool autoScaled, double yMin, double yMax)
{
    if (autoScaled)
    {
        setAxisAutoScale(QwtPlot::yLeft);
    }
    else
    {
        setAxisScale(QwtPlot::yLeft, yMin, yMax);
    }
}


void BarPlot::darkBackground(bool enabled)
{
    if (enabled)
    {
        setCanvasBackground(QBrush(Qt::black));
    }
    else
    {
        setCanvasBackground(QBrush(Qt::white));
    }
    replot();
}
