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

# Find the QtWidgets library
find_package(Qt5Widgets)

include_directories("../src")

add_executable(Test EXCLUDE_FROM_ALL
  test.cpp
  ../src/datachunk.cpp
  ../src/chunkedbuffer.cpp)
add_test(NAME test1 COMMAND Test)
qt5_use_modules(Test Widgets)

set(CMAKE_CTEST_COMMAND ctest -V)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_dependencies(check Test)
