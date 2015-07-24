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

#include "zoomer.h"
#include <qwt_plot.h>
#include <QtDebug>

Zoomer::Zoomer(QWidget* widget, bool doReplot) :
    QwtPlotZoomer(widget, doReplot)
{
    // do nothing
}

void Zoomer::zoom(int up)
{
    if (up == +1)
    {
        this->setZoomBase(this->plot());
    }

    QwtPlotZoomer::zoom(up);

    if (up == 0)
    {
        emit unzoomed();
    }
}
