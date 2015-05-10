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
#include <QSerialPortInfo>
#include <QByteArray>
#include <QApplication>
#include <QtDebug>
#include <qwt_plot.h>
#include <limits.h>
#include <cmath>
#include "utils.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupAboutDialog();

    // init UI signals

    // menu signals
    QObject::connect(ui->actionHelpAbout, &QAction::triggered,
              &aboutDialog, &QWidget::show);

    // port tab signals
    QObject::connect(ui->pbReloadPorts, &QPushButton::clicked,
                     this, &MainWindow::loadPortList);

    QObject::connect(ui->pbOpenPort, &QPushButton::clicked,
                     this, &MainWindow::togglePort);

    QObject::connect(this, &MainWindow::portToggled,
                     this, &MainWindow::onPortToggled);

    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &MainWindow::selectPort);

    QObject::connect(ui->cbBaudRate,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &MainWindow::selectBaudRate);

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
    numberFormatButtons.addButton(ui->rbASCII,  NumberFormat_ASCII);

    QObject::connect(&numberFormatButtons, SIGNAL(buttonToggled(int, bool)),
                     this, SLOT(onNumberFormatButtonToggled(int, bool)));

    // setup parity selection buttons
    parityButtons.addButton(ui->rbNoParity, (int) QSerialPort::NoParity);
    parityButtons.addButton(ui->rbEvenParity, (int) QSerialPort::EvenParity);
    parityButtons.addButton(ui->rbOddParity, (int) QSerialPort::OddParity);

    QObject::connect(&parityButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &MainWindow::selectParity);

    // setup data bits selection buttons
    dataBitsButtons.addButton(ui->rb8Bits, (int) QSerialPort::Data8);
    dataBitsButtons.addButton(ui->rb7Bits, (int) QSerialPort::Data7);
    dataBitsButtons.addButton(ui->rb6Bits, (int) QSerialPort::Data6);
    dataBitsButtons.addButton(ui->rb5Bits, (int) QSerialPort::Data5);

    QObject::connect(&dataBitsButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &MainWindow::selectDataBits);

    // setup stop bits selection buttons
    stopBitsButtons.addButton(ui->rb1StopBit, (int) QSerialPort::OneStop);
    stopBitsButtons.addButton(ui->rb2StopBit, (int) QSerialPort::TwoStop);

    QObject::connect(&stopBitsButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &MainWindow::selectStopBits);

    // setup flow control selection buttons
    flowControlButtons.addButton(ui->rbNoFlowControl,
                                 (int) QSerialPort::NoFlowControl);
    flowControlButtons.addButton(ui->rbHardwareControl,
                                 (int) QSerialPort::HardwareControl);
    flowControlButtons.addButton(ui->rbSoftwareControl,
                                 (int) QSerialPort::SoftwareControl);

    QObject::connect(&flowControlButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &MainWindow::selectFlowControl);

    // init port signals
    QObject::connect(&(this->serialPort), SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(onPortError(QSerialPort::SerialPortError)));

    // init skip byte button
    skipByteRequested = false;
    QObject::connect(ui->pbSkipByte, &QPushButton::clicked,
                     this, &MainWindow::skipByte);

    loadPortList();
    loadBaudRateList();
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText("9600"));

    // set limits for axis limit boxes
    ui->spYmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    // init data arrays and plot

    numOfSamples = ui->spNumOfSamples->value();
    numOfChannels = 1;

    dataX.resize(numOfSamples);
    for (int i = 0; i < dataX.size(); i++)
    {
        dataX[i] = i;
    }

    // init channel data and curve list
    for (int i = 0; i < numOfChannels; i++)
    {
        channelsData.append(DataArray(numOfSamples, 0.0));
        curves.append(new QwtPlotCurve());
        curves[i]->setSamples(dataX, channelsData[i]);
        curves[i]->setPen(makeColor(i));
        curves[i]->attach(ui->plot);
    }

    // init number format
    if (numberFormatButtons.checkedId() >= 0)
    {
        selectNumberFormat((NumberFormat) numberFormatButtons.checkedId());
    }
    else
    {
        selectNumberFormat(NumberFormat_uint8);
    }

    // Init demo mode
    demoCount = 0;
    demoTimer.setInterval(100);
    QObject::connect(&demoTimer, &QTimer::timeout,
                     this, &MainWindow::demoTimerTimeout);
    QObject::connect(ui->actionDemoMode, &QAction::toggled,
                     this, &MainWindow::enableDemo);
}

