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

#include <QRectF>
#include <QKeySequence>
#include <QColor>
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include <math.h>
#include <algorithm>

#include "plot.h"
#include "utils.h"

static const int SYMBOL_SHOW_AT_WIDTH = 5;
static const int SYMBOL_SIZE_MAX = 7;

Plot::Plot(QWidget* parent) :
    QwtPlot(parent),
    zoomer(this->canvas(), false),
    sZoomer(this, &zoomer)
{
    isAutoScaled = true;
    symbolSize = 0;
    numOfSamples = 1;
    showSymbols = Plot::Auto;

    QObject::connect(&zoomer, &Zoomer::unzoomed, this, &Plot::unzoomed);

    zoomer.setZoomBase();
    grid.attach(this);
    legend.attach(this);

    showGrid(false);
    darkBackground(false);

    snapshotOverlay = NULL;

    connect(&zoomer, &QwtPlotZoomer::zoomed,
            [this](const QRectF &rect)
            {
                onXScaleChanged();
            });

    connect(this, &QwtPlot::itemAttached,
            [this](QwtPlotItem *plotItem, bool on)
            {
                if (symbolSize) updateSymbols();
            });

    // init demo indicator
    QwtText demoText(" DEMO RUNNING ");  // looks better with spaces
    demoText.setColor(QColor("white"));
    demoText.setBackgroundBrush(Qt::darkRed);
    demoText.setBorderRadius(4);
    demoText.setRenderFlags(Qt::AlignLeft | Qt::AlignBottom);
    demoIndicator.setText(demoText);
    demoIndicator.hide();
    demoIndicator.attach(this);
}

Plot::~Plot()
{
    if (snapshotOverlay != NULL) delete snapshotOverlay;
}

void Plot::setYAxis(bool autoScaled, double yAxisMin, double yAxisMax)
{
    this->isAutoScaled = autoScaled;

    if (!autoScaled)
    {
        yMin = yAxisMin;
        yMax = yAxisMax;
    }

    zoomer.zoom(0);
    resetAxes();
}

void Plot::setXAxis(double xMin, double xMax)
{
    _xMin = xMin;
    _xMax = xMax;

    zoomer.zoom(0); // unzoom

    // set axis
    setAxisScale(QwtPlot::xBottom, xMin, xMax);
    replot(); // Note: if we don't replot here scale at startup isn't set correctly

    // reset zoom base
    auto base = zoomer.zoomBase();
    base.setLeft(xMin);
    base.setRight(xMax);
    zoomer.setZoomBase(base);

    onXScaleChanged();
}

void Plot::resetAxes()
{
    // reset y axis
    if (isAutoScaled)
    {
        setAxisAutoScale(QwtPlot::yLeft);
    }
    else
    {
        setAxisScale(QwtPlot::yLeft, yMin, yMax);
    }

    zoomer.setZoomBase();

    replot();
}

void Plot::unzoomed()
{
    resetAxes();
    onXScaleChanged();
}

void Plot::showGrid(bool show)
{
    grid.enableX(show);
    grid.enableY(show);
    replot();
}

void Plot::showMinorGrid(bool show)
{
    grid.enableXMin(show);
    grid.enableYMin(show);
    replot();
}

void Plot::showLegend(bool show)
{
    legend.setVisible(show);
    replot();
}

void Plot::showDemoIndicator(bool show)
{
    demoIndicator.setVisible(show);
    replot();
}

void Plot::unzoom()
{
    zoomer.zoom(0);
}

void Plot::darkBackground(bool enabled)
{
    QColor gridColor;
    if (enabled)
    {
        setCanvasBackground(QBrush(Qt::black));
        gridColor.setHsvF(0, 0, 0.25);
        grid.setPen(gridColor);
        zoomer.setRubberBandPen(QPen(Qt::white));
        zoomer.setTrackerPen(QPen(Qt::white));
        sZoomer.setPickerPen(QPen(Qt::white));
        legend.setTextPen(QPen(Qt::white));
    }
    else
    {
        setCanvasBackground(QBrush(Qt::white));
        gridColor.setHsvF(0, 0, 0.80);
        grid.setPen(gridColor);
        zoomer.setRubberBandPen(QPen(Qt::black));
        zoomer.setTrackerPen(QPen(Qt::black));
        sZoomer.setPickerPen(QPen(Qt::black));
        legend.setTextPen(QPen(Qt::black));
    }
    updateSymbols();
    replot();
}

