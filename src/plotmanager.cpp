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

#include <algorithm>
#include <QActionGroup>
#include <QMetaEnum>
#include <QtDebug>
#include "qwt_symbol.h"

#include "plot.h"
#include "plotmanager.h"
#include "utils.h"
#include "setting_defines.h"

PlotManager::PlotManager(
    QWidget* plotArea, QSplitter* splitter, ChannelManager* channelMan,
    ChannelInfoModel* infoModel, QObject *parent) :
    QObject(parent),
    _plotArea(plotArea),
    _splitter(splitter),
    showGridAction("&Grid", this),
    showMinorGridAction("&Minor Grid", this),
    unzoomAction("&Unzoom", this),
    darkBackgroundAction("&Dark Background", this),
    showLegendAction("&Legend", this),
    showMultiAction("Multi &Plot", this),
    showBarPlotAction("&Bar Plot", this),
    setSymbolsAction("Symbols", this)
{
    _autoScaled = true;
    _yMin = 0;
    _yMax = 1;
    _xAxisAsIndex = true;
    isDemoShown = false;
    _channelMan = channelMan;
    _infoModel = infoModel;
    _numOfSamples = 1;
    _plotWidth = 1;
    showSymbols = Plot::ShowSymbolsAuto;
    emptyPlot = NULL;
    barPlot = NULL;

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
    showBarPlotAction.setToolTip("Show bar plot");
    setSymbolsAction.setToolTip("Show/Hide symbols");

    showGridAction.setShortcut(QKeySequence("G"));
    showMinorGridAction.setShortcut(QKeySequence("M"));

    showGridAction.setCheckable(true);
    showMinorGridAction.setCheckable(true);
    darkBackgroundAction.setCheckable(true);
    showLegendAction.setCheckable(true);
    showMultiAction.setCheckable(true);
    showBarPlotAction.setCheckable(true);

    showGridAction.setChecked(false);
    showMinorGridAction.setChecked(false);
    darkBackgroundAction.setChecked(false);
    showLegendAction.setChecked(true);
    showMultiAction.setChecked(false);
    showBarPlotAction.setChecked(false);

    showMinorGridAction.setEnabled(false);

    // setup symbols menu
    setSymbolsAutoAct = setSymbolsMenu.addAction("Show When Zoomed");
    setSymbolsAutoAct->setCheckable(true);
    setSymbolsAutoAct->setChecked(true);
    connect(setSymbolsAutoAct, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            [this](bool checked)
            {
                if (checked) setSymbols(Plot::ShowSymbolsAuto);
            });
    setSymbolsShowAct = setSymbolsMenu.addAction("Always Show");
    setSymbolsShowAct->setCheckable(true);
    connect(setSymbolsShowAct, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            [this](bool checked)
            {
                if (checked) setSymbols(Plot::ShowSymbolsShow);
            });
    setSymbolsHideAct = setSymbolsMenu.addAction("Always Hide");
    setSymbolsHideAct->setCheckable(true);
    connect(setSymbolsHideAct, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            [this](bool checked)
            {
                if (checked) setSymbols(Plot::ShowSymbolsHide);
            });
    setSymbolsAction.setMenu(&setSymbolsMenu);

    // add symbol actions to same group so that they appear as radio buttons
    auto group = new QActionGroup(this);
    group->addAction(setSymbolsAutoAct);
    group->addAction(setSymbolsShowAct);
    group->addAction(setSymbolsHideAct);

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
    connect(&showBarPlotAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            this, &PlotManager::showBarPlot);

    // connect to channel info model
    if (_infoModel != NULL)     // TODO: remove when snapshots have infomodel
    {
        connect(_infoModel, &QAbstractItemModel::dataChanged,
                this, &PlotManager::onChannelInfoChanged);

        connect(_infoModel, &QAbstractItemModel::modelReset,
                [this]()
                {
                    onChannelInfoChanged(_infoModel->index(0, 0), // start
                                         _infoModel->index(_infoModel->rowCount()-1, 0), // end
                                         {}); // roles ignored
                });
    }
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
    if (emptyPlot != NULL) delete emptyPlot;
}

