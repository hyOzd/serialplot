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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <QtEndian>
#include <qwt_plot.h>
#include <limits.h>
#include <cmath>
#include <iostream>

#include <plot.h>

#include "utils.h"
#include "version.h"
#include "floatswap.h"

#if defined(Q_OS_WIN) && defined(QT_STATIC)
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

struct Range
{
    double rmin;
    double rmax;
};

Q_DECLARE_METATYPE(Range);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    portControl(&serialPort),
    snapshotMan(this, &channelBuffers)
{
    ui->setupUi(this);
    ui->tabWidget->insertTab(0, &portControl, "Port");
    ui->tabWidget->setCurrentIndex(0);
    addToolBar(portControl.toolBar());

    ui->mainToolBar->addAction(snapshotMan.takeSnapshotAction());
    ui->menuBar->insertMenu(ui->menuHelp->menuAction(), snapshotMan.menu());

    setupAboutDialog();

    // init view menu
    for (auto a : ui->plot->menuActions())
    {
        ui->menuView->addAction(a);
    }

    // init UI signals

    // menu signals
    QObject::connect(ui->actionHelpAbout, &QAction::triggered,
              &aboutDialog, &QWidget::show);

    QObject::connect(ui->actionExportCsv, &QAction::triggered,
                     this, &MainWindow::onExportCsv);

    ui->actionQuit->setShortcutContext(Qt::ApplicationShortcut);

    QObject::connect(ui->actionQuit, &QAction::triggered,
                     this, &MainWindow::close);

    // port control signals
    QObject::connect(&portControl, &PortControl::portToggled,
                     this, &MainWindow::onPortToggled);

    QObject::connect(&portControl, &PortControl::skipByteRequested,
                     this, &MainWindow::skipByte);

    QObject::connect(ui->spNumOfSamples, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
                     this, &MainWindow::onNumOfSamplesChanged);

    QObject::connect(ui->cbAutoScale, &QCheckBox::toggled,
                     this, &MainWindow::onAutoScaleChecked);

    QObject::connect(ui->spYmin, SIGNAL(valueChanged(double)),
                     this, SLOT(onYScaleChanged()));

    QObject::connect(ui->spYmax, SIGNAL(valueChanged(double)),
                     this, SLOT(onYScaleChanged()));

    QObject::connect(ui->actionClear, SIGNAL(triggered(bool)),
                     this, SLOT(clearPlot()));

    // QObject::connect(ui->actionSnapShot, SIGNAL(triggered(bool)),
    //                  this, SLOT(takeSnapShot()));

    // setup number of channels spinbox
    QObject::connect(ui->spNumOfChannels,
                     SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
                     this, &MainWindow::onNumOfChannelsChanged);

    // setup number format buttons
    numberFormatButtons.addButton(ui->rbUint8,  NumberFormat_uint8);
    numberFormatButtons.addButton(ui->rbUint16, NumberFormat_uint16);
    numberFormatButtons.addButton(ui->rbUint32, NumberFormat_uint32);
    numberFormatButtons.addButton(ui->rbInt8,   NumberFormat_int8);
    numberFormatButtons.addButton(ui->rbInt16,  NumberFormat_int16);
    numberFormatButtons.addButton(ui->rbInt32,  NumberFormat_int32);
    numberFormatButtons.addButton(ui->rbFloat,  NumberFormat_float);
    numberFormatButtons.addButton(ui->rbASCII,  NumberFormat_ASCII);

    QObject::connect(&numberFormatButtons, SIGNAL(buttonToggled(int, bool)),
                     this, SLOT(onNumberFormatButtonToggled(int, bool)));

    // init port signals
    QObject::connect(&(this->serialPort), SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(onPortError(QSerialPort::SerialPortError)));

    // set limits for axis limit boxes
    ui->spYmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    // init data arrays and plot

    numOfSamples = ui->spNumOfSamples->value();
    numOfChannels = ui->spNumOfChannels->value();

    // init channel data and curve list
    for (unsigned int i = 0; i < numOfChannels; i++)
    {
        channelBuffers.append(new FrameBuffer(numOfSamples));
        curves.append(new QwtPlotCurve());
        curves[i]->setSamples(channelBuffers[i]);
        curves[i]->setPen(Plot::makeColor(i));
        curves[i]->attach(ui->plot);
    }

    // init auto scale
    ui->plot->setAxis(ui->cbAutoScale->isChecked(),
                      ui->spYmin->value(), ui->spYmax->value());

    // init scale range preset list
    for (int nbits = 8; nbits <= 24; nbits++) // signed binary formats
    {
        int rmax = pow(2, nbits-1)-1;
        int rmin = -rmax-1;
        Range r = {double(rmin),  double(rmax)};
        ui->cbRangePresets->addItem(
            QString().sprintf("Signed %d bits %d to +%d", nbits, rmin, rmax),
            QVariant::fromValue(r));
    }
    for (int nbits = 8; nbits <= 24; nbits++) // unsigned binary formats
    {
        int rmax = pow(2, nbits)-1;
        ui->cbRangePresets->addItem(
            QString().sprintf("Unsigned %d bits %d to +%d", nbits, 0, rmax),
            QVariant::fromValue(Range{0, double(rmax)}));
    }
    ui->cbRangePresets->addItem("-1 to +1", QVariant::fromValue(Range{-1, +1}));
    ui->cbRangePresets->addItem("0 to +1", QVariant::fromValue(Range{0, +1}));
    ui->cbRangePresets->addItem("-100 to +100", QVariant::fromValue(Range{-100, +100}));
    ui->cbRangePresets->addItem("0 to +100", QVariant::fromValue(Range{0, +100}));

    QObject::connect(ui->cbRangePresets, SIGNAL(activated(int)),
                     this, SLOT(onRangeSelected()));

    // init number format
    if (numberFormatButtons.checkedId() >= 0)
    {
        selectNumberFormat((NumberFormat) numberFormatButtons.checkedId());
    }
    else
    {
        selectNumberFormat(NumberFormat_uint8);
    }

    // Init sps (sample per second) counter
    sampleCount = 0;
    spsLabel.setText("0sps");
    spsLabel.setToolTip("samples per second (total of all channels)");
    ui->statusBar->addPermanentWidget(&spsLabel);
    spsTimer.start(SPS_UPDATE_TIMEOUT * 1000);
    QObject::connect(&spsTimer, &QTimer::timeout,
                     this, &MainWindow::spsTimerTimeout);

    // Init demo mode
    demoCount = 0;
    demoTimer.setInterval(100);
    QObject::connect(&demoTimer, &QTimer::timeout,
                     this, &MainWindow::demoTimerTimeout);
    QObject::connect(ui->actionDemoMode, &QAction::toggled,
                     this, &MainWindow::enableDemo);

    {   // init demo indicator
        QwtText demoText(" DEMO RUNNING ");  // looks better with spaces
        demoText.setColor(QColor("white"));
        demoText.setBackgroundBrush(Qt::darkRed);
        demoText.setBorderRadius(4);
        demoText.setRenderFlags(Qt::AlignLeft | Qt::AlignTop);
        demoIndicator.setText(demoText);
        demoIndicator.hide();
        demoIndicator.attach(ui->plot);
    }

}

