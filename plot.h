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

#ifndef PLOT_H
#define PLOT_H

#include <QColor>
#include <QList>
#include <QAction>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_shapeitem.h>

#include "zoomer.h"
#include "scalezoomer.h"
#include "plotsnapshotoverlay.h"

class Plot : public QwtPlot
{
    Q_OBJECT

public:
    Plot(QWidget* parent = 0);
    ~Plot();
    void setAxis(bool autoScaled, double yMin = 0, double yMax = 1);

    QList<QAction*> menuActions();

    static QColor makeColor(unsigned int channelIndex);

private:
    bool isAutoScaled;
    double yMin, yMax;
    Zoomer zoomer;
    ScaleZoomer sZoomer;
    QwtPlotGrid grid;
    PlotSnapshotOverlay* snapshotOverlay;

    QAction _showGridAction;
    QAction _showMinorGridAction;
    QAction _unzoomAction;
    QAction _darkBackgroundAction;

    void resetAxes();

public slots:
    void showGrid(bool show = true);
    void showMinorGrid(bool show = true);
    void unzoom();
    void darkBackground(bool enabled = true);

    void flashSnapshotOverlay();

private slots:
    void unzoomed();
};

#endif // PLOT_H