void PlotManager::onChannelInfoChanged(const QModelIndex &topLeft,
                                       const QModelIndex &bottomRight,
                                       const QVector<int> &roles)
{
    int start = topLeft.row();
    int end = bottomRight.row();

    for (int ci = start; ci <= end; ci++)
    {
        QString name = topLeft.sibling(ci, ChannelInfoModel::COLUMN_NAME).data(Qt::EditRole).toString();
        QColor color = topLeft.sibling(ci, ChannelInfoModel::COLUMN_NAME).data(Qt::ForegroundRole).value<QColor>();
        bool visible = topLeft.sibling(ci, ChannelInfoModel::COLUMN_VISIBILITY).data(Qt::CheckStateRole).toBool();

        curves[ci]->setTitle(name);
        curves[ci]->setPen(color);
        curves[ci]->setVisible(visible);
        curves[ci]->setItemAttribute(QwtPlotItem::Legend, visible);

        // replot only updated widgets
        if (isMulti)
        {
            plotWidgets[ci]->updateSymbols(); // required for color change
            plotWidgets[ci]->updateLegend(curves[ci]);
            plotWidgets[ci]->setVisible(visible);
            if (visible)
            {
                plotWidgets[ci]->replot();
            }
        }
    }

    checkNoVisChannels();

    // replot single widget
    if (!isMulti)
    {
        plotWidgets[0]->updateSymbols();
        plotWidgets[0]->updateLegend();
        replot();
    }
}

void PlotManager::checkNoVisChannels()
{
    // if all channels are hidden show indicator
    bool allhidden = std::none_of(curves.cbegin(), curves.cend(),
                                  [](QwtPlotCurve* c) {return c->isVisible();});

    plotWidgets[0]->showNoChannel(allhidden);
    if (isMulti)
    {
        plotWidgets[0]->showNoChannel(allhidden);
        plotWidgets[0]->setVisible(true);
    }
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
            auto plot = addPlotWidget();
            plot->setVisible(curve->isVisible());
            curve->attach(plot);
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

    checkNoVisChannels();
}

