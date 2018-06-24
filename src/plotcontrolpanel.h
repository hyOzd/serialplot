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

#ifndef PLOTCONTROLPANEL_H
#define PLOTCONTROLPANEL_H

#include <QWidget>
#include <QSettings>
#include <QAction>
#include <QMenu>

#include "channelinfomodel.h"

namespace Ui {
class PlotControlPanel;
}

class PlotControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PlotControlPanel(QWidget *parent = 0);
    ~PlotControlPanel();

    unsigned numOfSamples();
    bool   autoScale() const;
    double yMax() const;
    double yMin() const;
    bool   xAxisAsIndex() const;
    double xMax() const;
    double xMin() const;
    /// Returns the plot width adjusted for x axis scaling.
    double plotWidth() const;

    void setChannelInfoModel(ChannelInfoModel* model);

    /// Stores plot settings into a `QSettings`
    void saveSettings(QSettings* settings);
    /// Loads plot settings from a `QSettings`.
    void loadSettings(QSettings* settings);

signals:
    void numOfSamplesChanged(int value);
    void yScaleChanged(bool autoScaled, double yMin = 0, double yMax = 1);
    void xScaleChanged(bool asIndex, double xMin = 0, double xMax = 1);
    void plotWidthChanged(double width);

private:
    Ui::PlotControlPanel *ui;

    /// Holds 'number of samples' after the confirmation
    unsigned _numOfSamples;
    /// User can disable this setting in the checkbox
    bool warnNumOfSamples;

    QAction resetAct, resetNamesAct, resetColorsAct, showAllAct, hideAllAct;
    QMenu resetMenu;

    /// Show a confirmation dialog before setting #samples to a big value
    bool askNSConfirmation(int value);

private slots:
    void onNumOfSamples(int value);
    void onAutoScaleChecked(bool checked);
    void onYScaleChanged();
    void onRangeSelected();
    void onIndexChecked(bool checked);
    void onXScaleChanged();
    void onPlotWidthChanged();
};

#endif // PLOTCONTROLPANEL_H
