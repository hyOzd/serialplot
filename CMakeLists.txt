#
# Copyright © 2015 Hasan Yavuz Özderya
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

cmake_minimum_required(VERSION 2.8.11)

project(serialplot)

set(PROGRAM_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "Output program name")
set(PROGRAM_DISPLAY_NAME "SerialPlot" CACHE STRING "Display name (menus etc) of the program")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# add local path for cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find the QtWidgets library
find_package(Qt5Widgets)

# Find QWT or use static
set(QWT_USE_STATIC false CACHE BOOL "Use a static version of Qwt provided by user.")
set(QWT_STATIC_LIBRARY "" CACHE FILEPATH "Path to the static Qwt library, libqwt.a.")
set(QWT_STATIC_INCLUDE "" CACHE PATH "Path to the Qwt include directory when building Qwt static.")

if (QWT_USE_STATIC)
  set(QWT_LIBRARY ${QWT_STATIC_LIBRARY})
  set(QWT_INCLUDE_DIR ${QWT_STATIC_INCLUDE})
else (QWT_USE_STATIC)
  find_package(Qwt 6.1 REQUIRED)
endif (QWT_USE_STATIC)

# includes
include_directories(${QWT_INCLUDE_DIR})

# wrap UI files
qt5_wrap_ui(UI_FILES mainwindow.ui portcontrol.ui about_dialog.ui)

add_executable(${PROGRAM_NAME} WIN32
  main.cpp
  mainwindow.cpp
  portcontrol.cpp
  plot.cpp
  zoomer.cpp
  hidabletabwidget.cpp
  framebuffer.cpp
  portlist.cpp
  ${UI_FILES}
  misc/windows_icon.rc
  )

# Use the Widgets module from Qt 5.
target_link_libraries(${PROGRAM_NAME} ${QWT_LIBRARY})
qt5_use_modules(${PROGRAM_NAME} Widgets SerialPort Svg)

# set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Enable C++11 support, fail if not supported
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# version number
set(MAJOR_VERSION 0)
set(MINOR_VERSION 4)
set(PATCH_VERSION 0)
set(VERSION_STRING "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

# get revision number from mercurial
find_program(MERCURIAL hg)

if (MERCURIAL)
  execute_process(COMMAND ${MERCURIAL} id -i
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE MERCURIAL_RESULT
    OUTPUT_VARIABLE VERSION_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT MERCURIAL_RESULT EQUAL 0)
    set(VERSION_SCM_REVISION false)
  endif(NOT MERCURIAL_RESULT EQUAL 0)
endif (MERCURIAL)

if (NOT VERSION_REVISION)
  set(VERSION_REVISION "0")
endif (NOT VERSION_REVISION)

message("SCM revision: ${VERSION_REVISION}")

# configure version file
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h")

# add make run target
add_custom_target(run
    COMMAND ${PROGRAM_NAME}
    DEPENDS ${PROGRAM_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# installing
install(TARGETS ${PROGRAM_NAME} DESTINATION bin)

# for windows put libraries to install directory
if (WIN32)
  file(GLOB WINDOWS_INSTALL_LIBRARIES
    "${CMAKE_BINARY_DIR}/windows_install_libraries/*.*")
  install(FILES ${WINDOWS_INSTALL_LIBRARIES} DESTINATION bin)
endif (WIN32)

# install menu item and icon
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/misc/program_name.desktop.in"
  "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.desktop")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/misc/program_name.png"
  "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.png" COPYONLY)

find_program(XDG-DESKTOP-MENU xdg-desktop-menu)
find_program(XDG-ICON-RESOURCE xdg-icon-resource)

if (UNIX)
  # first copy files to share/serialplot/
  install(FILES
    ${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.desktop
    ${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.png
    DESTINATION share/${PROGRAM_NAME})

  # install copyright file (in debian format)
  install(FILES
    ${CMAKE_SOURCE_DIR}/debian/copyright
    DESTINATION share/doc/${PROGRAM_NAME}/)

  install(CODE "
  execute_process(COMMAND ${XDG-DESKTOP-MENU} install --novendor ${CMAKE_INSTALL_PREFIX}/share/${PROGRAM_NAME}/${PROGRAM_NAME}.desktop)
  execute_process(COMMAND ${XDG-ICON-RESOURCE} install --novendor --size 256 ${CMAKE_INSTALL_PREFIX}/share/${PROGRAM_NAME}/${PROGRAM_NAME}.png)
")
endif (UNIX)

# uninstalling
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY)

if (UNIX)
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    COMMAND ${XDG-DESKTOP-MENU} uninstall --novendor ${PROGRAM_NAME}.desktop
    COMMAND ${XDG-ICON-RESOURCE} uninstall --novendor --size 256 ${PROGRAM_NAME})
endif (UNIX)

# packaging
if (UNIX)
  set(CPACK_GENERATOR "DEB")
elseif (WIN32)
  set(CPACK_GENERATOR "NSIS")
endif (UNIX)

include(InstallRequiredSystemLibraries)

# configure debian scripts
configure_file(${CMAKE_SOURCE_DIR}/debian/postinst.in ${CMAKE_BINARY_DIR}/debian/postinst)
configure_file(${CMAKE_SOURCE_DIR}/debian/postrm.in ${CMAKE_BINARY_DIR}/debian/postrm)

set(CPACK_PACKAGE_NAME "${PROGRAM_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Small and simple software for plotting data from serial port")
set(CPACK_PACKAGE_CONTACT "Hasan Yavuz Özderya <hy@ozderya.net>")
set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.2.1), libqt5serialport5 (>= 5.2.1), libc6 (>= 2.19)")
if (NOT QWT_USE_STATIC)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libqwt6-qt5 (>= 6.1.1)")
endif (NOT QWT_USE_STATIC)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
  "${CMAKE_BINARY_DIR}/debian/postinst"
  "${CMAKE_BINARY_DIR}/debian/postrm")

if (UNIX)
  set(CPACK_PACKAGE_EXECUTABLES "${PROGRAM_NAME}")
elseif (WIN32)
  set(CPACK_PACKAGE_EXECUTABLES "${PROGRAM_NAME};${PROGRAM_DISPLAY_NAME}")
  set(CPACK_CREATE_DESKTOP_LINKS "${PROGRAM_NAME}")
  set(CPACK_NSIS_MODIFY_PATH "ON")
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/misc/serialplot.bmp")
  string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
  set(CPACK_NSIS_MENU_LINKS
    "https://bitbucket.org/hyOzd/serialplot" "SerialPlot source code on bitbucket.org")
endif (UNIX)

if (UNIX)
  # set debian package name
  string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
  find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
  if(DPKG_PROGRAM)
    execute_process(
      COMMAND ${DPKG_PROGRAM} --print-architecture
      OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CPACK_PACKAGE_FILE_NAME
      "${CPACK_PACKAGE_NAME_LOWERCASE}_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  else(DPKG_PROGRAM)
    set(CPACK_PACKAGE_FILE_NAME
      "${CPACK_PACKAGE_NAME_LOWERCASE}_${PROJECT_VERSION}-${PROJECT_VERSION_REVISION}_${CMAKE_SYSTEM_NAME}")
  endif(DPKG_PROGRAM)
endif (UNIX)

include(CPack)