MainWindow::~MainWindow()
{
    for (auto curve : curves)
    {
        delete curve;
    }

    if (serialPort.isOpen())
    {
        serialPort.close();
    }
    delete ui;
}

void MainWindow::setupAboutDialog()
{
    Ui_AboutDialog uiAboutDialog;
    uiAboutDialog.setupUi(&aboutDialog);

    QObject::connect(uiAboutDialog.pbAboutQt, &QPushButton::clicked,
                     [](){ QApplication::aboutQt();});

    QString aboutText = uiAboutDialog.lbAbout->text();
    aboutText.replace("$VERSION_STRING$", VERSION_STRING);
    uiAboutDialog.lbAbout->setText(aboutText);
}

void MainWindow::loadPortList()
{
    QString currentSelection = ui->cbPortList->currentText();

    ui->cbPortList->clear();

    for (auto port : QSerialPortInfo::availablePorts())
    {
        ui->cbPortList->addItem(port.portName());
    }

    // find current selection in the new list, maybe it doesn't exist anymore?
    int currentSelectionIndex = ui->cbPortList->findText(currentSelection);
    if (currentSelectionIndex >= 0)
    {
        ui->cbPortList->setCurrentIndex(currentSelectionIndex);
    }
    else // our port doesn't exist anymore, close port if it's open
    {
        if (serialPort.isOpen()) togglePort();
    }
}

void MainWindow::loadBaudRateList()
{
    ui->cbBaudRate->clear();

    for (auto baudRate : QSerialPortInfo::standardBaudRates())
    {
        ui->cbBaudRate->addItem(QString::number(baudRate));
    }
}

void MainWindow::togglePort()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
        qDebug() << "Port closed, " << serialPort.portName();
        emit portToggled(false);
    }
    else
    {
        serialPort.setPortName(ui->cbPortList->currentText());

        // open port
        if (serialPort.open(QIODevice::ReadWrite))
        {
            qDebug() << "Port opened, " << serialPort.portName();
            emit portToggled(true);

            // set baud rate
            if (!serialPort.setBaudRate(ui->cbBaudRate->currentText().toInt()))
            {
                qDebug() << "Set baud rate failed during port opening: "
                         << serialPort.error();
            }
        }
        else
        {
            qDebug() << "Port open error: " << serialPort.error();
        }
    }
}

void MainWindow::selectPort(QString portName)
{
    // has selection actually changed
    if (portName != serialPort.portName())
    {
        // if another port is already open, close it by toggling
        if (serialPort.isOpen())
        {
            togglePort();

            // open new selection by toggling
            togglePort();
        }
    }
}

void MainWindow::selectBaudRate(QString baudRate)
{
    if (serialPort.isOpen())
    {
        if (!serialPort.setBaudRate(baudRate.toInt()))
        {
            qDebug() << "Set baud rate failed during select: "
                     << serialPort.error();
        }
        else
        {
            qDebug() << "Baud rate changed: " << serialPort.baudRate();
        }
    }
}

void MainWindow::selectParity(int parity)
{
    if (serialPort.isOpen())
    {
        if(!serialPort.setParity((QSerialPort::Parity) parity))
        {
            qDebug() << "Set parity failed: " << serialPort.error();
        }
    }
}

void MainWindow::selectDataBits(int dataBits)
{
    if (serialPort.isOpen())
    {
        if(!serialPort.setDataBits((QSerialPort::DataBits) dataBits))
        {
            qDebug() << "Set data bits failed: " << serialPort.error();
        }
    }
}

void MainWindow::selectStopBits(int stopBits)
{
    if (serialPort.isOpen())
    {
        if(!serialPort.setStopBits((QSerialPort::StopBits) stopBits))
        {
            qDebug() << "Set stop bits failed: " << serialPort.error();
        }
    }
}

