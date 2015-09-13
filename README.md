# SerialPlot

Small and simple software for plotting data from serial port.

![SerialPlot v0.4 Screenshot](http://i.imgur.com/Wb53LRt.png)

## Features
* Reading data from serial port
* Binary data formats (u)int8, (u)int16, (u)int32, float
* ASCII input (Comma Separated Values)
* Synchronized multi channel plotting

## Installation

You can obtain installation packages from
[downloads](https://bitbucket.org/hyOzd/serialplot/downloads). As of
moment there are only Debian and Windows 64 bits packages.

## Dependencies
- Qt 5, including SerialPort module
- Qwt 6.1

## Building

### Obtain Dependencies

- Qt5 development packages
- Qt5 SerialPort module
- CMake
- Mercurial

Under Ubuntu/Debian:
```sudo apt-get install qtbase5-dev libqt5serialport5-dev cmake mercurial```

### Download and Install Qwt

[Qwt](http://qwt.sourceforge.net) is the library that provides
plotting widgets for SerialPlot. If your distribution doesn't provide
a Qwt 6 package (the Qt5 version, usually named `libqwt6-qt5`) you
will need to download and build Qwt with Qt5 yourself.

Download Qwt 6 from here: http://sourceforge.net/projects/qwt/files/

Follow these instructions to build and install:
http://qwt.sourceforge.net/qwtinstall.html#qwtinstall-unix

Make sure that you have built the Qwt with Qt 5 not Qt 4.

### Download and Build SerialPlot

You can use Mercurial to download SerialPlot source code. Or you can
download it from here:
https://bitbucket.org/hyOzd/serialplot/downloads

    hg clone https://hyOzd@bitbucket.org/hyOzd/serialplot
    cd serialplot
    mkdir build && cd build
    cmake ..
    make

You can also build with QtCreator IDE using `serialplot.pro` file.

## Known Issues
- Port error 13 happens when closing. This is a Qt issue. It's known
  to not happen with Qt 5.4.1 . Not fatal.

## License
This software is licensed under GPLv3. See file COPYING for details.
