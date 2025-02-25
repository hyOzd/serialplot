/*
  Copyright © 2025 Hasan Yavuz Özderya

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

#ifndef PLOTMENU_H
#define PLOTMENU_H

#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QSettings>

#include "plot.h"

/// Used to quickly transfer view options between different menus
struct PlotViewSettings
{
    bool showGrid;
    bool showMinorGrid;
    bool darkBackground;
    bool showLegend;
    bool showMulti;
    Plot::ShowSymbols showSymbols;
};

class PlotMenu : public QMenu
{
    Q_OBJECT

public:
    PlotMenu(QWidget* parent = 0);
    PlotMenu(PlotViewSettings s, QWidget* parent = 0);

    QAction showGridAction;
    QAction showMinorGridAction;
    QAction unzoomAction;
    QAction darkBackgroundAction;
    QAction showLegendAction;
    QAction showMultiAction;

    /// Returns a bundle of current view settings (menu selections)
    PlotViewSettings viewSettings() const;
    /// Selected "show symbol" option
    Plot::ShowSymbols showSymbols() const;
    /// Return selected legend position as Qt alignment enum
    Qt::AlignmentFlag legendPosition() const;
    /// Stores plot settings into a `QSettings`.
    void saveSettings(QSettings* settings);
    /// Loads plot settings from a `QSettings`.
    void loadSettings(QSettings* settings);

private:
    // Symbol Menu
    QAction setSymbolsAction;
    QMenu setSymbolsMenu;
    QAction setSymbolsAutoAct;
    QAction setSymbolsShowAct;
    QAction setSymbolsHideAct;

    // Legend position menu
    QAction setLegendPosAction;
    QMenu setLegendPosMenu;
    QActionGroup legendPosGrp;
    QAction setLegendTopLeftAct;
    QAction setLegendTopRightAct;
    QAction setLegendBottomRightAct;
    QAction setLegendBottomLeftAct;

signals:
    void symbolShowChanged(Plot::ShowSymbols shown);
    void legendPosChanged(Qt::AlignmentFlag alignment);
};

#endif // PLOTMENU_H
