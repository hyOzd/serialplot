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
#include "commandpanel.h"
#include "dataformatpanel.h"
#include "ui_about_dialog.h"
#include "framebuffer.h"
#include "channelmanager.h"
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
    Ui::MainWindow *ui;

    QDialog aboutDialog;
    void setupAboutDialog();

    QSerialPort serialPort;
    PortControl portControl;

    unsigned int numOfSamples;

    QList<QwtPlotCurve*> curves;
    // Note: FrameBuffer s are owned by their respective QwtPlotCurve s.
    // QList<FrameBuffer*> channelBuffers;
    ChannelManager channelMan;

    QLabel spsLabel;

    CommandPanel commandPanel;
    DataFormatPanel dataFormatPanel;

    SnapshotManager snapshotMan;

    QwtPlotTextLabel demoIndicator;
    bool isDemoRunning();

private slots:
    void onPortToggled(bool open);
    void onPortError(QSerialPort::SerialPortError error);

    void onNumOfSamplesChanged(int value);
    void onAutoScaleChecked(bool checked);
    void onYScaleChanged();
    void onRangeSelected();
    void onNumOfChannelsChanged(unsigned value);
    void onChannelNameChanged(unsigned channel, QString name);

    void clearPlot();

    void onSpsChanged(unsigned sps);

    void enableDemo(bool enabled);

    void onExportCsv();
};

#endif // MAINWINDOW_H
