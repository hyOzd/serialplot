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

#include "zoomer.h"
#include <qwt_plot.h>
#include <QPen>
#include <QMouseEvent>
#include <QtMath>

static const int VALUE_POINT_DIAM = 4;
static const int VALUE_TEXT_MARGIN = VALUE_POINT_DIAM + 2;

Zoomer::Zoomer(QWidget* widget, bool doReplot) :
    ScrollZoomer(widget)
{
    is_panning = false;

    setTrackerMode(AlwaysOn);

    // set corner widget between the scrollbars with default background color
    auto cornerWidget = new QWidget();
    auto bgColor = cornerWidget->palette().color(QPalette::Window).name();
    auto styleSheet = QString("background-color:%1;").arg(bgColor);
    cornerWidget->setStyleSheet(styleSheet);
    ScrollZoomer::setCornerWidget(cornerWidget);
}

void Zoomer::zoom(int up)
{
    ScrollZoomer::zoom(up);

    if(zoomRectIndex() == 0)
    {
        emit unzoomed();
    }
}

void Zoomer::zoom( const QRectF & rect)
{
    // set the zoom base when user zooms in to first level
    if (zoomRectIndex() == 0)
    {
        this->setZoomBase(false);
    }

    ScrollZoomer::zoom(rect);
}

void Zoomer::setDispChannels(QVector<const StreamChannel*> channels)
{
    dispChannels = channels;
}

QwtText Zoomer::trackerTextF(const QPointF& pos) const
{
    QwtText b = ScrollZoomer::trackerTextF(pos);

    const QPolygon pa = selection();
    if (!isActive() || pa.count() < 2)
    {
        return b;
    }

    const QRectF rect = invTransform(QRect(pa.first(), pa.last()).normalized());

    QString sizeText = QString(" [%1, %2]").\
        arg(rect.width(), 0, 'g', 4).\
        arg(rect.height(), 0, 'g', 4);

    b.setText(b.text() + sizeText);

    return b;
}

void Zoomer::drawRubberBand(QPainter* painter) const
{
    const double FILL_ALPHA = 0.2;

    QColor color = painter->pen().color();
    color.setAlphaF(FILL_ALPHA);
    painter->setBrush(color);

    ScrollZoomer::drawRubberBand(painter);
}

QRegion Zoomer::rubberBandMask() const
{
    const QPolygon pa = selection();
    if (pa.count() < 2)
    {
        return QRegion();
    }
    const QRect r = QRect(pa.first(), pa.last()).normalized().adjusted(0, 0, 1, 1);
    return QRegion(r);
}

void Zoomer::drawTracker(QPainter* painter) const
{
    if (isActive())
    {
        QwtPlotZoomer::drawTracker(painter);
    }
    else if (dispChannels.length())
    {
        drawValues(painter);
    }
}

QList<const StreamChannel*> Zoomer::visChannels() const
{
    QList<const StreamChannel*> result;

    for (unsigned ci = 0; ci < (unsigned) dispChannels.length(); ci++)
    {
        if (dispChannels[ci]->visible())
            result.append(dispChannels[ci]);
    }

    return result;
}

void Zoomer::drawValues(QPainter* painter) const
{
    struct ChannelValue
    {
        const StreamChannel* ch;
        double value;
    };

    auto tpos = trackerPosition();
    if (tpos.x() < 0) return;   // cursor not on window

    // find Y values for current cursor X position
    double x = invTransform(tpos).x();
    auto channels = visChannels();
    QList<ChannelValue> values;
    for (auto ch : channels)
    {
        double value = ch->findValue(x);
        if (!std::isnan(value))
            values.append({ch, value});
    }

    // TODO should keep?
    if (values.isEmpty())
    {
        qDebug() << "no value to draw??";
        return;
    }

    // TODO layout

    painter->save();

    // draw vertical line
    auto linePen = rubberBandPen();
    linePen.setStyle(Qt::DotLine);
    painter->setPen(linePen);
    const QRect pRect = pickArea().boundingRect().toRect();
    int px = tpos.x();
    painter->drawLine(px, pRect.top(), px, pRect.bottom());

    // draw sample values
    for (auto value : values)
    {
        double val = value.value;
        auto ch = value.ch;

        auto point = transform(QPointF(x, val));

        painter->setBrush(ch->color());
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(point, VALUE_POINT_DIAM, VALUE_POINT_DIAM);

        painter->setPen(rubberBandPen());
        // We give a very small (1x1) rectangle but disable clipping
        painter->drawText(QRectF(point.x() + VALUE_TEXT_MARGIN, point.y(), 1, 1),
                          Qt::AlignVCenter | Qt::TextDontClip,
                          QString("%1").arg(val));
    }

    painter->restore();
}

QRect Zoomer::trackerRect(const QFont& font) const
{
    if (isActive())
    {
        return QwtPlotZoomer::trackerRect(font);
    }
    else
    {
        return valueTrackerRect(font);
    }
}

QRect Zoomer::valueTrackerRect(const QFont& font) const
{
    // TODO: consider using actual tracker values for width calculation
    const int textWidth = qCeil(QwtText("-8.8888888").textSize(font).width());
    const int width = textWidth + VALUE_POINT_DIAM + VALUE_TEXT_MARGIN;
    const int x = trackerPosition().x() - VALUE_POINT_DIAM;
    const auto pickRect = pickArea().boundingRect();

    return QRect(x, pickRect.y(), width, pickRect.height());
}

void Zoomer::widgetMousePressEvent(QMouseEvent* mouseEvent)
{
    if (mouseEvent->modifiers() & Qt::ControlModifier)
    {
        is_panning = true;
        parentWidget()->setCursor(Qt::ClosedHandCursor);
        pan_point = invTransform(mouseEvent->pos());
    }
    else
    {
        ScrollZoomer::widgetMousePressEvent(mouseEvent);
    }
}

void Zoomer::widgetMouseMoveEvent(QMouseEvent* mouseEvent)
{
    if (is_panning)
    {
        auto cur_point = invTransform(mouseEvent->pos());
        auto delta = cur_point - pan_point;
        moveBy(-delta.x(), -delta.y());
        pan_point = invTransform(mouseEvent->pos());
    }
    else
    {
        ScrollZoomer::widgetMouseMoveEvent(mouseEvent);
    }
}

void Zoomer::widgetMouseReleaseEvent(QMouseEvent* mouseEvent)
{
    if (is_panning)
    {
        is_panning = false;
        parentWidget()->setCursor(Qt::CrossCursor);
    }
    else
    {
        ScrollZoomer::widgetMouseReleaseEvent(mouseEvent);
    }
}
