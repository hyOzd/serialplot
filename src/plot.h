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

#ifndef PLOT_H
#define PLOT_H

#include <QColor>
#include <QList>
#include <QAction>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_shapeitem.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_textlabel.h>

#include "zoomer.h"
#include "scalezoomer.h"
#include "plotsnapshotoverlay.h"

class Plot : public QwtPlot
{
    Q_OBJECT

    friend class PlotManager;

public:
    enum ShowSymbols
    {
        ShowSymbolsAuto,
        ShowSymbolsShow,
        ShowSymbolsHide
    };

    Plot(QWidget* parent = 0);
    ~Plot();

    static QColor makeColor(unsigned int channelIndex);

public slots:
    void showGrid(bool show = true);
    void showMinorGrid(bool show = true);
    void showLegend(bool show = true);
    void showDemoIndicator(bool show = true);
    void showNoChannel(bool show = true);
    void unzoom();
    void darkBackground(bool enabled = true);
    void setYAxis(bool autoScaled, double yMin = 0, double yMax = 1);
    void setXAxis(double xMin, double xMax);
    void setSymbols(ShowSymbols shown);

    /**
     * Displays an animation for snapshot.
     *
     * @param light show a light colored (white) animation or the opposite
     */
    void flashSnapshotOverlay(bool light);

    void setNumOfSamples(unsigned value);

    void setPlotWidth(double width);

protected:
    /// update the display of symbols depending on `symbolSize`
    void updateSymbols();

private:
    bool isAutoScaled;
    double yMin, yMax;
    double _xMin, _xMax;
    unsigned numOfSamples;
    double plotWidth;
    int symbolSize;
    Zoomer zoomer;
    ScaleZoomer sZoomer;
    QwtPlotGrid grid;
    PlotSnapshotOverlay* snapshotOverlay;
    QwtPlotLegendItem legend;
    QwtPlotTextLabel demoIndicator;
    QwtPlotTextLabel noChannelIndicator;
    ShowSymbols showSymbols;

    void resetAxes();
    void resizeEvent(QResizeEvent * event);
    void calcSymbolSize();

private slots:
    void unzoomed();
    void onXScaleChanged();
};

#endif // PLOT_H
