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

#include "zoomer.h"
#include <qwt_plot.h>
#include <QtDebug>

Zoomer::Zoomer(QWidget* widget, bool doReplot) :
    ScrollZoomer(widget)
{
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
    const QRectF rect = invTransform(QRect(pa.first(), pa.last()).normalized());

    QString sizeText = QString(" [%1, %2]").\
        arg(rect.width(), 0, 'g', 4).\
        arg(rect.height(), 0, 'g', 4);

    b.setText(b.text() + sizeText);

    return b;
}
