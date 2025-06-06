#
# Copyright © 2025 Hasan Yavuz Özderya
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

cmake_minimum_required(VERSION 3.16)

project(serialplot VERSION 0.13.0 LANGUAGES CXX)

set(PROGRAM_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "Output program name")
set(PROGRAM_DISPLAY_NAME "SerialPlot" CACHE STRING "Display name (menus etc) of the program")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add local path for cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find the QtWidgets library
find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Network Svg)
qt_standard_project_setup()

# If set, cmake will download Qwt over SVN, build and use it as a static library.
set(BUILD_QWT true CACHE BOOL "Download and build Qwt automatically.")
if (BUILD_QWT)
  include(BuildQwt)
else (BUILD_QWT)
    find_package(Qwt 6.3 REQUIRED)
endif (BUILD_QWT)

# includes
include_directories("./src"
  ${QWT_INCLUDE_DIR}
  )

if (WIN32)
  qt_add_resources(RES_FILES misc/icons.qrc misc/winicons.qrc)
else (WIN32)
  qt_add_resources(RES_FILES misc/icons.qrc)
endif (WIN32)

qt_add_executable(${PROGRAM_NAME} WIN32
  src/main.cpp
  src/mainwindow.cpp
  src/portcontrol.cpp
  src/plot.cpp
  src/zoomer.cpp
  src/scrollzoomer.cpp
  src/scrollbar.cpp
  src/hidabletabwidget.cpp
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
  src/stream.cpp
  src/streamchannel.cpp
  src/channelinfomodel.cpp
  src/ringbuffer.cpp
  src/ringbuffer.cpp
  src/indexbuffer.cpp
  src/linindexbuffer.cpp
  src/readonlybuffer.cpp
  src/framebufferseries.cpp
  src/numberformatbox.cpp
  src/endiannessbox.cpp
  src/abstractreader.cpp
  src/binarystreamreader.cpp
  src/binarystreamreadersettings.cpp
  src/asciireader.cpp
  src/asciireadersettings.cpp
  src/demoreader.cpp
  src/demoreadersettings.cpp
  src/framedreader.cpp
  src/framedreadersettings.cpp
  src/plotmanager.cpp
  src/plotmenu.cpp
  src/barplot.cpp
  src/barchart.cpp
  src/barscaledraw.cpp
  src/numberformat.cpp
  src/updatechecker.cpp
  src/versionnumber.cpp
  src/updatecheckdialog.cpp
  src/samplepack.cpp
  src/source.cpp
  src/sink.cpp
  src/samplecounter.cpp
  src/ledwidget.cpp
  src/datatextview.cpp
  src/bpslabel.cpp
  misc/windows_icon.rc
  ${RES_FILES}
  )

# Use the Widgets module from Qt 5.
target_link_libraries(${PROGRAM_NAME}
  PRIVATE ${QWT_LIBRARY} Qt6::Widgets Qt6::SerialPort Qt6::Network Qt6::Svg)

if (BUILD_QWT)
  add_dependencies(${PROGRAM_NAME} QWT)
endif ()

# set compiler flags
add_compile_options(-Wall)

# get the revision number as 'shorthash+' from git
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_HASH)
string(SUBSTRING ${GIT_HASH} 0 10 GIT_SHORT_HASH)
git_local_changes(GIT_STATUS)
set(VERSION_REVISION "${GIT_SHORT_HASH}")
if (GIT_STATUS STREQUAL "DIRTY")
    set(VERSION_REVISION "${VERSION_REVISION}+")
endif()
message(STATUS "Git revision: ${VERSION_REVISION}")

# set compiler defines for version number
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_STRING=\\\"${PROJECT_VERSION}\\\" ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_MAJOR=${PROJECT_VERSION_MAJOR} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_MINOR=${PROJECT_VERSION_MINOR} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_PATCH=${PROJECT_VERSION_PATCH} ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVERSION_REVISION=\\\"${VERSION_REVISION}\\\" ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROGRAM_NAME=\\\"${PROGRAM_NAME}\\\" ")

# add make run target
add_custom_target(run
    COMMAND ${PROGRAM_NAME}
    DEPENDS ${PROGRAM_NAME}
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# installing
install(TARGETS ${PROGRAM_NAME} DESTINATION bin)

# on windows use windeployqt to collect qt dependencies
if (WIN32)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt REQUIRED)

  install(CODE "
        execute_process(
            COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --dir \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin\" \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/${PROGRAM_NAME}.exe\"
            OUTPUT_VARIABLE WINDEPLOYQT_OUTPUT
            ERROR_VARIABLE WINDEPLOYQT_ERROR
            RESULT_VARIABLE WINDEPLOYQT_RESULT
        )
        if (WINDEPLOYQT_RESULT)
            message(FATAL_ERROR \"windeployqt failed: \${WINDEPLOYQT_ERROR}\")
        endif()
  ")
endif()

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

# testing
set(ENABLE_TESTS false CACHE BOOL "Build tests.")
if (ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif ()

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
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_STRIP_FILES TRUE)
# add debian package versions from ubuntu 24.04
set(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libqt6widgets6t64 (>= 6.4.2), \
libqt6svg6 (>= 6.4.2), \
libqt6serialport6 (>= 6.4.2), \
libqt6network6t64 (>= 6.4.2), \
libc6 (>= 2.39)")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Small and simple software for plotting data from serial port
 Supports binary data formats ([u]int8, [u]int16, [u]int32, float)
 and ASCII (as CSV). Captured waveforms can be exported in CSV format.
 Can also send simple user defined commands to serial port device.")

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
    "https://hg.sr.ht/~hyozd/serialplot/" "SerialPlot source code on sourcehut.org")
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
