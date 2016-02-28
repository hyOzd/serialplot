/*
  Copyright © 2016 Hasan Yavuz Özderya

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

#include "sneakylineedit.h"
#include <QFont>

SneakyLineEdit::SneakyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    setFrame(false);
    setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);}");
    setToolTip(trUtf8("Click to edit"));    

    setBold(true);
}

void SneakyLineEdit::focusInEvent(QFocusEvent *event)
{
    setFrame(true);
    setBold(false);
    QLineEdit::focusInEvent(event);
}

void SneakyLineEdit::focusOutEvent(QFocusEvent *event)
{
    setFrame(false);
    setBold(true);
    QLineEdit::focusOutEvent(event);
}

void SneakyLineEdit::setBold(bool bold)
{
    QFont f(font());
    f.setBold(bold);
    setFont(f);
}
