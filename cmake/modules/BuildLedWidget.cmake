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

include(ExternalProject)

ExternalProject_Add(LEDW
  PREFIX ledw
  HG_REPOSITORY https://bitbucket.org/hyOzd/ledwidget
  UPDATE_COMMAND ""
  INSTALL_COMMAND "")

ExternalProject_Get_Property(LEDW binary_dir source_dir)
set(LEDWIDGET_INCLUDE_DIR ${source_dir}/src)
set(LEDWIDGET_LIBRARY ${binary_dir}/libledwidget.a)