void PlotManager::showBarPlot(bool show)
{
    // A secondary plot can only be shown if splitter is provided.
    Q_ASSERT(_splitter != NULL);

    if (show)
    {
        Q_ASSERT(barPlot == NULL);

        barPlot = new BarPlot(_channelMan, _splitter);
        barPlot->darkBackground(darkBackgroundAction.isChecked());
    }
    else
    {
        Q_ASSERT(barPlot != NULL);

        delete barPlot;
        barPlot = NULL;
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
    plot->setYAxis(_autoScaled, _yMin, _yMax);
    plot->setNumOfSamples(_numOfSamples);
    plot->setSymbols(showSymbols);

    plot->setPlotWidth(_plotWidth);
    if (_xAxisAsIndex)
    {
        plot->setXAxis(0, _numOfSamples);
    }
    else
    {
        plot->setXAxis(_xMin, _xMax);
    }

    return plot;
}

void PlotManager::addCurve(QString title, FrameBuffer* buffer)
{
    auto curve = new QwtPlotCurve(title);
    auto series = new FrameBufferSeries(buffer);
    series->setXAxis(_xAxisAsIndex, _xMin, _xMax);
    curve->setSamples(series);
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
    auto color = _infoModel->color(index);
    curve->setPen(color);

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
    actions << &showBarPlotAction;
    actions << &setSymbolsAction;
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
    if (barPlot != NULL) barPlot->darkBackground(enabled);
}

void PlotManager::setSymbols(Plot::ShowSymbols shown)
{
    showSymbols = shown;
    for (auto plot : plotWidgets)
    {
        plot->setSymbols(shown);
    }
}

void PlotManager::setYAxis(bool autoScaled, double yAxisMin, double yAxisMax)
{
    _autoScaled = autoScaled;
    _yMin = yAxisMin;
    _yMax = yAxisMax;
    for (auto plot : plotWidgets)
    {
        plot->setYAxis(autoScaled, yAxisMin, yAxisMax);
    }
}

void PlotManager::setXAxis(bool asIndex, double xMin, double xMax)
{
    _xAxisAsIndex = asIndex;
    _xMin = xMin;
    _xMax = xMax;
    for (auto curve : curves)
    {
        // TODO: what happens when addCurve(QVector) is used?
        FrameBufferSeries* series = static_cast<FrameBufferSeries*>(curve->data());
        series->setXAxis(asIndex, xMin, xMax);
    }
    for (auto plot : plotWidgets)
    {
        if (asIndex)
        {
            plot->setXAxis(0, _numOfSamples);
        }
        else
        {
            plot->setXAxis(xMin, xMax);
        }
    }
    replot();
}

void PlotManager::flashSnapshotOverlay()
{
    for (auto plot : plotWidgets)
    {
        plot->flashSnapshotOverlay(darkBackgroundAction.isChecked());
    }
}

void PlotManager::setNumOfSamples(unsigned value)
{
    _numOfSamples = value;
    for (auto plot : plotWidgets)
    {
        plot->setNumOfSamples(value);
        if (_xAxisAsIndex) plot->setXAxis(0, value);
    }
}

void PlotManager::setPlotWidth(double width)
{
    _plotWidth = width;
    for (auto plot : plotWidgets)
    {
        plot->setPlotWidth(width);
    }
}

PlotViewSettings PlotManager::viewSettings() const
{
    return PlotViewSettings(
        {
            showGridAction.isChecked(),
            showMinorGridAction.isChecked(),
            darkBackgroundAction.isChecked(),
            showLegendAction.isChecked(),
            showMultiAction.isChecked(),
            showSymbols
        });
}

void PlotManager::setViewSettings(const PlotViewSettings& settings)
{
    showGridAction.setChecked(settings.showGrid);
    showGrid(settings.showGrid);
    showMinorGridAction.setChecked(settings.showMinorGrid);
    showMinorGrid(settings.showMinorGrid);
    darkBackgroundAction.setChecked(settings.darkBackground);
    darkBackground(settings.darkBackground);
    showLegendAction.setChecked(settings.showLegend);
    showLegend(settings.showLegend);
    showMultiAction.setChecked(settings.showMulti);
    setMulti(settings.showMulti);

    setSymbols(settings.showSymbols);
    if (showSymbols == Plot::ShowSymbolsAuto)
    {
        setSymbolsAutoAct->setChecked(true);
    }
    else if (showSymbols == Plot::ShowSymbolsShow)
    {
        setSymbolsShowAct->setChecked(true);
    }
    else
    {
        setSymbolsHideAct->setChecked(true);
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

    QString showSymbolsStr;
    if (showSymbols == Plot::ShowSymbolsAuto)
    {
        showSymbolsStr = "auto";
    }
    else if (showSymbols == Plot::ShowSymbolsShow)
    {
        showSymbolsStr = "show";
    }
    else
    {
        showSymbolsStr = "hide";
    }
    settings->setValue(SG_Plot_Symbols, showSymbolsStr);

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

    QString showSymbolsStr = settings->value(SG_Plot_Symbols, QString()).toString();
    if (showSymbolsStr == "auto")
    {
        setSymbols(Plot::ShowSymbolsAuto);
        setSymbolsAutoAct->setChecked(true);
    }
    else if (showSymbolsStr == "show")
    {
        setSymbols(Plot::ShowSymbolsShow);
        setSymbolsShowAct->setChecked(true);
    }
    else if (showSymbolsStr == "hide")
    {
        setSymbols(Plot::ShowSymbolsHide);
        setSymbolsHideAct->setChecked(true);
    }
    else
    {
        qCritical() << "Invalid symbol setting:" << showSymbolsStr;
    }

    settings->endGroup();
}
