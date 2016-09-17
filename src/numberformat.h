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

#ifndef NUMBERFORMAT_H
#define NUMBERFORMAT_H

#include <QString>

enum NumberFormat
{
    NumberFormat_uint8,
    NumberFormat_uint16,
    NumberFormat_uint32,
    NumberFormat_int8,
    NumberFormat_int16,
    NumberFormat_int32,
    NumberFormat_float,
    NumberFormat_INVALID ///< used for error cases
};

/// Convert `NumberFormat` to string for representation
QString numberFormatToStr(NumberFormat nf);

/// Convert string to `NumberFormat`
NumberFormat strToNumberFormat(QString str);

#endif // NUMBERFORMAT_H