void MainWindow::selectFlowControl(int flowControl)
{
    if (serialPort.isOpen())
    {
        if(!serialPort.setFlowControl((QSerialPort::FlowControl) flowControl))
        {
            qDebug() << "Set flow control failed: " << serialPort.error();
        }
    }
}

void MainWindow::onPortToggled(bool open)
{
    ui->pbOpenPort->setChecked(open);
    // make sure demo mode is disabled
    if (open && isDemoRunning()) enableDemo(false);
    ui->actionDemoMode->setEnabled(!open);
}

void MainWindow::onDataReady()
{
    if (!ui->actionPause->isChecked())
    {
        // a package is a set of channel data like {CHAN0_SAMPLE, CHAN1_SAMPLE...}
        int packageSize = sampleSize * numOfChannels;
        int bytesAvailable = serialPort.bytesAvailable();

        if (bytesAvailable > 0 && skipByteRequested)
        {
            serialPort.read(1);
            skipByteRequested = false;
            bytesAvailable--;
        }

        if (bytesAvailable < packageSize)
        {
            return;
        }
        else
        {
            int numOfPackagesToRead =
                (bytesAvailable - (bytesAvailable % packageSize)) / packageSize;
            QVector<DataArray> channelSamples(numOfChannels);
            for (int ci = 0; ci < numOfChannels; ci++)
            {
                channelSamples[ci].resize(numOfPackagesToRead);
            }

            int i = 0;
            while(i < numOfPackagesToRead)
            {
                for (int ci = 0; ci < numOfChannels; ci++)
                {
                    channelSamples[ci].replace(i, (this->*readSample)());
                }
                i++;
            }

            for (int ci = 0; ci < numOfChannels; ci++)
            {
                addChannelData(ci, channelSamples[ci]);
            }
        }
    }
    else
    {
        serialPort.clear(QSerialPort::Input);
    }
}

void MainWindow::onDataReadyASCII()
{
    while(serialPort.canReadLine())
    {
        QByteArray line = serialPort.readLine();
        line = line.trimmed();
        auto separatedValues = line.split(',');

        if (separatedValues.length() >= numOfChannels)
        {
            for (int ci = 0; ci < numOfChannels; ci++)
            {
                double channelSample = separatedValues[ci].toDouble();
                addChannelData(ci, DataArray({channelSample}));
            }
        }
        else // there is missing channel data
        {
            qDebug() << "Incoming data is missing data for some channels!";
            for (int ci = 0; ci < separatedValues.length(); ci++)
            {
                double channelSample = separatedValues[ci].toDouble();
                addChannelData(ci, DataArray({channelSample}));
            }
        }
    }
}

void MainWindow::onPortError(QSerialPort::SerialPortError error)
{
    switch(error)
    {
        case QSerialPort::NoError :
            break;
        case QSerialPort::ResourceError :
            qDebug() << "Port error: resource unavaliable; most likely device removed.";
            if (serialPort.isOpen())
            {
                qDebug() << "Closing port on resource error: " << serialPort.portName();
                togglePort();
            }
            loadPortList();
            break;
        default:
            qDebug() << "Unhandled port error: " << error;
            break;
    }

}

void MainWindow::skipByte()
{
    skipByteRequested = true;
}

void MainWindow::addChannelData(unsigned int channel, DataArray data)
{
    DataArray* channelDataArray = &(channelsData[channel]);
    int offset = numOfSamples - data.size();

    if (offset < 0)
    {
        for (int i = 0; i < numOfSamples; i++)
        {
            (*channelDataArray)[i] = data[i - offset];
        }
    }
    else if (offset == 0)
    {
        (*channelDataArray) = data;
    }
    else
    {
        // shift old samples
        int shift = data.size();
        for (int i = 0; i < offset; i++)
        {
            (*channelDataArray)[i] = (*channelDataArray)[i + shift];
        }
        // place new samples
        for (int i = 0; i < data.size(); i++)
        {
            (*channelDataArray)[offset + i] = data[i];
        }
    }

    // update plot
    curves[channel]->setSamples(dataX, (*channelDataArray));
    ui->plot->replot(); // TODO: replot after all channel data updated
}

