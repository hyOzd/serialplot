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

#include <QtDebug>

#include "plot.h"
#include "plotmanager.h"
#include "utils.h"
#include "setting_defines.h"

PlotManager::PlotManager(QWidget* plotArea, QObject *parent) :
    QObject(parent),
    _plotArea(plotArea),
    showGridAction("Grid", this),
    showMinorGridAction("Minor Grid", this),
    unzoomAction("Unzoom", this),
    darkBackgroundAction("Dark Background", this),
    showLegendAction("Legend", this),
    showMultiAction("Multi Plot", this)
{
    _autoScaled = true;
    _yMin = 0;
    _yMax = 1;
    isDemoShown = false;

    // initalize layout and single widget
    isMulti = false;
    scrollArea = NULL;
    setupLayout(isMulti);
    addPlotWidget();

    // initialize menu actions
    showGridAction.setToolTip("Show Grid");
    showMinorGridAction.setToolTip("Show Minor Grid");
    unzoomAction.setToolTip("Unzoom the Plot");
    darkBackgroundAction.setToolTip("Enable Dark Plot Background");
    showLegendAction.setToolTip("Display the Legend on Plot");
    showMultiAction.setToolTip("Display All Channels Separately");

    showGridAction.setShortcut(QKeySequence("G"));
    showMinorGridAction.setShortcut(QKeySequence("M"));

    showGridAction.setCheckable(true);
    showMinorGridAction.setCheckable(true);
    darkBackgroundAction.setCheckable(true);
    showLegendAction.setCheckable(true);
    showMultiAction.setCheckable(true);

    showGridAction.setChecked(false);
    showMinorGridAction.setChecked(false);
    darkBackgroundAction.setChecked(false);
    showLegendAction.setChecked(true);
    showMultiAction.setChecked(false);

    showMinorGridAction.setEnabled(false);

    connect(&showGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::showGrid);
    connect(&showGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            &showMinorGridAction, &QAction::setEnabled);
    connect(&showMinorGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::showMinorGrid);
    connect(&unzoomAction, &QAction::triggered, this, &PlotManager::unzoom);
    connect(&darkBackgroundAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::darkBackground);
    connect(&showLegendAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::showLegend);
    connect(&showLegendAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::showLegend);
    connect(&showMultiAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::setMulti);
}

PlotManager::~PlotManager()
{
    while (curves.size())
    {
        delete curves.takeLast();
    }

    // remove all widgets
    while (plotWidgets.size())
    {
        delete plotWidgets.takeLast();
    }

    if (scrollArea != NULL) delete scrollArea;
}

void PlotManager::setMulti(bool enabled)
{
    if (enabled == isMulti) return;

    isMulti = enabled;

    // detach all curves
    for (auto curve : curves)
    {
        curve->detach();
    }

    // remove all widgets
    while (plotWidgets.size())
    {
        delete plotWidgets.takeLast();
    }

    // setup new layout
    setupLayout(isMulti);

    if (isMulti)
    {
        // add new widgets and attach
        for (auto curve : curves)
        {
            curve->attach(addPlotWidget());
        }
    }
    else
    {
        // add a single widget
        auto plot = addPlotWidget();

        // attach all curves
        for (auto curve : curves)
        {
            curve->attach(plot);
        }
    }
}

void PlotManager::setupLayout(bool multiPlot)
{
    // delete previous layout if it exists
    if (_plotArea->layout() != 0)
    {
        delete _plotArea->layout();
    }

    if (multiPlot)
    {
        // setup a scroll area
        scrollArea = new QScrollArea();
        auto scrolledPlotArea = new QWidget(scrollArea);
        scrollArea->setWidget(scrolledPlotArea);
        scrollArea->setWidgetResizable(true);

        _plotArea->setLayout(new QVBoxLayout());
        _plotArea->layout()->addWidget(scrollArea);
        _plotArea->layout()->setContentsMargins(0,0,0,0);

        layout = new QVBoxLayout(scrolledPlotArea);
    }
    else
    {
        // delete scrollArea left from multi layout
        if (scrollArea != NULL)
        {
            delete scrollArea;
            scrollArea = NULL;
        }

        layout = new QVBoxLayout(_plotArea);
    }

    layout->setContentsMargins(2,2,2,2);
    layout->setSpacing(1);
}

Plot* PlotManager::addPlotWidget()
{
    auto plot = new Plot();
    plotWidgets.append(plot);
    layout->addWidget(plot);

    plot->darkBackground(darkBackgroundAction.isChecked());
    plot->showGrid(showGridAction.isChecked());
    plot->showMinorGrid(showMinorGridAction.isChecked());
    plot->showLegend(showLegendAction.isChecked());
    plot->showDemoIndicator(isDemoShown);
    plot->setAxis(_autoScaled, _yMin, _yMax);

    return plot;
}

void PlotManager::addCurve(QString title, FrameBuffer* buffer)
{
    auto curve = new QwtPlotCurve(title);
    curve->setSamples(new FrameBufferSeries(buffer));
    _addCurve(curve);
}

