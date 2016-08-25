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

#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>

#include <qwt_plot_curve.h>
#include "plot.h"
#include "framebufferseries.h"

class PlotManager : public QObject
{
    Q_OBJECT

public:
    explicit PlotManager(QWidget* plotArea, QObject *parent = 0);
    ~PlotManager();

    /// Add a new curve with title and buffer. A color is
    /// automatically chosen for curve.
    void addCurve(QString title, FrameBuffer* buffer);

    /// Set the displayed title for a curve
    void setTitle(unsigned index, QString title);

    /// Removes curves from the end
    void removeCurves(unsigned number);

    /// Returns current number of curves known by plot manager
    unsigned numOfCurves();

signals:

public slots:
    /// Enable/Disable multiple plot display
    void setMulti(bool enabled);

private:
    bool isMulti;
    QWidget* _plotArea;
    QVBoxLayout* layout; ///< layout of the `plotArea`
    QScrollArea* scrollArea;
    QList<QwtPlotCurve*> curves;
    QList<Plot*> plotWidgets;

    void setupLayout(bool multiPlot);
    Plot* addPlotWidget(); ///< inserts a new plot widget to the current layout
    /// Returns the plot widget that given curve is attached to
    Plot* plotWidget(unsigned curveIndex);
};

#endif // PLOTMANAGER_H
