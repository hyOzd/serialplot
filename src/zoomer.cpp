/*
  Copyright © 2021 Hasan Yavuz Özderya

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
#include <qwt_text.h>
#include <QPen>
#include <QMouseEvent>
#include <QtMath>
#include <QPainter>
#include <algorithm>

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

const double ValueLabelHeight = 12; // TODO: calculate

struct ChannelValue
{
    const StreamChannel* ch;
    double value;
    double y;
    double top() const {return y;};
    double bottom() const {return y + ValueLabelHeight;};
};

static void layoutValues(QList<ChannelValue>& values)
{
    typedef ChannelValue LayItem;
    typedef QList<LayItem*> LayItemList;

    struct LayGroup
    {
        struct VRange {double top, bottom;};
        LayItemList items;
        LayGroup(LayItem* initialItem) {items.append(initialItem);}
        unsigned numItems() const {return items.size();}
        double top() const {return items.first()->top();}
        double bottom() const {return items.last()->bottom();}
        VRange vRange() const {return {top(), bottom()};}
        double overlap(const LayGroup* otrGroup) const
            {
                auto myr = vRange();
                auto otr = otrGroup->vRange();

                double a = myr.bottom - otr.top;
                double b = otr.bottom - myr.top;
                if (a > 0 and b > 0)
                {
                    return std::min(a, b);
                }
                return 0;
            }
        void moveBy(double y) {for (auto it : items) it->y += y;}
        void join(LayGroup* other)
            {
                // assumes other group is below this one and they overlap
                double ovr_h = overlap(other);

                // groups are moved less if they have more items and vice versa
                double ratio = double(numItems()) / double(numItems() + other->numItems());
                double self_off = ovr_h * (1. - ratio);

                // make sure we don't go out of screen (above) after the shift
                double final_top = top() - self_off;
                if (final_top < 0)
                    self_off += final_top;

                // move groups
                moveBy(-self_off);                // up
                other->moveBy(ovr_h - self_off);  // down

                // finalize the merge by gettin items from other
                do
                {
                    items.append(other->items.takeFirst());
                } while (!other->items.isEmpty());
            }
    };

    // create initial groups (1 group per item)
    QList<LayGroup*> groups;
    for (auto& val : values)
        groups.append(new LayGroup(&val));

    // sort groups according to their items position
    struct {
        bool operator()(LayGroup* a, LayGroup* b) const
            {
                return a->top() < b->top();
            }
    } compTops;

    std::sort(groups.begin(), groups.end(), compTops);

    // do spacing
    bool somethingOverlaps = true;
    while (somethingOverlaps and groups.size() > 1)
    {
        somethingOverlaps = false;
        for (int i = 0; i < groups.size() - 1; i++)
        {
            auto a = groups[i];
            auto b = groups[i + 1];

            // make sure nothing is over the top
            if (a->top() < 0)
                a->moveBy(-a->top());

            // join if groups overlap
            if (a->overlap(b))
            {
                somethingOverlaps = true;
                a->join(b);
                delete groups.takeAt(i + 1);
                break;
            }
        }
    }

    // cleanup
    do
    {
        delete groups.takeFirst();
    } while (!groups.isEmpty());
};

void Zoomer::drawValues(QPainter* painter) const
{
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
        {
            auto point = transform(QPointF(x, value));
            values.append({ch, value, double(point.y())});
        }
    }

    // TODO should keep?
    if (values.isEmpty())
    {
        return;
    }

    layoutValues(values);

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
        painter->drawText(QRectF(point.x() + VALUE_TEXT_MARGIN, value.y, 1, 1),
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
