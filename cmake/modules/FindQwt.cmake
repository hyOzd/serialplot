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

## First, we look for local Qwt installations, if this fails we will
## look into system locations

file(GLOB qwt_glob_dir "/usr/local/qwt*/" "c:/Qwt*")

if(qwt_glob_dir)
  foreach(qwt_path ${qwt_glob_dir})
	# find the qwt_global.h file
	unset(qwt_root CACHE)
	find_path(qwt_root
	  "include/qwt_global.h"
	  HINTS "${qwt_path}")
	if(qwt_root)
	  list(APPEND qwt_roots ${qwt_root})
	endif(qwt_root)
  endforeach(qwt_path)
endif(qwt_glob_dir)

# select qwt root according to version
if(qwt_roots)
  foreach(qwt_root ${qwt_roots})
	# extract the version information
	unset(qwt_version_string)
	file(STRINGS "${qwt_root}/include/qwt_global.h" qwt_version_string
	  REGEX "#define[ \t]+QWT_VERSION_STR")
	if(qwt_version_string)
	  string(REGEX REPLACE "[^\"]*\"([0-9.]+)\".*" "\\1"
		qwt_version_string ${qwt_version_string})

	  if(Qwt_FIND_VERSION)
		if( (qwt_version_string VERSION_EQUAL Qwt_FIND_VERSION) OR
		  (qwt_version_string VERSION_GREATER Qwt_FIND_VERSION))
		  set(QWT_VERSION ${qwt_version_string})
		  set(QWT_ROOT ${qwt_root})
		  break() # found an appropriate version
		endif()
	  else(Qwt_FIND_VERSION)
		set(QWT_ROOT ${qwt_root})
		set(QWT_VERSION ${qwt_version_string})
		break() # version is not specified stop at the first qwt_root
	  endif(Qwt_FIND_VERSION)
	else(qwt_version_string)
	  message(WARNING "Couldn't find version string in qwt_global.h file.")
	endif(qwt_version_string)
  endforeach(qwt_root ${qwt_roots})
endif(qwt_roots)

if(QWT_ROOT)
  set(QWT_INCLUDE_DIR "${QWT_ROOT}/include")
  find_library(QWT_LIBRARY "qwt"
	PATHS "${QWT_ROOT}/lib")
else (QWT_ROOT)
  ## Look into system locations
  find_path(QWT_INCLUDE_DIR qwt_plot.h PATHS /usr/include/qwt)
  # try extracting version information
  if (QWT_INCLUDE_DIR)
	unset(qwt_version_string)
	file(STRINGS "${QWT_INCLUDE_DIR}/qwt_global.h" qwt_version_string
	  REGEX "#define[ \t]+QWT_VERSION_STR")
	if(qwt_version_string)
	  string(REGEX REPLACE "[^\"]*\"([0-9.]+)\".*" "\\1"
		qwt_version_string ${qwt_version_string})
	  if(Qwt_FIND_VERSION)
		if( (qwt_version_string VERSION_EQUAL Qwt_FIND_VERSION) OR
		  (qwt_version_string VERSION_GREATER Qwt_FIND_VERSION))
		  set(QWT_VERSION ${qwt_version_string})
		else ()
		  set(QWT_INCLUDE_DIR "NOTFOUND")
		endif()
	  endif(Qwt_FIND_VERSION)
	endif(qwt_version_string)
  endif (QWT_INCLUDE_DIR)
  # look into system locations for lib file
  find_library(QWT_LIBRARY "qwt" PATHS /usr/lib)
endif(QWT_ROOT)

# set version variables
if(QWT_VERSION)
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1"
	QWT_MAJOR_VERSION ${QWT_VERSION})
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\2"
	QWT_MINOR_VERSION ${QWT_VERSION})
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3"
	QWT_PATCH_VERSION ${QWT_VERSION})
endif(QWT_VERSION)

# set QWT_FOUND
if(QWT_INCLUDE_DIR AND QWT_LIBRARY)
  set(QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})
  set(QWT_LIBRARIES ${QWT_LIBRARY})
  set(QWT_FOUND true)
else()
  set(QWT_FOUND false)
endif()

# errors
if(NOT QWT_FOUND)
  if(Qwt_FIND_QUIET)
	message(WARNING "Couldn't find Qwt.")
  elseif(Qwt_FIND_REQUIRED)
	message(FATAL_ERROR "Couldn't find Qwt.")
  endif(Qwt_FIND_QUIET)
endif(NOT QWT_FOUND)
