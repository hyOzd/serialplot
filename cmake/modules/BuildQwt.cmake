#
# Copyright © 2021 Hasan Yavuz Özderya
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

ExternalProject_Add(QWT
  PREFIX qwt
  # SVN_REPOSITORY svn://svn.code.sf.net/p/qwt/code/branches/qwt-6.1
  URL https://sourceforge.net/projects/qwt/files/qwt/6.1.6/qwt-6.1.6.tar.bz2
  # disable QwtDesigner plugin and enable static build
  PATCH_COMMAND sed -i -r -e "s/QWT_CONFIG\\s*\\+=\\s*QwtDesigner/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtDll/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtSvg/#&/"
                          -e "s/QWT_CONFIG\\s*\\+=\\s*QwtOpenGL/#&/"
						  -e "s|QWT_INSTALL_PREFIX\\s*=.*|QWT_INSTALL_PREFIX = <INSTALL_DIR>|"
                             <SOURCE_DIR>/qwtconfig.pri
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND qmake <SOURCE_DIR>/qwt.pro
  )

ExternalProject_Get_Property(QWT install_dir)
set(QWT_ROOT ${install_dir})
set(QWT_LIBRARY ${QWT_ROOT}/lib/libqwt.a)
set(QWT_INCLUDE_DIR ${QWT_ROOT}/include)