MainWindow::~MainWindow()
{
    for (auto curve : curves)
    {
        // also deletes respective FrameBuffer
        delete curve;
    }

    if (serialPort.isOpen())
    {
        serialPort.close();
    }

    delete ui;
    ui = NULL; // we check if ui is deleted in messageHandler
}

void MainWindow::setupAboutDialog()
{
    Ui_AboutDialog uiAboutDialog;
    uiAboutDialog.setupUi(&aboutDialog);

    QObject::connect(uiAboutDialog.pbAboutQt, &QPushButton::clicked,
                     [](){ QApplication::aboutQt();});

    QString aboutText = uiAboutDialog.lbAbout->text();
    aboutText.replace("$VERSION_STRING$", VERSION_STRING);
    aboutText.replace("$VERSION_REVISION$", VERSION_REVISION);
    uiAboutDialog.lbAbout->setText(aboutText);
}

void MainWindow::onPortToggled(bool open)
{
    // make sure demo mode is disabled
    if (open && isDemoRunning()) enableDemo(false);
    ui->actionDemoMode->setEnabled(!open);
}

void MainWindow::onDataReady()
{
    if (ui->actionPause->isChecked())
    {
        serialPort.clear(QSerialPort::Input);
        return;
    }

    // a package is a set of channel data like {CHAN0_SAMPLE, CHAN1_SAMPLE...}
    int packageSize = sampleSize * numOfChannels;
    int bytesAvailable = serialPort.bytesAvailable();

    if (bytesAvailable > 0 && skipByteRequested)
    {
        serialPort.read(1);
        skipByteRequested = false;
        bytesAvailable--;
    }

    if (bytesAvailable < packageSize) return;

    int numOfPackagesToRead =
        (bytesAvailable - (bytesAvailable % packageSize)) / packageSize;
    double* channelSamples = new double[numOfPackagesToRead*numOfChannels];

    int i = 0;
    while(i < numOfPackagesToRead)
    {
        for (unsigned int ci = 0; ci < numOfChannels; ci++)
        {
            // channelSamples[ci].replace(i, (this->*readSample)());
            channelSamples[ci*numOfPackagesToRead+i] = (this->*readSample)();
        }
        i++;
    }

    for (unsigned int ci = 0; ci < numOfChannels; ci++)
    {
        addChannelData(ci,
                       channelSamples + ci*numOfPackagesToRead,
                       numOfPackagesToRead);
    }
    ui->plot->replot();

    delete channelSamples;
}

