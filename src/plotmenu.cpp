/*
  Copyright © 2019 Hasan Yavuz Özderya

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

#include <QDebug>
#include "plotmenu.h"
#include "setting_defines.h"
#include "utils.h"

PlotMenu::PlotMenu(QWidget* parent) :
    QMenu(tr("&View"), parent),
    showGridAction("&Grid", this),
    showMinorGridAction("&Minor Grid", this),
    unzoomAction("&Unzoom", this),
    darkBackgroundAction("&Dark Background", this),
    showLegendAction("&Legend", this),
    showMultiAction("Multi &Plot", this),
    setSymbolsAction("&Symbols", this),
    setSymbolsAutoAct("Show When &Zoomed", this),
    setSymbolsShowAct("Always &Show", this),
    setSymbolsHideAct("Always &Hide", this)
{
    showGridAction.setToolTip("Show Grid");
    showMinorGridAction.setToolTip("Show Minor Grid");
    unzoomAction.setToolTip("Unzoom the Plot");
    darkBackgroundAction.setToolTip("Enable Dark Plot Background");
    showLegendAction.setToolTip("Display the Legend on Plot");
    showMultiAction.setToolTip("Display All Channels Separately");
    setSymbolsAction.setToolTip("Show/Hide symbols");

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

    // minor grid is only enabled when _major_ grid is enabled
    showMinorGridAction.setEnabled(false);
    connect(&showGridAction, SELECT<bool>::OVERLOAD_OF(&QAction::triggered),
            &showMinorGridAction, &QAction::setEnabled);

    // setup set symbols menu
    setSymbolsMenu.addAction(&setSymbolsAutoAct);
    setSymbolsAutoAct.setCheckable(true);
    setSymbolsAutoAct.setChecked(true);
    connect(&setSymbolsAutoAct, SELECT<bool>::OVERLOAD_OF(&QAction::toggled),
            [this](bool checked)
            {
                if (checked) emit symbolShowChanged(Plot::ShowSymbolsAuto);
            });

    setSymbolsMenu.addAction(&setSymbolsShowAct);
    setSymbolsShowAct.setCheckable(true);
    connect(&setSymbolsShowAct, SELECT<bool>::OVERLOAD_OF(&QAction::toggled),
            [this](bool checked)
            {
                if (checked) emit symbolShowChanged(Plot::ShowSymbolsShow);
            });

    setSymbolsMenu.addAction(&setSymbolsHideAct);
    setSymbolsHideAct.setCheckable(true);
    connect(&setSymbolsHideAct, SELECT<bool>::OVERLOAD_OF(&QAction::toggled),
            [this](bool checked)
            {
                if (checked) emit symbolShowChanged(Plot::ShowSymbolsHide);
            });

    // add symbol actions to same group so that they appear as radio buttons
    auto group = new QActionGroup(this);
    group->addAction(&setSymbolsAutoAct);
    group->addAction(&setSymbolsShowAct);
    group->addAction(&setSymbolsHideAct);

    setSymbolsAction.setMenu(&setSymbolsMenu);

    // add all actions to create this menu
    addAction(&showGridAction);
    addAction(&showMinorGridAction);
    addAction(&unzoomAction);
    addAction(&darkBackgroundAction);
    addAction(&showLegendAction);
    addAction(&showMultiAction);
    addAction(&setSymbolsAction);
}

PlotMenu::PlotMenu(PlotViewSettings s, QWidget* parent) :
    PlotMenu(parent)
{
    showGridAction.setChecked(s.showGrid);
    showMinorGridAction.setChecked(s.showMinorGrid);
    darkBackgroundAction.setChecked(s.darkBackground);
    showLegendAction.setChecked(s.showLegend);
    showMultiAction.setChecked(s.showMulti);
    switch (s.showSymbols)
    {
        case Plot::ShowSymbolsAuto:
             setSymbolsAutoAct.setChecked(true);
            break;
        case Plot::ShowSymbolsShow:
            setSymbolsShowAct.setChecked(true);
            break;
        case Plot::ShowSymbolsHide:
            setSymbolsHideAct.setChecked(true);
            break;
    }
}

PlotViewSettings PlotMenu::viewSettings() const
{
    return PlotViewSettings(
        {
            showGridAction.isChecked(),
            showMinorGridAction.isChecked(),
            darkBackgroundAction.isChecked(),
            showLegendAction.isChecked(),
            showMultiAction.isChecked(),
            showSymbols()
        });
}

Plot::ShowSymbols PlotMenu::showSymbols() const
{
    if (setSymbolsAutoAct.isChecked())
    {
        return Plot::ShowSymbolsAuto;
    }
    else if (setSymbolsShowAct.isChecked())
    {
        return Plot::ShowSymbolsShow;
    }
    else // setSymbolsHideAct.isChecked()
    {
        return Plot::ShowSymbolsHide;
    }
}

void PlotMenu::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    settings->setValue(SG_Plot_DarkBackground, darkBackgroundAction.isChecked());
    settings->setValue(SG_Plot_Grid, showGridAction.isChecked());
    settings->setValue(SG_Plot_MinorGrid, showMinorGridAction.isChecked());
    settings->setValue(SG_Plot_Legend, showLegendAction.isChecked());
    settings->setValue(SG_Plot_MultiPlot, showMultiAction.isChecked());

    QString showSymbolsStr;
    if (showSymbols() == Plot::ShowSymbolsAuto)
    {
        showSymbolsStr = "auto";
    }
    else if (showSymbols() == Plot::ShowSymbolsShow)
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

void PlotMenu::loadSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Plot);
    darkBackgroundAction.setChecked(
        settings->value(SG_Plot_DarkBackground, darkBackgroundAction.isChecked()).toBool());
    showGridAction.setChecked(
        settings->value(SG_Plot_Grid, showGridAction.isChecked()).toBool());
    showMinorGridAction.setChecked(
        settings->value(SG_Plot_MinorGrid, showMinorGridAction.isChecked()).toBool());
    showMinorGridAction.setEnabled(showGridAction.isChecked());
    showLegendAction.setChecked(
        settings->value(SG_Plot_Legend, showLegendAction.isChecked()).toBool());
    showMultiAction.setChecked(
        settings->value(SG_Plot_MultiPlot, showMultiAction.isChecked()).toBool());

    QString showSymbolsStr = settings->value(SG_Plot_Symbols, QString()).toString();
    if (showSymbolsStr == "auto")
    {
        setSymbolsAutoAct.setChecked(true);
    }
    else if (showSymbolsStr == "show")
    {
        setSymbolsShowAct.setChecked(true);
    }
    else if (showSymbolsStr == "hide")
    {
        setSymbolsHideAct.setChecked(true);
    }
    else if (!showSymbolsStr.isEmpty())
    {
        qCritical() << "Invalid symbol setting:" << showSymbolsStr;
    }

    settings->endGroup();
}