void PlotManager::addCurve(QString title, QVector<QPointF> data)
{
    auto curve = new QwtPlotCurve(title);
    curve->setSamples(data);
    _addCurve(curve);
}

void PlotManager::_addCurve(QwtPlotCurve* curve)
{
    // store and init the curve
    curves.append(curve);

    unsigned index = curves.size()-1;
    curve->setPen(Plot::makeColor(index));

    // create the plot for the curve if we are on multi display
    Plot* plot;
    if (isMulti)
    {
        // create a new plot widget
        plot = addPlotWidget();
    }
    else
    {
        plot = plotWidgets[0];
    }

    // show the curve
    curve->attach(plot);
    plot->replot();
}

void PlotManager::removeCurves(unsigned number)
{
    for (unsigned i = 0; i < number; i++)
    {
        if (!curves.isEmpty())
        {
            delete curves.takeLast();
            if (isMulti) // delete corresponding widget as well
            {
                delete plotWidgets.takeLast();
            }
        }
    }
}

unsigned PlotManager::numOfCurves()
{
    return curves.size();
}

void PlotManager::setTitle(unsigned index, QString title)
{
    curves[index]->setTitle(title);

    plotWidget(index)->replot();
}

Plot* PlotManager::plotWidget(unsigned curveIndex)
{
    if (isMulti)
    {
        return plotWidgets[curveIndex];
    }
    else
    {
        return plotWidgets[0];
    }
}

void PlotManager::replot()
{
    for (auto plot : plotWidgets)
    {
        plot->replot();
    }
}

QList<QAction*> PlotManager::menuActions()
{
    QList<QAction*> actions;
    actions << &showGridAction;
    actions << &showMinorGridAction;
    actions << &unzoomAction;
    actions << &darkBackgroundAction;
    actions << &showLegendAction;
    actions << &showMultiAction;
    return actions;
}

void PlotManager::showGrid(bool show)
{
    for (auto plot : plotWidgets)
    {
        plot->showGrid(show);
    }
}

void PlotManager::showMinorGrid(bool show)
{
    for (auto plot : plotWidgets)
    {
        plot->showMinorGrid(show);
    }
}

void PlotManager::showLegend(bool show)
{
    for (auto plot : plotWidgets)
    {
        plot->showLegend(show);
    }
}

void PlotManager::showDemoIndicator(bool show)
{
    isDemoShown = show;
    for (auto plot : plotWidgets)
    {
        plot->showDemoIndicator(show);
    }
}

void PlotManager::unzoom()
{
    for (auto plot : plotWidgets)
    {
        plot->unzoom();
    }
}

void PlotManager::darkBackground(bool enabled)
{
    for (auto plot : plotWidgets)
    {
        plot->darkBackground(enabled);
    }
}

void PlotManager::setAxis(bool autoScaled, double yAxisMin, double yAxisMax)
{
    _autoScaled = autoScaled;
    _yMin = yAxisMin;
    _yMax = yAxisMax;
    for (auto plot : plotWidgets)
    {
        plot->setAxis(autoScaled, yAxisMin, yAxisMax);
    }
}

void PlotManager::flashSnapshotOverlay()
{
    for (auto plot : plotWidgets)
    {
        plot->flashSnapshotOverlay(darkBackgroundAction.isChecked());
    }
}

void PlotManager::onNumOfSamplesChanged(unsigned value)
{
    for (auto plot : plotWidgets)
    {
        plot->onNumOfSamplesChanged(value);
    }
}

void PlotManager::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    settings->setValue(SG_Plot_DarkBackground, darkBackgroundAction.isChecked());
    settings->setValue(SG_Plot_Grid, showGridAction.isChecked());
    settings->setValue(SG_Plot_MinorGrid, showMinorGridAction.isChecked());
    settings->setValue(SG_Plot_Legend, showLegendAction.isChecked());
    settings->setValue(SG_Plot_MultiPlot, showMultiAction.isChecked());
    settings->endGroup();
}

void PlotManager::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    darkBackgroundAction.setChecked(
        settings->value(SG_Plot_DarkBackground, darkBackgroundAction.isChecked()).toBool());
    darkBackground(darkBackgroundAction.isChecked());
    showGridAction.setChecked(
        settings->value(SG_Plot_Grid, showGridAction.isChecked()).toBool());
    showGrid(showGridAction.isChecked());
    showMinorGridAction.setChecked(
        settings->value(SG_Plot_MinorGrid, showMinorGridAction.isChecked()).toBool());
    showMinorGridAction.setEnabled(showGridAction.isChecked());
    showMinorGrid(showMinorGridAction.isChecked());
    showLegendAction.setChecked(
        settings->value(SG_Plot_Legend, showLegendAction.isChecked()).toBool());
    showLegend(showLegendAction.isChecked());
    showMultiAction.setChecked(
        settings->value(SG_Plot_MultiPlot, showMultiAction.isChecked()).toBool());
    setMulti(showMultiAction.isChecked());
    settings->endGroup();
}
