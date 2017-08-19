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

#include <QRegularExpression>

#include "versionnumber.h"

VersionNumber::VersionNumber(unsigned mj, unsigned mn, unsigned pt)
{
    major = mj;
    minor = mn;
    patch = pt;
}

QString VersionNumber::toString() const
{
    return QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
}

bool VersionNumber::extract(const QString& str, VersionNumber& number)
{
    QRegularExpression regexp("(?:[-_vV \\t]|^)(?<major>\\d+)"
                              "(?:\\.(?<minor>\\d+))?(?:\\.(?<patch>\\d+))?[-_ \\t]?");
    auto match = regexp.match(str, 0, QRegularExpression::PartialPreferCompleteMatch);

    if (!(match.hasMatch() || match.hasPartialMatch())) return false;

    number.major = match.captured("major").toUInt();

    auto zeroIfNull = [](QString str) -> unsigned
        {
            if (str.isNull()) return 0;
            return str.toUInt();
        };

    number.minor = zeroIfNull(match.captured("minor"));
    number.patch = zeroIfNull(match.captured("patch"));

    return true;
}

bool operator==(const VersionNumber& lhs, const VersionNumber& rhs)
{
    return lhs.major == rhs.major &&
           lhs.minor == rhs.minor &&
           lhs.patch == rhs.patch;
}

bool operator<(const VersionNumber& lhs, const VersionNumber& rhs)
{
    if (lhs.major < rhs.major)
    {
        return true;
    }
    else if (lhs.major == rhs.major)
    {
        if (lhs.minor < rhs.minor)
        {
             return true;
        }
        else if (lhs.minor == rhs.minor)
        {
            if (lhs.patch < rhs.patch) return true;
        }
    }
    return false;
}

bool operator>(const VersionNumber& lhs, const VersionNumber& rhs)
{
    if (lhs.major > rhs.major)
    {
        return true;
    }
    else if (lhs.major == rhs.major)
    {
        if (lhs.minor > rhs.minor)
        {
             return true;
        }
        else if (lhs.minor == rhs.minor)
        {
            if (lhs.patch > rhs.patch) return true;
        }
    }
    return false;
}
