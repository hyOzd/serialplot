#
# Copyright © 2018 Hasan Yavuz Özderya
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

include(ExternalProject)

ExternalProject_Add(QCW
  PREFIX qcw
  GIT_REPOSITORY https://gitlab.com/mattia.basaglia/Qt-Color-Widgets.git
  GIT_TAG 2c49e1bb4e1f591e720e2132cc2aaeef3ba73f14
  CMAKE_CACHE_ARGS "-DCMAKE_CXX_FLAGS:string=-D QTCOLORWIDGETS_STATICALLY_LINKED"
  UPDATE_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Get_Property(QCW binary_dir source_dir)
set(QTCOLORWIDGETS_FLAGS "-D QTCOLORWIDGETS_STATICALLY_LINKED")
set(QTCOLORWIDGETS_LIBRARY ${binary_dir}/libColorWidgets-qt5.a)
set(QTCOLORWIDGETS_INCLUDE_DIR ${source_dir}/include)

set(QTCOLORWIDGETS_LIBRARIES ${QTCOLORWIDGETS_LIBRARY})
set(QTCOLORWIDGETS_INCLUDE_DIRS ${QTCOLORWIDGETS_INCLUDE_DIR})
