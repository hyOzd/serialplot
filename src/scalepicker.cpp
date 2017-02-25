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

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <qwt_scale_widget.h>
#include <qwt_scale_map.h>
#include <qwt_scale_div.h>
#include <qwt_text.h>
#include <math.h>

#include "scalepicker.h"

// minimum size for pick (in pixels)
#define MIN_PICK_SIZE (2)

#define SNAP_DISTANCE (5)

class PlotOverlay : public QwtWidgetOverlay
{
public:
    PlotOverlay(QWidget* widget, ScalePicker* picker);

protected:
    virtual void drawOverlay(QPainter*) const;

private:
    ScalePicker* _picker;
};

PlotOverlay::PlotOverlay(QWidget* widget, ScalePicker* picker) :
    QwtWidgetOverlay(widget)
{
    _picker = picker;
}

void PlotOverlay::drawOverlay(QPainter* painter) const
{
    _picker->drawPlotOverlay(painter);
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
    pickerOverlay = new PlotOverlay(canvas, this);
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
        updateSnapPoints();

        QMouseEvent* mouseEvent = (QMouseEvent*) event;
        int posPx = this->positionPx(mouseEvent);

        // do snapping unless Shift is pressed
        if (! (mouseEvent->modifiers() & Qt::ShiftModifier))
        {
            for (auto sp : snapPoints)
            {
                if (std::abs(posPx-sp) <= SNAP_DISTANCE)
                {
                    posPx = sp;
                    break;
                }
            }
        }

        double pos = this->position(posPx);
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
                emit pickStarted(pos);
            }
            else if (started)
            {
                emit picking(firstPos, pos);
            }
            pickerOverlay->updateOverlay();
            scaleOverlay->updateOverlay();
        }
        else // event->type() == QEvent::MouseButtonRelease
        {
            pressed = false;
            if (started)
            {
                // finalize
                started = false;
                emit picked(firstPos, pos);
            }
        }
        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        scaleOverlay->updateOverlay();
        pickerOverlay->updateOverlay();
        return true;
    }
    else
    {
        return QObject::eventFilter(object, event);
    }
}

const int TEXT_MARGIN = 4;

void ScalePicker::drawPlotOverlay(QPainter* painter)
{
    const double FILL_ALPHA = 0.2;

    painter->save();
    painter->setPen(_pen);

    if (started)
    {
        QColor color = _pen.color();
        color.setAlphaF(FILL_ALPHA);
        painter->setBrush(color);

        QRect rect;
        QwtText text = trackerText();
        auto tSize = text.textSize(painter->font());

        if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
            _scaleWidget->alignment() == QwtScaleDraw::TopScale)
        {
            int canvasHeight = painter->device()->height();
            int pickWidth = currentPosPx-firstPosPx;
            rect = QRect(posCanvasPx(firstPosPx), 0, pickWidth, canvasHeight);
        }
        else // vertical
        {
            int canvasWidth = painter->device()->width();
            int pickHeight = currentPosPx-firstPosPx;
            rect = QRect(0, posCanvasPx(firstPosPx), canvasWidth, pickHeight);
        }
        painter->drawRect(rect);
        text.draw(painter, pickTrackerTextRect(painter, rect, tSize));
    }
    else if (_scaleWidget->underMouse())
    {
        // draw tracker text centered on cursor
        QwtText text = trackerText();
        auto tsize = text.textSize(painter->font());
        text.draw(painter, trackerTextRect(painter, currentPosPx, tsize));
    }
    painter->restore();
}

QwtText ScalePicker::trackerText() const
{
    double pos;
    // use stored value if snapped to restore precision
    if (snapPointMap.contains(currentPosPx))
    {
        pos = snapPointMap[currentPosPx];
    }
    else
    {
        pos = position(currentPosPx);
    }

    return QwtText(QString("%1").arg(pos));
}

QRectF ScalePicker::trackerTextRect(QPainter* painter, int posPx, QSizeF textSize) const
{
    int canvasPosPx = posCanvasPx(posPx);
    QPointF topLeft;

    if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
        _scaleWidget->alignment() == QwtScaleDraw::TopScale)
    {
        int left = canvasPosPx - textSize.width() / 2;
        int canvasWidth = painter->device()->width();
        left = std::max(TEXT_MARGIN, left);
        left = std::min(double(left), canvasWidth - textSize.width() - TEXT_MARGIN);
        int top = 0;
        if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale)
        {
            top = painter->device()->height() - textSize.height();
        }
        topLeft = QPointF(left, top);
    }
    else                        // left/right scales
    {
        int top = canvasPosPx-textSize.height() / 2;
        int canvasHeight = painter->device()->height();
        top = std::max(0, top);
        top = std::min(double(top), canvasHeight - textSize.height());
        int left = TEXT_MARGIN;
        if (_scaleWidget->alignment() == QwtScaleDraw::RightScale)
        {
            left = painter->device()->width() - textSize.width();
        }
        topLeft = QPointF(left, top);
    }
    return QRectF(topLeft, textSize);
}

