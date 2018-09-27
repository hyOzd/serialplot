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

#include "zoomer.h"
#include <qwt_plot.h>
#include <QPen>
#include <QMouseEvent>

Zoomer::Zoomer(QWidget* widget, const Stream* stream, bool doReplot) :
    ScrollZoomer(widget)
{
    is_panning = false;
    _stream = stream;

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
    // ScrollZoomer::drawTracker(painter);
    if (_stream != nullptr && _stream->numChannels())
    {
        drawValues(painter);
    }
    return;
}

void Zoomer::drawValues(QPainter* painter) const
{
    painter->save();

    double x = invTransform(trackerPosition()).x();
    auto values = findValues(x);

    // draw vertical line
    auto linePen = QPen(Qt::DotLine);
    linePen.setColor(Qt::white);
    painter->setPen(linePen);
    const QRect pRect = pickArea().boundingRect().toRect();
    int px = trackerPosition().x();
    painter->drawLine(px, pRect.top(), px, pRect.bottom());

    // draw sample values
    for (int ci = 0; ci < values.size(); ci++)
    {
        double val = values[ci];
        if (!std::isnan(val))
        {
            auto p = transform(QPointF(x, val));

            painter->setBrush(_stream->channel(ci)->color());
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(p, 4, 4);

            painter->setPen(Qt::white);
            painter->drawText(p, QString("%1").arg(val));
        }
    }

    painter->restore();
}

QVector<double> Zoomer::findValues(double x) const
{
    // TODO: process only channel(s) of this plot
    unsigned nc = _stream->numChannels();
    QVector<double> r(nc);
    for (unsigned ci = 0; ci < nc; ci++)
    {
        auto chan = _stream->channel(ci);
        double val = chan->findValue(x);
        r[ci] = val;
    }
    return r;
}

QRect Zoomer::trackerRect(const QFont& font) const
{
    // TODO: optimize tracker area for masking instead of returning whole plot size
    return pickArea().boundingRect().toRect();
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
