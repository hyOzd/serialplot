cmake_minimum_required(VERSION 2.8.11)

project(serialplot)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# add local path for cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qwt 6.1 REQUIRED)

# includes
include_directories(${QWT_INCLUDE_DIR})

# wrap UI files
qt5_wrap_ui(UI_FILES mainwindow.ui)

# Tell CMake to create the helloworld executable
add_executable(serialplot main.cpp mainwindow.cpp customcheckablebutton.cpp ${UI_FILES})

# Use the Widgets module from Qt 5.
qt5_use_modules(serialplot Widgets SerialPort)
target_link_libraries(serialplot ${QWT_LIBRARY})

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

# add make run target
add_custom_target(run
    COMMAND serialplot
    DEPENDS serialplot
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)