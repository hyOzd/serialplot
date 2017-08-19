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

#ifndef VERSIONNUMBER_H
#define VERSIONNUMBER_H

#include <QString>

struct VersionNumber
{
    unsigned major = 0;
    unsigned minor = 0;
    unsigned patch = 0;

    VersionNumber(unsigned mj=0, unsigned mn=0, unsigned pt=0);

    /// Convert version number to string.
    QString toString() const;

    /// Extracts the version number from given string.
    static bool extract(const QString& str, VersionNumber& number);
};

bool operator==(const VersionNumber& lhs, const VersionNumber& rhs);
bool operator<(const VersionNumber& lhs, const VersionNumber& rhs);
bool operator>(const VersionNumber& lhs, const VersionNumber& rhs);

#endif // VERSIONNUMBER_H
