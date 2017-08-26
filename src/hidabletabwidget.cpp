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

#include "hidabletabwidget.h"
#include <QTabBar>
#include <QToolButton>
#include <QSizePolicy>
#include <QTimer>

#define DOUBLE_CLICK_DELAY (200) // ms

HidableTabWidget::HidableTabWidget(QWidget *parent) :
    QTabWidget(parent),
    hideAction("▾", this)
{
    hideAction.setCheckable(true);
    hideAction.setToolTip("Hide Panels");
    QToolButton* hideButton = new QToolButton();
    hideButton->setDefaultAction(&hideAction);
    hideButton->setAutoRaise(true);
    this->setCornerWidget(hideButton);

    connect(&hideAction, SIGNAL(toggled(bool)), this, SLOT(onHideAction(bool)));
    connectSignals();
}

void HidableTabWidget::onHideAction(bool checked)
{
    if (checked) // hide
    {
        this->setMaximumHeight(this->tabBar()->height());
        disconnect(this, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(onTabBarDoubleClicked()));
        QTimer::singleShot(DOUBLE_CLICK_DELAY, this, SLOT(connectSignals()));
    }
    else // show
    {
        this->setMaximumHeight(100000); // just a very big number
        disconnect(this, SIGNAL(tabBarClicked(int)), this, SLOT(onTabBarClicked()));
        QTimer::singleShot(DOUBLE_CLICK_DELAY, this, SLOT(connectSignals()));
    }
}

void HidableTabWidget::onTabBarClicked()
{
    hideAction.setChecked(false);
}

void HidableTabWidget::onTabBarDoubleClicked()
{
    hideAction.setChecked(true);
}

void HidableTabWidget::connectSignals()
{
    if (hideAction.isChecked()) // hidden
    {
        connect(this, SIGNAL(tabBarClicked(int)), this, SLOT(onTabBarClicked()));
    }
    else // shown
    {
        connect(this, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(onTabBarDoubleClicked()));
    }
}

void HidableTabWidget::showTabs()
{
    hideAction.setChecked(false);
}