void MainWindow::onDataReadyASCII()
{
    while(serialPort.canReadLine())
    {
        QByteArray line = serialPort.readLine();

        // discard data if paused
        if (ui->actionPause->isChecked())
        {
            return;
        }

        line = line.trimmed();
        auto separatedValues = line.split(',');

        int numReadChannels; // effective number of channels to read
        if (separatedValues.length() >= int(numOfChannels))
        {
            numReadChannels = numOfChannels;
        }
        else // there is missing channel data
        {
            numReadChannels = separatedValues.length();
            qWarning() << "Incoming data is missing data for some channels!";
        }

        // parse read line
        for (int ci = 0; ci < numReadChannels; ci++)
        {
            bool ok;
            double channelSample = separatedValues[ci].toDouble(&ok);
            if (ok)
            {
                addChannelData(ci, &channelSample, 1);
            }
            else
            {
                qWarning() << "Data parsing error for channel: " << ci;
            }
        }
        ui->plot->replot();
    }
}

void MainWindow::onPortError(QSerialPort::SerialPortError error)
{
    switch(error)
    {
        case QSerialPort::NoError :
            break;
        case QSerialPort::ResourceError :
            qWarning() << "Port error: resource unavaliable; most likely device removed.";
            if (serialPort.isOpen())
            {
                qWarning() << "Closing port on resource error: " << serialPort.portName();
                portControl.togglePort();
            }
            portControl.loadPortList();
            break;
        case QSerialPort::DeviceNotFoundError:
            qCritical() << "Device doesn't exists: " << serialPort.portName();
            break;
        case QSerialPort::PermissionError:
            qCritical() << "Permission denied. Either you don't have \
required privileges or device is already opened by another process.";
            break;
        case QSerialPort::OpenError:
            qWarning() << "Device is already opened!";
            break;
        case QSerialPort::NotOpenError:
            qCritical() << "Device is not open!";
            break;
        case QSerialPort::ParityError:
            qCritical() << "Parity error detected.";
            break;
        case QSerialPort::FramingError:
            qCritical() << "Framing error detected.";
            break;
        case QSerialPort::BreakConditionError:
            qCritical() << "Break condition is detected.";
            break;
        case QSerialPort::WriteError:
            qCritical() << "An error occurred while writing data.";
            break;
        case QSerialPort::ReadError:
            qCritical() << "An error occurred while reading data.";
            break;
        case QSerialPort::UnsupportedOperationError:
            qCritical() << "Operation is not supported.";
            break;
        case QSerialPort::TimeoutError:
            qCritical() << "A timeout error occurred.";
            break;
        case QSerialPort::UnknownError:
            qCritical() << "Unknown error!";
            break;
        default:
            qCritical() << "Unhandled port error: " << error;
            break;
    }
}