QRectF ScalePicker::pickTrackerTextRect(QPainter* painter, QRect pickRect, QSizeF textSize) const
{
    qreal left = 0;
    int pickLength = currentPosPx - firstPosPx;
    QPointF topLeft;

    if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale ||
        _scaleWidget->alignment() == QwtScaleDraw::TopScale)
    {
        int canvasWidth = painter->device()->width();

        if (pickLength > 0)
        {
            left = pickRect.right() + TEXT_MARGIN;
        }
        else
        {
            left = pickRect.right() - (textSize.width() + TEXT_MARGIN);
        }

        // make sure text is not off the canvas
        if (left < TEXT_MARGIN)
        {
            left = std::max(0, pickRect.right()) + TEXT_MARGIN;
        }
        else if (left + textSize.width() + TEXT_MARGIN > canvasWidth)
        {
            left = std::min(pickRect.right(), canvasWidth) - (textSize.width() + TEXT_MARGIN);
        }

        if (_scaleWidget->alignment() == QwtScaleDraw::BottomScale)
        {
            int canvasHeight = painter->device()->height();
            topLeft = QPointF(left, canvasHeight - textSize.height());
        }
        else                // top scale
        {
            topLeft = QPointF(left, 0);
        }
    }
    else                        // left/right scale
    {
        int canvasHeight = painter->device()->height();

        int top = 0;
        if (pickLength > 0)
        {
            top = pickRect.bottom();
        }
        else
        {
            top = pickRect.bottom() - textSize.height();
        }

        // make sure text is not off the canvas
        if (top < 0)
        {
            top = std::max(0, pickRect.bottom());
        }
        else if (top + textSize.height() > canvasHeight)
        {
            top = std::min(canvasHeight, pickRect.bottom()) - textSize.height();
        }

        if (_scaleWidget->alignment() == QwtScaleDraw::LeftScale)
        {
            topLeft = QPointF(TEXT_MARGIN, top);
        }
        else                    // right scale
        {
            int canvasWidth = painter->device()->width();
            topLeft = QPointF(canvasWidth - textSize.width() - TEXT_MARGIN, top);
        }
    }
    return QRectF(topLeft, textSize);
}

void ScalePicker::drawScaleOverlay(QPainter* painter)
{
    painter->save();

    // rotate & adjust coordinate system for vertical drawing
    if (_scaleWidget->alignment() == QwtScaleDraw::LeftScale ||
        _scaleWidget->alignment() == QwtScaleDraw::RightScale) // vertical
    {
        int width = painter->device()->width();
        painter->rotate(90);
        painter->translate(0, -width);
    }

    // draw the indicators
    if (started) drawTriangle(painter, firstPosPx);
    if (started || _scaleWidget->underMouse())
    {
        drawTriangle(painter, currentPosPx);
    }

    painter->restore();
}

void ScalePicker::drawTriangle(QPainter* painter, int position)
{
    const double tan60 = 1.732;
    const double trsize = 10;
    const int TRIANGLE_NUM_POINTS = 3;
    const int MARGIN = 2;
    const QPointF points[TRIANGLE_NUM_POINTS] =
        {
            {0, 0},
            {-trsize/tan60 , trsize},
            {trsize/tan60 , trsize}
        };

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(_scaleWidget->palette().windowText());
    painter->setRenderHint(QPainter::Antialiasing);

    painter->translate(position, MARGIN);
    painter->drawPolygon(points, TRIANGLE_NUM_POINTS);

    painter->restore();
}

void ScalePicker::setPen(QPen pen)
{
    _pen = pen;
}

// convert the position of the click to the plot coordinates
double ScalePicker::position(double posPx) const
{
    return _scaleWidget->scaleDraw()->scaleMap().invTransform(posPx);
}

int ScalePicker::positionPx(QMouseEvent* mouseEvent)
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
double ScalePicker::posCanvasPx(double pos) const
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

void ScalePicker::updateSnapPoints()
{
    auto allTicks = _scaleWidget->scaleDraw()->scaleDiv().ticks(QwtScaleDiv::MajorTick) +
        _scaleWidget->scaleDraw()->scaleDiv().ticks(QwtScaleDiv::MediumTick) +
        _scaleWidget->scaleDraw()->scaleDiv().ticks(QwtScaleDiv::MinorTick);

    snapPoints.clear();
    snapPointMap.clear();
    for(auto t : allTicks)
    {
        // `round` is used because `allTicks` is double but `snapPoints` is int
        int p = round(_scaleWidget->scaleDraw()->scaleMap().transform(t));
        snapPoints << p;
        snapPointMap[p] = t;
    }
}
