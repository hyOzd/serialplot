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

#ifndef BARPLOT_H
#define BARPLOT_H

#include <qwt_plot.h>

#include "stream.h"
#include "plotmenu.h"
#include "barchart.h"

class BarPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit BarPlot(Stream* stream,
                     PlotMenu* menu,
                     QWidget* parent = 0);

public slots:
    /// Set the Y axis
    void setYAxis(bool autoScaled, double yMin = 0, double yMax = 1);
    /// Enable/disable dark background
    void darkBackground(bool enabled);

private:
    Stream* _stream;
    PlotMenu* _menu;
    BarChart barChart;

    QVector<double> chartData() const;

private slots:
    void update();
};

#endif // BARPLOT_H
