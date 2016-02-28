/*
  Copyright © 2015 Hasan Yavuz Özderya

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

#include <QToolButton>
#include <QAction>
#include <QKeySequence>
#include <QHelpEvent>
#include <QToolTip>

#include "tooltipfilter.h"

bool ToolTipFilter::eventFilter(QObject *obj, QEvent *ev)
{
    if (ev->type() == QEvent::ToolTip && obj->inherits("QToolButton"))
    {
        // prepare tooltip message
        QToolButton* toolButton = (QToolButton*) obj;
        QAction* action = toolButton->defaultAction();

        QString toolTip;
        if (action != NULL)
        {
            toolTip = action->toolTip();
            QKeySequence keys = action->shortcut();
            if (!keys.isEmpty())
            {
                toolTip += QString(" <b>[") + keys.toString() + "]</b>";
            }
        }
        else
        {
            toolTip = toolButton->toolTip();
        }

        if (toolTip.isEmpty()) return false;

        // show tooltip message
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(ev);
        QToolTip::showText(helpEvent->globalPos(), toolTip);
        return true;
    }
    else
    {
        return QObject::eventFilter(obj, ev);
    }
}
