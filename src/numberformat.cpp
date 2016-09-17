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

#include <QMap>

#include "numberformat.h"

QMap<NumberFormat, QString> mapping({
        {NumberFormat_uint8, "uint8"},
        {NumberFormat_uint16, "uint16"},
        {NumberFormat_uint32, "uint32"},
        {NumberFormat_int8, "int8"},
        {NumberFormat_int16, "int16"},
        {NumberFormat_int32, "int32"},
        {NumberFormat_float, "float"}
    });

QString numberFormatToStr(NumberFormat nf)
{
    return mapping.value(nf);
}

NumberFormat strToNumberFormat(QString str)
{
    return mapping.key(str, NumberFormat_INVALID);
}
