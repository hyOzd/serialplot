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

# try to get latest version from mercurial
find_program(HG hg)

if (HG)
  # get latest tag
  execute_process(COMMAND ${HG} parents --template {latesttag}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE HG_RESULT
    OUTPUT_VARIABLE HG_LATEST_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT HG_RESULT EQUAL 0)
    set(HG_LATEST_TAG "")
  endif(NOT HG_RESULT EQUAL 0)

  # get revision
  execute_process(COMMAND ${HG} id -i
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE HG_RESULT
    OUTPUT_VARIABLE HG_REVISION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT HG_RESULT EQUAL 0)
    set(HG_REVISION "")
  endif(NOT HG_RESULT EQUAL 0)
endif (HG)

# Try to get version from .hg_archival file
if (${HG_LATEST_TAG} STREQUAL "" OR NOT ${HG_LATEST_TAG})
  set(HG_ARCHIVAL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/.hg_archival.txt)
  if (EXISTS ${HG_ARCHIVAL_FILE})
    # get latest tag
    file(STRINGS ${HG_ARCHIVAL_FILE} HG_ARCHIVAL_LATEST_TAG REGEX "^latesttag:.+")
    string(REGEX REPLACE "latesttag:[ \t]*(.+)" "\\1" HG_LATEST_TAG ${HG_ARCHIVAL_LATEST_TAG})

    # get latest revision
    file(STRINGS ${HG_ARCHIVAL_FILE} HG_ARCHIVAL_REV REGEX "^node:.+")
    string(REGEX REPLACE "node:[ \t]*([a-fA-F0-9]+)" "\\1" HG_ARCHIVAL_REV ${HG_ARCHIVAL_REV})
    string(SUBSTRING ${HG_ARCHIVAL_REV} 0 12 HG_REVISION)
  endif()
endif ()

# extract version information from tag (remove 'v' prefix)
if (NOT ${HG_LATEST_TAG} STREQUAL "")
  string(REPLACE "v" "" HG_VERSION ${HG_LATEST_TAG})
  message("Version from mercurial: ${HG_VERSION} (${HG_REVISION})")

  # replace version string
  set(VERSION_STRING ${HG_VERSION})
  set(VERSION_REVISION ${HG_REVISION})
else ()
  message("Failed to find version information from mercurial.")
endif ()

# parse version numbers
string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)
