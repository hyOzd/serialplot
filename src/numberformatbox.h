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

#ifndef NUMBERFORMATBOX_H
#define NUMBERFORMATBOX_H

#include <QGroupBox>
#include <QButtonGroup>

namespace Ui {
class NumberFormatBox;
}

enum NumberFormat
{
    NumberFormat_uint8,
    NumberFormat_uint16,
    NumberFormat_uint32,
    NumberFormat_int8,
    NumberFormat_int16,
    NumberFormat_int32,
    NumberFormat_float,
};

class NumberFormatBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit NumberFormatBox(QWidget *parent = 0);
    ~NumberFormatBox();

    NumberFormat currentSelection(); ///< returns the currently selected number format

signals:
    /// Signaled when number format selection is changed
    void selectionChanged(NumberFormat numberFormat);

private:
    Ui::NumberFormatBox *ui;
    QButtonGroup buttonGroup;

private slots:
    void onButtonToggled(int numberFormatId, bool checked);
};

#endif // NUMBERFORMATBOX_H
