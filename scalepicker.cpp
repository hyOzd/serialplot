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

#include <QEvent>
#include <QMouseEvent>
#include <qwt_scale_widget.h>
#include <qwt_scale_map.h>
#include <QtDebug>

#include "scalepicker.h"

ScalePicker::ScalePicker(QwtScaleWidget* scaleWidget) :
    QObject(scaleWidget)
{
    scaleWidget->installEventFilter(this);
    _scaleWidget = scaleWidget;
    started = false;
}

bool ScalePicker::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = (QMouseEvent*) event;
        double pos = this->position(mouseEvent);

        if (event->type() == QEvent::MouseButtonPress)
        {
            started = true;
            firstPos = pos;
            qDebug() << "Pick started:" << firstPos;
            emit pickStarted(pos);
        }
        else if (event->type() == QEvent::MouseMove)
        {
            if (started)
            {
                emit picking(firstPos, pos);
            }
        }
        else // event->type() == QEvent::MouseButtonRelease
        {
            if (started)
            {
                started = false;
                qDebug() << "Picked:" << firstPos << pos;
                emit picked(firstPos, pos);
            }
        }
        return true;
    }
    else
    {
        return QObject::eventFilter(object, event);
    }
}

double ScalePicker::position(QMouseEvent* mouseEvent)
{
    // capture and convert the position of the click to the plot coordinates
    double pos;
    if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
        _scaleWidget->alignment() == QwtScaleDraw::TopScale)
    {
        pos = mouseEvent->pos().x();
    }
    else // left or right scale
    {
        pos = mouseEvent->pos().y();
    }
    pos = _scaleWidget->scaleDraw()->scaleMap().invTransform(pos);
    return pos;
}