void MainWindow::clearPlot()
{
    for (int ci = 0; ci < numOfChannels; ci++)
    {
        channelsData[ci].fill(0.0);
        curves[ci]->setSamples(dataX, channelsData[ci]);
    }

    // update plot
    ui->plot->replot();
}

void MainWindow::onNumOfSamplesChanged(int value)
{
    unsigned int oldNum = this->numOfSamples;
    numOfSamples = value;

    // resize data arrays
    if (numOfSamples < oldNum)
    {
        dataX.resize(numOfSamples);
        for (int ci = 0; ci < numOfChannels; ci++)
        {
            channelsData[ci].remove(0, oldNum - numOfSamples);
        }
    }
    else if(numOfSamples > oldNum)
    {
        dataX.resize(numOfSamples);
        for (unsigned int i = oldNum; i < numOfSamples; i++)
        {
            dataX[i] = i;
            for (int ci = 0; ci < numOfChannels; ci++)
            {
                channelsData[ci].prepend(0);
            }
        }
    }
}

void MainWindow::onNumOfChannelsChanged(int value)
{
    unsigned int oldNum = this->numOfChannels;
    this->numOfChannels = value;

    if (numOfChannels > oldNum)
    {
        // add new channels
        for (int i = 0; i < numOfChannels - oldNum; i++)
        {
            channelsData.append(DataArray(numOfSamples, 0.0));
            curves.append(new QwtPlotCurve());
            curves.last()->setSamples(dataX, channelsData.last());
            curves.last()->setPen(makeColor(curves.length()-1));
            curves.last()->attach(ui->plot);
        }
    }
    else if(numOfChannels < oldNum)
    {
        // remove channels
        for (int i = 0; i < oldNum - numOfChannels; i++)
        {
            channelsData.removeLast();
            auto curve = curves.takeLast();
            curve->detach();
            delete curve;
        }
    }
}

void MainWindow::onAutoScaleChecked(bool checked)
{
    if (checked)
    {
        ui->plot->setAxisAutoScale(QwtPlot::yLeft);
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

        ui->plot->setAxisScale(QwtPlot::yLeft, ui->spYmin->value(),
                               ui->spYmax->value());
    }
}

void MainWindow::onYScaleChanged()
{
    ui->plot->setAxisScale(QwtPlot::yLeft, ui->spYmin->value(),
                           ui->spYmax->value());
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

    }
    else
    {
        QObject::disconnect(&(this->serialPort), &QSerialPort::readyRead, 0, 0);
        QObject::connect(&(this->serialPort), &QSerialPort::readyRead,
                         this, &MainWindow::onDataReady);
    }
}

template<typename T> double MainWindow::readSampleAs()
{
    T data;
    this->serialPort.read((char*) &data, sizeof(data));
    return double(data);
}

bool MainWindow::isDemoRunning()
{
    return ui->actionDemoMode->isChecked();
}

void MainWindow::demoTimerTimeout()
{
    demoCount++;
    if (demoCount > 100) demoCount = 0;

    for (int ci = 0; ci < numOfChannels; ci++)
    {
        DataArray data(1);
        data.replace(0, (ci + 1)*demoCount);
        addChannelData(ci, data);
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
    }
}

/*
  Below crude drawing demostrates how color selection occurs for
  given channel index

  0°                     <--Hue Value-->                           360°
  |* . o . + . o . * . o . + . o . * . o . + . o . * . o . + . o . |

  * -> 0-3
  + -> 4-7
  o -> 8-15
  . -> 16-31

 */

QColor MainWindow::makeColor(unsigned int channelIndex)
{
    auto i = channelIndex;

    if (i < 4)
    {
        return QColor::fromHsv(360*i/4, 255, 255);
    }
    else
    {
        double p = floor(log2(i));
        double n = pow(2, p);
        i = i - n;
        return QColor::fromHsv(360*i/n + 360/pow(2,p+1), 255, 255);
    }
}
