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

#ifndef ENDIANNESSBOX_H
#define ENDIANNESSBOX_H

#include <QWidget>

namespace Ui {
class EndiannessBox;
}

enum Endianness
{
    LittleEndian,
    BigEndian
};

class EndiannessBox : public QWidget
{
    Q_OBJECT

public:
    explicit EndiannessBox(QWidget *parent = 0);
    ~EndiannessBox();

    Endianness currentSelection(); ///< currently selected endianness

signals:
    /// Signaled when endianness selection is changed
    void selectionChanged(Endianness endianness);

private:
    Ui::EndiannessBox *ui;
};

#endif // ENDIANNESSBOX_H
