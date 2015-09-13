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
#include <QPainter>
#include <qwt_scale_widget.h>
#include <qwt_scale_map.h>
#include <math.h>

#include "scalepicker.h"

// minimum size for pick (in pixels)
#define MIN_PICK_SIZE (2)

// TODO: rename to PlotOverlay
class ScalePickerOverlay : public QwtWidgetOverlay
{
public:
    ScalePickerOverlay(QWidget* widget, ScalePicker* picker);

protected:
    virtual void drawOverlay(QPainter*) const;

private:
    ScalePicker* _picker;
};

ScalePickerOverlay::ScalePickerOverlay(QWidget* widget, ScalePicker* picker) :
    QwtWidgetOverlay(widget)
{
    _picker = picker;
}

void ScalePickerOverlay::drawOverlay(QPainter* painter) const
{
    _picker->drawOverlay(painter);
}

class ScaleOverlay : public QwtWidgetOverlay
{
public:
    ScaleOverlay(QWidget* widget, ScalePicker* picker);

protected:
    virtual void drawOverlay(QPainter*) const;

private:
    ScalePicker* _picker;
};

ScaleOverlay::ScaleOverlay(QWidget* widget, ScalePicker* picker) :
    QwtWidgetOverlay(widget)
{
    _picker = picker;
}

void ScaleOverlay::drawOverlay(QPainter* painter) const
{
    _picker->drawScaleOverlay(painter);
}

ScalePicker::ScalePicker(QwtScaleWidget* scaleWidget, QWidget* canvas) :
    QObject(scaleWidget)
{
    _scaleWidget = scaleWidget;
    _canvas = canvas;
    scaleWidget->installEventFilter(this);
    scaleWidget->setMouseTracking(true);
    pickerOverlay = new ScalePickerOverlay(canvas, this);
    scaleOverlay = new ScaleOverlay(scaleWidget, this);
    started = false;
    pressed = false;
}

bool ScalePicker::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = (QMouseEvent*) event;
        double pos = this->position(mouseEvent);
        double posPx = this->positionPx(mouseEvent);
        currentPosPx = posPx;

        if (event->type() == QEvent::MouseButtonPress &&
            mouseEvent->button() == Qt::LeftButton)
        {
            pressed = true; // not yet started
            firstPos = pos;
            firstPosPx = posPx;
        }
        else if (event->type() == QEvent::MouseMove)
        {
            // make sure pick size is big enough, so that just
            // clicking won't trigger pick
            if (!started && pressed && (fabs(posPx-firstPosPx) > MIN_PICK_SIZE))
            {
                started = true;
                // pickerOverlay->updateOverlay();
                emit pickStarted(pos);
            }
            else if (started)
            {
                pickerOverlay->updateOverlay();
                emit picking(firstPos, pos);
            }
            scaleOverlay->updateOverlay();
        }
        else // event->type() == QEvent::MouseButtonRelease
        {
            if (started)
            {
                // finalize
                started = false;
                pressed = false;
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

void ScalePicker::drawOverlay(QPainter* painter)
{
    if (started)
    {
        painter->save();
        painter->setPen(_pen);

        QRect rect;
        if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
            _scaleWidget->alignment() == QwtScaleDraw::TopScale)
        {
            int height = painter->device()->height();
            rect = QRect(posCanvasPx(firstPosPx), 0, currentPosPx-firstPosPx, height);
        }
        else // vertical
        {
            int width = painter->device()->width();
            rect = QRect(0, posCanvasPx(firstPosPx), width, currentPosPx-firstPosPx);
        }
        painter->drawRect(rect);
        painter->restore();
    }
}

void ScalePicker::drawScaleOverlay(QPainter* painter)
{
    painter->save();
    painter->setPen(_pen);
    if (1)
    {

        if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
            _scaleWidget->alignment() == QwtScaleDraw::TopScale)
        {
            int height = painter->device()->height();
            if (started) painter->drawLine(firstPosPx, 0, firstPosPx, height);
            painter->drawLine(currentPosPx, 0, currentPosPx, height);
        }
        else // vertical
        {
            int width = painter->device()->width();
            if (started) painter->drawLine(0, firstPosPx, width, firstPosPx);
            painter->drawLine(0, currentPosPx, width, currentPosPx);
        }
    }
    painter->restore();
}

void ScalePicker::setPen(QPen pen)
{
    _pen = pen;
}

double ScalePicker::position(QMouseEvent* mouseEvent)
{
    double pos;
    pos = positionPx(mouseEvent);
    // convert the position of the click to the plot coordinates
    pos = _scaleWidget->scaleDraw()->scaleMap().invTransform(pos);
    return pos;
}

double ScalePicker::positionPx(QMouseEvent* mouseEvent)
{
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
    return pos;
}

/*
 * Scale widget and canvas widget is not always aligned. Especially
 * when zooming scaleWidget moves around. This causes irregularities
 * when drawing the tracker lines. This function maps scale widgets
 * pixel coordinate to canvas' coordinate.
 */
double ScalePicker::posCanvasPx(double pos)
{
    // assumption: scale.width < canvas.width && scale.x > canvas.x
    if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
        _scaleWidget->alignment() == QwtScaleDraw::TopScale)
    {
        return pos + (_scaleWidget->x() - _canvas->x());
    }
    else // left or right scale
    {
        return pos + (_scaleWidget->y() - _canvas->y());
    }
    return pos;
}
