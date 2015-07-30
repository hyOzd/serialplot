#include "hidabletabwidget.h"
#include <QTabBar>
#include <QToolButton>

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
    connect(this, SIGNAL(tabBarClicked(int)), this, SLOT(onTabBarClicked()));
}

void HidableTabWidget::onHideAction(bool checked)
{
    if (checked)
    {
        this->setMaximumHeight(this->tabBar()->height());
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }
    else
    {
        this->setMaximumHeight(100000); // just a very big number
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

void HidableTabWidget::onTabBarClicked()
{
    hideAction.setChecked(false);
}
