
Small and simple software for plotting data from serial port.

# Dependencies
- Qt 5, including serialport module
- qwt 6.1
- libudev-dev (required for QSerialPort)

# Known Issues
- Port error 13 happens when closing. This is a Qt issue. It's known
  to not happen Qt 5.4.1 .