void MainWindow::skipByte()
{
    skipByteRequested = true;
}

void MainWindow::addChannelData(unsigned int channel, double* data, unsigned size)
{
    channelBuffers[channel]->addSamples(data, size);
    sampleCount += size;
}

void MainWindow::clearPlot()
{
    for (unsigned int ci = 0; ci < numOfChannels; ci++)
    {
        channelBuffers[ci]->clear();
    }
    ui->plot->replot();
}

void MainWindow::onNumOfSamplesChanged(int value)
{
    numOfSamples = value;

    for (unsigned int ci = 0; ci < numOfChannels; ci++)
    {
        channelBuffers[ci]->resize(numOfSamples);
    }

    ui->plot->replot();
}

void MainWindow::onNumOfChannelsChanged(int value)
{
    unsigned int oldNum = this->numOfChannels;
    this->numOfChannels = value;

    if (numOfChannels > oldNum)
    {
        // add new channels
        for (unsigned int i = 0; i < numOfChannels - oldNum; i++)
        {
            channelBuffers.append(new FrameBuffer(numOfSamples));
            curves.append(new QwtPlotCurve());
            curves.last()->setSamples(channelBuffers.last());
            curves.last()->setPen(Plot::makeColor(curves.length()-1));
            curves.last()->attach(ui->plot);
        }
    }
    else if(numOfChannels < oldNum)
    {
        // remove channels
        for (unsigned int i = 0; i < oldNum - numOfChannels; i++)
        {
            // also deletes owned FrameBuffer
            delete curves.takeLast();
            channelBuffers.removeLast();
        }
    }
}

void MainWindow::onAutoScaleChecked(bool checked)
{
    if (checked)
    {
        ui->plot->setAxis(true);
        ui->lYmin->setEnabled(false);
        ui->lYmax->setEnabled(false);
        ui->spYmin->setEnabled(false);
        ui->spYmax->setEnabled(false);
    }
    else
    {
        ui->lYmin->setEnabled(true);
        ui->lYmax->setEnabled(true);
        ui->spYmin->setEnabled(true);
        ui->spYmax->setEnabled(true);

        ui->plot->setAxis(false,  ui->spYmin->value(), ui->spYmax->value());
    }
}

void MainWindow::onYScaleChanged()
{
    ui->plot->setAxis(false,  ui->spYmin->value(), ui->spYmax->value());
}

void MainWindow::onRangeSelected()
{
    Range r = ui->cbRangePresets->currentData().value<Range>();
    ui->spYmin->setValue(r.rmin);
    ui->spYmax->setValue(r.rmax);
    ui->cbAutoScale->setChecked(false);
}

void MainWindow::onNumberFormatButtonToggled(int numberFormatId, bool checked)
{
    if (checked) selectNumberFormat((NumberFormat) numberFormatId);
}

void MainWindow::selectNumberFormat(NumberFormat numberFormatId)
{
    numberFormat = numberFormatId;

    switch(numberFormat)
    {
        case NumberFormat_uint8:
            sampleSize = 1;
            readSample = &MainWindow::readSampleAs<quint8>;
            break;
        case NumberFormat_int8:
            sampleSize = 1;
            readSample = &MainWindow::readSampleAs<qint8>;
            break;
        case NumberFormat_uint16:
            sampleSize = 2;
            readSample = &MainWindow::readSampleAs<quint16>;
            break;
        case NumberFormat_int16:
            sampleSize = 2;
            readSample = &MainWindow::readSampleAs<qint16>;
            break;
        case NumberFormat_uint32:
            sampleSize = 4;
            readSample = &MainWindow::readSampleAs<quint32>;
            break;
        case NumberFormat_int32:
            sampleSize = 4;
            readSample = &MainWindow::readSampleAs<qint32>;
            break;
        case NumberFormat_float:
            sampleSize = 4;
            readSample = &MainWindow::readSampleAs<float>;
            break;
        case NumberFormat_ASCII:
            sampleSize = 0;    // these two members should not be used
            readSample = NULL; // in this mode
            break;
    }

    if (numberFormat == NumberFormat_ASCII)
    {
        QObject::disconnect(&(this->serialPort), &QSerialPort::readyRead, 0, 0);
        QObject::connect(&(this->serialPort), &QSerialPort::readyRead,
                         this, &MainWindow::onDataReadyASCII);
        portControl.enableSkipByte();
    }
    else
    {
        QObject::disconnect(&(this->serialPort), &QSerialPort::readyRead, 0, 0);
        QObject::connect(&(this->serialPort), &QSerialPort::readyRead,
                         this, &MainWindow::onDataReady);
        portControl.enableSkipByte(false);
    }
}