/*
  Below crude drawing demostrates how color selection occurs for
  given channel index

  0°                     <--Hue Value-->                           360°
  |* . o . + . o . * . o . + . o . * . o . + . o . * . o . + . o . |

  * -> 0-3
  + -> 4-7
  o -> 8-15
  . -> 16-31

 */
QColor Plot::makeColor(unsigned int channelIndex)
{
    auto i = channelIndex;

    if (i < 4)
    {
        return QColor::fromHsv(360*i/4, 255, 230);
    }
    else
    {
        double p = floor(log2(i));
        double n = pow(2, p);
        i = i - n;
        return QColor::fromHsv(360*i/n + 360/pow(2,p+1), 255, 230);
    }
}

void Plot::flashSnapshotOverlay(bool light)
{
    if (snapshotOverlay != NULL) delete snapshotOverlay;

    QColor color;
    if(light)
    {
        color = QColor(Qt::white);
    }
    else
    {
        color = QColor(Qt::black);
    }

    snapshotOverlay = new PlotSnapshotOverlay(this->canvas(), color);
    connect(snapshotOverlay, &PlotSnapshotOverlay::done,
            [this]()
            {
                delete snapshotOverlay;
                snapshotOverlay = NULL;
            });
}

void Plot::setSymbols(ShowSymbols shown)
{
    showSymbols = shown;

    if (showSymbols == Plot::Auto)
    {
        calcSymbolSize();
    }
    else if (showSymbols == Plot::Show)
    {
        symbolSize = SYMBOL_SIZE_MAX;
    }
    else
    {
        symbolSize = 0;
    }

    updateSymbols();
    replot();
}

void Plot::onXScaleChanged()
{
    if (showSymbols == Plot::Auto)
    {
        calcSymbolSize();
        updateSymbols();
    }
}

void Plot::calcSymbolSize()
{
    auto sw = axisWidget(QwtPlot::xBottom);
    auto paintDist = sw->scaleDraw()->scaleMap().pDist();
    auto scaleDist = sw->scaleDraw()->scaleMap().sDist();
    auto fullScaleDist = zoomer.zoomBase().width();
    auto zoomRate = fullScaleDist / scaleDist;
    float samplesInView = numOfSamples / zoomRate;
    int symDisPx = round(paintDist / samplesInView);

    if (symDisPx < SYMBOL_SHOW_AT_WIDTH)
    {
        symbolSize = 0;
    }
    else
    {
        symbolSize = std::min(SYMBOL_SIZE_MAX, symDisPx-SYMBOL_SHOW_AT_WIDTH+1);
    }
}

void Plot::updateSymbols()
{
    const QwtPlotItemList curves = itemList( QwtPlotItem::Rtti_PlotCurve );

    if (curves.size() > 0)
    {
        for (int i = 0; i < curves.size(); i++)
        {
            QwtSymbol* symbol = NULL;
            QwtPlotCurve* curve = static_cast<QwtPlotCurve*>(curves[i]);
            if (symbolSize)
            {
                symbol = new QwtSymbol(QwtSymbol::Ellipse,
                                       canvasBackground(),
                                       curve->pen(),
                                       QSize(symbolSize, symbolSize));
            }
            curve->setSymbol(symbol);
        }
    }
}

void Plot::resizeEvent(QResizeEvent * event)
{
    QwtPlot::resizeEvent(event);
    onXScaleChanged();
}

void Plot::setNumOfSamples(unsigned value)
{
    numOfSamples = value;
    onXScaleChanged();
}
