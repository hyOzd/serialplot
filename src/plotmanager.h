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

#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QSplitter>

#include <qwt_plot_curve.h>
#include "plot.h"
#include "barplot.h"
#include "framebufferseries.h"
#include "channelmanager.h"
#include "channelinfomodel.h"

struct PlotViewSettings
{
    bool showGrid;
    bool showMinorGrid;
    bool darkBackground;
    bool showLegend;
    bool showMulti;
    Plot::ShowSymbols showSymbols;
};

class PlotManager : public QObject
{
    Q_OBJECT

public:
    explicit PlotManager(
        QWidget* plotArea, QSplitter* splitter = NULL,
        ChannelManager* channelMan = NULL, ChannelInfoModel* infoModel = NULL,
        QObject *parent = 0);
    ~PlotManager();
    /// Add a new curve with title and buffer. A color is
    /// automatically chosen for curve.
    void addCurve(QString title, FrameBuffer* buffer);
    /// Alternative of `addCurve` for static curve data (snapshots).
    void addCurve(QString title, QVector<QPointF> data);
    /// Set the displayed title for a curve
    void setTitle(unsigned index, QString title);
    /// Removes curves from the end
    void removeCurves(unsigned number);
    /// Returns current number of curves known by plot manager
    unsigned numOfCurves();
    /// Returns the list of actions to be inserted into the `View` menu
    QList<QAction*> menuActions();
    /// Returns current status of menu actions
    PlotViewSettings viewSettings() const;
    /// Set the current state of view
    void setViewSettings(const PlotViewSettings& settings);
    /// Stores plot settings into a `QSettings`.
    void saveSettings(QSettings* settings);
    /// Loads plot settings from a `QSettings`.
    void loadSettings(QSettings* settings);

public slots:
    /// Update all plot widgets
    void replot();
    /// Enable display of a "DEMO" label on each plot
    void showDemoIndicator(bool show = true);
    /// Set the Y axis
    void setYAxis(bool autoScaled, double yMin = 0, double yMax = 1);
    /// Set the X axis
    void setXAxis(bool asIndex, double xMin = 0 , double xMax = 1);
    /// Display an animation for snapshot
    void flashSnapshotOverlay();
    /// Should be called to update zoom base
    void setNumOfSamples(unsigned value);
    /// Maximum width of X axis (limit of hscroll)
    void setPlotWidth(double width);

private:
    bool isMulti;
    QWidget* _plotArea;
    BarPlot* barPlot;
    QSplitter* _splitter;
    QVBoxLayout* layout; ///< layout of the `plotArea`
    QScrollArea* scrollArea;
    QList<QwtPlotCurve*> curves;
    QList<Plot*> plotWidgets;
    Plot* emptyPlot;  ///< for displaying when all channels are hidden
    ChannelManager* _channelMan;
    ChannelInfoModel* _infoModel;
    bool isDemoShown;
    bool _autoScaled;
    double _yMin;
    double _yMax;
    bool _xAxisAsIndex;
    double _xMin;
    double _xMax;
    unsigned _numOfSamples;
    double _plotWidth;
    Plot::ShowSymbols showSymbols;

    // menu actions
    QAction showGridAction;
    QAction showMinorGridAction;
    QAction unzoomAction;
    QAction darkBackgroundAction;
    QAction showLegendAction;
    QAction showMultiAction;
    QAction showBarPlotAction;
    QAction setSymbolsAction;
    QMenu setSymbolsMenu;
    QAction* setSymbolsAutoAct;
    QAction* setSymbolsShowAct;
    QAction* setSymbolsHideAct;

    void setupLayout(bool multiPlot);
    /// Inserts a new plot widget to the current layout.
    Plot* addPlotWidget();
    /// Returns the plot widget that given curve is attached to
    Plot* plotWidget(unsigned curveIndex);
    /// Common part of overloaded `addCurve` functions
    void _addCurve(QwtPlotCurve* curve);
    void setSymbols(Plot::ShowSymbols shown);
    /// Check and make sure "no visible channels" text is shown
    void checkNoVisChannels();

private slots:
    void showGrid(bool show = true);
    void showMinorGrid(bool show = true);
    void showLegend(bool show = true);
    void unzoom();
    void darkBackground(bool enabled = true);
    /// Enable/Disable multiple plot display
    void setMulti(bool enabled);
    /// Display/Hide bar plot
    void showBarPlot(bool show = true);

    void onChannelInfoChanged(const QModelIndex & topLeft,
                              const QModelIndex & bottomRight,
                              const QVector<int> & roles = QVector<int> ());
};

#endif // PLOTMANAGER_H
