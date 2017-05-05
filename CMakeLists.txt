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

cmake_minimum_required(VERSION 3.2.2)

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

# If set, cmake will download Qwt over SVN, build and use it as a static library.
set(BUILD_QWT true CACHE BOOL "Download and build Qwt automatically.")
if (BUILD_QWT)
  include(BuildQwt)
else (BUILD_QWT)
    find_package(Qwt 6.1 REQUIRED)
endif (BUILD_QWT)

# If set, cmake will download QtColorWidgets over git, build and use it as a static library.
set(BUILD_QTCOLORWIDGETS true CACHE BOOL "Download and build QtColorWidgets library automatically.")
if (BUILD_QTCOLORWIDGETS)
  include(BuildQColorWidgets)
else ()
  find_package(QtColorWidgets REQUIRED)
endif ()

# includes
include_directories("./src"
  ${QWT_INCLUDE_DIR}
  ${QTCOLORWIDGETS_INCLUDE_DIRS}
  )

# flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTCOLORWIDGETS_FLAGS}")

# wrap UI and resource files
qt5_wrap_ui(UI_FILES
  src/mainwindow.ui
  src/portcontrol.ui
  src/about_dialog.ui
  src/snapshotview.ui
  src/commandpanel.ui
  src/commandwidget.ui
  src/dataformatpanel.ui
  src/plotcontrolpanel.ui
  src/recordpanel.ui
  src/numberformatbox.ui
  src/endiannessbox.ui
  src/binarystreamreadersettings.ui
  src/asciireadersettings.ui
  src/framedreadersettings.ui
  )

if (WIN32)
  qt5_add_resources(RES_FILES misc/icons.qrc misc/winicons.qrc)
else (WIN32)
  qt5_add_resources(RES_FILES misc/icons.qrc)
endif (WIN32)

add_executable(${PROGRAM_NAME} WIN32
  src/main.cpp
  src/mainwindow.cpp
  src/portcontrol.cpp
  src/plot.cpp
  src/zoomer.cpp
  src/scrollzoomer.cpp
  src/scrollbar.cpp
  src/hidabletabwidget.cpp
  src/framebuffer.cpp
  src/scalepicker.cpp
  src/scalezoomer.cpp
  src/portlist.cpp
  src/snapshot.cpp
  src/snapshotview.cpp
  src/snapshotmanager.cpp
  src/plotsnapshotoverlay.cpp
  src/commandpanel.cpp
  src/commandwidget.cpp
  src/commandedit.cpp
  src/dataformatpanel.cpp
  src/plotcontrolpanel.cpp
  src/recordpanel.cpp
  src/datarecorder.cpp
  src/tooltipfilter.cpp
  src/sneakylineedit.cpp
  src/channelmanager.cpp
  src/channelinfomodel.cpp
  src/framebufferseries.cpp
  src/numberformatbox.cpp
  src/endiannessbox.cpp
  src/abstractreader.cpp
  src/binarystreamreader.cpp
  src/binarystreamreadersettings.cpp
  src/asciireader.cpp
  src/asciireadersettings.cpp
  src/demoreader.cpp
  src/framedreader.cpp
  src/framedreadersettings.cpp
  src/plotmanager.cpp
  src/numberformat.cpp
  misc/windows_icon.rc
  ${UI_FILES}
  ${RES_FILES}
  )

# Use the Widgets module from Qt 5.
target_link_libraries(${PROGRAM_NAME}
  ${QWT_LIBRARY}
  ${QTCOLORWIDGETS_LIBRARIES}
  )
qt5_use_modules(${PROGRAM_NAME} Widgets SerialPort)

if (BUILD_QWT)
  add_dependencies(${PROGRAM_NAME} QWT)
endif ()

if (BUILD_QTCOLORWIDGETS)
  add_dependencies(${PROGRAM_NAME} QCW)
endif ()

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

# default version
set(VERSION_STRING "0.9.0")
set(VERSION_REVISION "0")

# get revision number from mercurial and parse version string
include(GetVersion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_STRING=\\\"${VERSION_STRING}\\\" ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_REVISION=\\\"${VERSION_REVISION}\\\" ")

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

# prepare menu item and icon
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/misc/program_name.desktop.in"
  "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.desktop")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/misc/program_name.png"
  "${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.png" COPYONLY)

set(DESKTOP_FILE ${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.desktop)
set(ICON_FILE ${CMAKE_BINARY_DIR}/${PROGRAM_NAME}.png)

# install menu item and icon
if (UNIX)
  install(FILES ${DESKTOP_FILE} DESTINATION share/applications/)
  install(FILES ${ICON_FILE} DESTINATION share/icons/hicolor/256x256/apps/)
endif (UNIX)

# uninstalling
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY)

if (UNIX)
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif (UNIX)

# packaging
include(BuildLinuxAppImage)

if (UNIX)
  set(CPACK_GENERATOR "DEB")
elseif (WIN32)
  set(CPACK_GENERATOR "NSIS")
endif (UNIX)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "${PROGRAM_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Small and simple software for plotting data from serial port")
set(CPACK_PACKAGE_CONTACT "Hasan Yavuz Özderya <hy@ozderya.net>")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_STRIP_FILES TRUE)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.2.1), libqt5svg5 (>= 5.2.1), libqt5serialport5 (>= 5.2.1), libc6 (>= 2.19)")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Small and simple software for plotting data from serial port
 Supports binary data formats ([u]int8, [u]int16, [u]int32, float)
 and ASCII (as CSV). Captured waveforms can be exported in CSV format.
 Can also send simple user defined commands to serial port device.")

if (NOT QWT_USE_STATIC)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libqwt6-qt5 (>= 6.1.1)")
endif (NOT QWT_USE_STATIC)

if (UNIX)
  set(CPACK_PACKAGE_EXECUTABLES "${PROGRAM_NAME}")
elseif (WIN32)
  set(CPACK_PACKAGE_EXECUTABLES "${PROGRAM_NAME};${PROGRAM_DISPLAY_NAME}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROGRAM_NAME}")
  set(CPACK_CREATE_DESKTOP_LINKS "${PROGRAM_NAME}")
  set(CPACK_NSIS_MODIFY_PATH "ON")
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/misc/serialplot.bmp")
  string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON})
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
  set(CPACK_NSIS_MENU_LINKS
    "https://bitbucket.org/hyOzd/serialplot" "SerialPlot source code on bitbucket.org")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL true)
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
