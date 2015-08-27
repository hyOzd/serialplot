/*
  Copyright © 2015 Hasan Yavuz Özderya

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

#ifndef SCALEZOOMER_H
#define SCALEZOOMER_H

#include <QObject>
#include <QPen>
#include <qwt_plot.h>
#include <qwt_plot_zoomer.h>

#include "scalepicker.h"

class ScaleZoomer : public QObject
{
    Q_OBJECT

public:
    ScaleZoomer(QwtPlot*, QwtPlotZoomer*);
    void setPickerPen(QPen pen);

private:
    QwtPlot* _plot;
    QwtPlotZoomer* _zoomer;
    ScalePicker bottomPicker;
    ScalePicker leftPicker;

private slots:
    void bottomPicked(double firstPos, double lastPos);
    void leftPicked(double firstPos, double lastPos);
};

#endif /* SCALEZOOMER_H */