template<typename T> double MainWindow::readSampleAs()
{
    T data;
    this->serialPort.read((char*) &data, sizeof(data));

    if (ui->rbLittleE->isChecked())
    {
        data = qFromLittleEndian(data);
    }
    else
    {
        data = qFromBigEndian(data);
    }

    return double(data);
}

bool MainWindow::isDemoRunning()
{
    return ui->actionDemoMode->isChecked();
}

void MainWindow::spsTimerTimeout()
{
    spsLabel.setText(QString::number(sampleCount/SPS_UPDATE_TIMEOUT) + "sps");
    sampleCount = 0;
}

void MainWindow::demoTimerTimeout()
{
    const double period = 100;
    demoCount++;
    if (demoCount > 100) demoCount = 0;

    if (!ui->actionPause->isChecked())
    {
        for (unsigned int ci = 0; ci < numOfChannels; ci++)
        {
            // we are calculating the fourier components of square wave
            double value = 4*sin(2*M_PI*double((ci+1)*demoCount)/period)/((2*(ci+1))*M_PI);
            addChannelData(ci, &value, 1);
        }
        ui->plot->replot();
    }
}

void MainWindow::enableDemo(bool enabled)
{
    if (enabled)
    {
        if (!serialPort.isOpen())
        {
            demoTimer.start();
            ui->actionDemoMode->setChecked(true);
            demoIndicator.show();
            ui->plot->replot();
        }
        else
        {
            ui->actionDemoMode->setChecked(false);
        }
    }
    else
    {
        demoTimer.stop();
        ui->actionDemoMode->setChecked(false);
        demoIndicator.hide();
        ui->plot->replot();
    }
}

void MainWindow::onExportCsv()
{
    bool wasPaused = ui->actionPause->isChecked();
    ui->actionPause->setChecked(true); // pause plotting

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export CSV File"));

    if (fileName.isNull())  // user canceled export
    {
        ui->actionPause->setChecked(wasPaused);
    }
    else
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream fileStream(&file);

            for (unsigned int ci = 0; ci < numOfChannels; ci++)
            {
                fileStream << "Channel " << ci;
                if (ci != numOfChannels-1) fileStream << ",";
            }
            fileStream << '\n';

            for (unsigned int i = 0; i < numOfSamples; i++)
            {
                for (unsigned int ci = 0; ci < numOfChannels; ci++)
                {
                    fileStream << channelBuffers[ci]->sample(i).y();
                    if (ci != numOfChannels-1) fileStream << ",";
                }
                fileStream << '\n';
            }
        }
        else
        {
            qCritical() << "File open error during export: " << file.error();
        }
    }
}

void MainWindow::messageHandler(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg)
{
    QString logString;

    switch (type)
    {
        case QtDebugMsg:
            logString = "[Debug] " + msg;
            break;
        case QtWarningMsg:
            logString = "[Warning] " + msg;
            break;
        case QtCriticalMsg:
            logString = "[Error] " + msg;
            break;
        case QtFatalMsg:
            logString = "[Fatal] " + msg;
            break;
    }

    if (ui != NULL) ui->ptLog->appendPlainText(logString);
    std::cerr << logString.toStdString() << std::endl;

    if (type != QtDebugMsg && ui != NULL)
    {
        ui->statusBar->showMessage(msg, 5000);
    }
}
