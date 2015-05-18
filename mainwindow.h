/*
  Copyright © 2015 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QString>
#include <QVector>
#include <QList>
#include <QSerialPort>
#include <QSignalMapper>
#include <QTimer>
#include <QColor>
#include <qwt_plot_curve.h>

#include "ui_about_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    enum NumberFormat
    {
        NumberFormat_uint8,
        NumberFormat_uint16,
        NumberFormat_uint32,
        NumberFormat_int8,
        NumberFormat_int16,
        NumberFormat_int32,
        NumberFormat_float,
        NumberFormat_ASCII
    };

    Ui::MainWindow *ui;
    QButtonGroup numberFormatButtons;
    QButtonGroup parityButtons;
    QButtonGroup dataBitsButtons;
    QButtonGroup stopBitsButtons;
    QButtonGroup flowControlButtons;

    QDialog aboutDialog;
    void setupAboutDialog();

    QSerialPort serialPort;

    unsigned int numOfSamples;
    unsigned int numOfChannels;

    QList<QwtPlotCurve*> curves;
    typedef QVector<double> DataArray;
    DataArray dataX;   // array that simply contains numbers 0..numberOfSamples
    QList<DataArray> channelsData;

    // `data` contains channel specific data
    void addChannelData(unsigned int channel, DataArray data);

    NumberFormat numberFormat;
    unsigned int sampleSize; // number of bytes in the selected number format
    double (MainWindow::*readSample)();

    // note that serialPort should already have enough bytes present
    template<typename T> double readSampleAs();

    bool skipByteRequested;

    // demo
    QTimer demoTimer;
    int demoCount;
    bool isDemoRunning();

    QColor makeColor(unsigned int channelIndex);

private slots:
    void loadPortList();
    void loadBaudRateList();
    void togglePort();
    void selectPort(QString portName);
    void onPortToggled(bool open);
    void selectBaudRate(QString baudRate);
    void selectParity(int parity); // parity must be one of QSerialPort::Parity
    void selectDataBits(int dataBits); // bits must be one of QSerialPort::DataBits
    void selectStopBits(int stopBits); // stopBits must be one of QSerialPort::StopBits
    void selectFlowControl(int flowControl); // flowControl must be one of QSerialPort::FlowControl

    void onDataReady();      // used with binary number formats
    void onDataReadyASCII(); // used with ASCII number format
    void onPortError(QSerialPort::SerialPortError error);

    void skipByte();

    void onNumOfSamplesChanged(int value);
    void onAutoScaleChecked(bool checked);
    void onYScaleChanged();

    void onNumOfChannelsChanged(int value);
    void onNumberFormatButtonToggled(int numberFormatId, bool checked);
    void selectNumberFormat(NumberFormat numberFormatId);

    void clearPlot();

    void demoTimerTimeout();
    void enableDemo(bool enabled);

signals:
    void portToggled(bool open);
};

#endif // MAINWINDOW_H
