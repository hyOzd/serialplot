# SerialPlot

Small and simple software for plotting data from serial port in realtime.

![SerialPlot v0.4 Screenshot](http://i.imgur.com/Wb53LRt.png)

## Features

* Reading data from serial port
* Binary data formats (u)int8, (u)int16, (u)int32, float
* User defined frame format for robust operation
* ASCII input (Comma Separated Values)
* Synchronized multi channel plotting
* Define and send commands to the device in ASCII or binary format
* Take snapshots of the current waveform and save to CSV file

See
[hackaday.io](https://hackaday.io/project/5334-serialplot-realtime-plotting-software)
page for [screenshots](https://hackaday.io/project/5334/gallery).

## Installation

You can obtain windows and linux binary packages hackaday.io page:

https://hackaday.io/project/5334-serialplot-realtime-plotting-software

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
```apt install qtbase5-dev libqt5serialport5-dev cmake mercurial```

Under OpenSUSE:
```zypper in libqt5-qtbase-devel libqt5-qtserialbus-devel libqt5-qtserialport-devel cmake mercurial```

### Download and Install Qwt [Optional]

[Qwt](http://qwt.sourceforge.net) is the library that provides
plotting widgets for SerialPlot. You have 3 different options for Qwt.

* Leave it to serialplot build scripts. Qwt will be downloaded and built for you.

* If your linux distribution has the `libqwt-qt5-dev` or `qwt-qt5-devel` or,
  on OpenSUSE, `qwt6-qt5-devel` package, 
  install it and set `BUILD_QWT` cmake option to `false`.

* Download Qwt 6 [here](http://sourceforge.net/projects/qwt/files/)
  and build it yourself per these
  [instructions](http://qwt.sourceforge.net/qwtinstall.html#qwtinstall-unix)
  and install. Make sure you use Qt5 (not Qt4) to build. And set
  `BUILD_QWT` cmake option to `false`.

### Download and Build SerialPlot

You can use Mercurial to download SerialPlot source code. Or you can
download it from here:
https://hg.sr.ht/~hyozd/serialplot/

    hg clone https://hg.sr.ht/~hyozd/serialplot/
    cd serialplot
    mkdir build && cd build
    cmake ..
    make

You can also build with QtCreator IDE (or qmake) using `serialplot.pro` file.

## Known Issues

- On Ubuntu 16.04 / Linux Mint 18, removing USB serial device while it
  is open in SerialPlot, causes application to go into a nasty loop
  and eventually it crashes. This is a bug of Qt 5.5. I have uploaded
  a patched version of the problematic `libqt5serialport5` package to
  launchpad ppa. If you install SerialPlot from there or use AppImage binary, you shouldn't
  have any problems.

## License

This software is licensed under GPLv3. See file COPYING for details.
