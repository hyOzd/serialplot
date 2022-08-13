/*
  Copyright © 2022 Hasan Yavuz Özderya

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
#include <QMenu>

#include <qwt_plot_curve.h>
#include "plot.h"
#include "framebufferseries.h"
#include "stream.h"
#include "snapshot.h"
#include "plotmenu.h"

class PlotManager : public QObject
{
    Q_OBJECT

public:
    explicit PlotManager(QWidget* plotArea, PlotMenu* menu,
                         const Stream* stream = nullptr,
                         QObject *parent = 0);
    explicit PlotManager(QWidget* plotArea, PlotMenu* menu,
                         Snapshot* snapshot,
                         QObject *parent = 0);
    ~PlotManager();
    /// Add a new curve with title and buffer.
    void addCurve(QString title, const XFrameBuffer* xBuf, const FrameBuffer* yBuf);
    /// Removes curves from the end
    void removeCurves(unsigned number);
    /// Returns current number of curves known by plot manager
    unsigned numOfCurves();
    /// export SVG
    void exportSvg (QString fileName) const;

public slots:
    /// Enable/Disable multiple plot display
    void setMulti(bool enabled);
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
    PlotMenu* _menu;
    QVBoxLayout* layout; ///< layout of the `plotArea`
    QScrollArea* scrollArea;
    QList<QwtPlotCurve*> curves;
    QList<Plot*> plotWidgets;
    Plot* emptyPlot;  ///< for displaying when all channels are hidden
    const Stream* _stream;       ///< attached stream, can be `nullptr`
    const ChannelInfoModel* infoModel;
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
    bool inScaleSync; ///< scaleSync is in progress

    /// Common constructor
    void construct(QWidget* plotArea, PlotMenu* menu);
    /// Setups the layout for multi or single plot
    void setupLayout(bool multiPlot);
    /// Inserts a new plot widget to the current layout.
    Plot* addPlotWidget();
    /// Returns the plot widget that given curve is attached to
    Plot* plotWidget(unsigned curveIndex);
    /// Common part of overloaded `addCurve` functions
    void _addCurve(QwtPlotCurve* curve);
    /// Check and make sure "no visible channels" text is shown
    void checkNoVisChannels();

private slots:
    void showGrid(bool show = true);
    void showMinorGrid(bool show = true);
    void showLegend(bool show = true);
    void setLegendPosition(Qt::AlignmentFlag alignment);
    void unzoom();
    void darkBackground(bool enabled = true);
    void setSymbols(Plot::ShowSymbols shown);

    void onNumChannelsChanged(unsigned value);
    void onChannelInfoChanged(const QModelIndex & topLeft,
                              const QModelIndex & bottomRight,
                              const QVector<int> & roles = QVector<int> ());

    /// Synchronize Y axes to be the same width (so that X axes are in line)
    void syncScales();
};

#endif // PLOTMANAGER_H
