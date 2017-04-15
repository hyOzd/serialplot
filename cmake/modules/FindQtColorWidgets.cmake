#
# Copyright © 2017 Hasan Yavuz Özderya
#
# This file is part of serialplot.
#
# serialplot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# serialplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
#

# Note: this script is intended for the debian package created for serialplot.

find_library(QTCOLORWIDGETS_LIBRARY "libColorWidgets-qt5.a")
find_path(QTCOLORWIDGETS_INCLUDE_DIR "color_preview.hpp" PATHS "/usr/include/qtcolorwidgets/" NO_DEFAULT_PATH)

mark_as_advanced(QTCOLORWIDGETS_LIBRARY QTCOLORWIDGETS_INCLUDE_DIR)

find_package_handle_standard_args(QtColorWidgets DEFAULT_MSG QTCOLORWIDGETS_LIBRARY QTCOLORWIDGETS_INCLUDE_DIR)

if (QTCOLORWIDGETS_FOUND)
  set(QTCOLORWIDGETS_FLAGS "-D QTCOLORWIDGETS_STATICALLY_LINKED")
  mark_as_advanced(QTCOLORWIDGETS_FLAGS)
  set(QTCOLORWIDGETS_LIBRARIES ${QTCOLORWIDGETS_LIBRARY})
  set(QTCOLORWIDGETS_INCLUDE_DIRS ${QTCOLORWIDGETS_INCLUDE_DIR})
endif (QTCOLORWIDGETS_FOUND)
