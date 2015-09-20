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
#include <QLabel>
#include <QString>
#include <QVector>
#include <QList>
#include <QSerialPort>
#include <QSignalMapper>
#include <QTimer>
#include <QColor>
#include <QtGlobal>
#include <qwt_plot_curve.h>
#include <qwt_plot_textlabel.h>

#include "portcontrol.h"
#include "ui_about_dialog.h"
#include "framebuffer.h"
#include "snapshotmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void messageHandler(QtMsgType type, const QMessageLogContext &context,
                        const QString &msg);

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

    QDialog aboutDialog;
    void setupAboutDialog();

    QSerialPort serialPort;
    PortControl portControl;

    unsigned int numOfSamples;
    unsigned int numOfChannels;

    QList<QwtPlotCurve*> curves;
    // Note: FrameBuffer s are owned by their respective QwtPlotCurve s.
    QList<FrameBuffer*> channelBuffers;

    // `data` contains i th channels data
    void addChannelData(unsigned int channel, double* data, unsigned size);

    NumberFormat numberFormat;
    unsigned int sampleSize; // number of bytes in the selected number format
    double (MainWindow::*readSample)();

    // note that serialPort should already have enough bytes present
    template<typename T> double readSampleAs();

    bool skipByteRequested;

    const int SPS_UPDATE_TIMEOUT = 1;  // second
    QLabel spsLabel;
    unsigned int sampleCount;
    QTimer spsTimer;

    SnapshotManager snapshotMan;

    // demo
    QTimer demoTimer;
    int demoCount;
    bool isDemoRunning();
    QwtPlotTextLabel demoIndicator;

private slots:
    void onPortToggled(bool open);
    void onDataReady();      // used with binary number formats
    void onDataReadyASCII(); // used with ASCII number format
    void onPortError(QSerialPort::SerialPortError error);

    void skipByte();

    void onNumOfSamplesChanged(int value);
    void onAutoScaleChecked(bool checked);
    void onYScaleChanged();
    void onRangeSelected();

    void onNumOfChannelsChanged(int value);
    void onNumberFormatButtonToggled(int numberFormatId, bool checked);
    void selectNumberFormat(NumberFormat numberFormatId);

    void clearPlot();

    void spsTimerTimeout();

    void demoTimerTimeout();
    void enableDemo(bool enabled);

    void onExportCsv();
};

#endif // MAINWINDOW_H
