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

#include "portcontrol.h"
#include "ui_portcontrol.h"

#include <QSerialPortInfo>
#include <QtDebug>
#include "utils.h"

PortControl::PortControl(QSerialPort* port, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PortControl)
{
    ui->setupUi(this);

    serialPort = port;

    QObject::connect(ui->pbReloadPorts, &QPushButton::clicked,
                     this, &PortControl::loadPortList);

    QObject::connect(ui->pbOpenPort, &QPushButton::clicked,
                     this, &PortControl::togglePort);

    // TODO: port name coming from combobox is dirty, create a separate layer of signals
    //       that will sanitize this information
    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::selectPort);

    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::onPortNameChanged);

    QObject::connect(ui->cbBaudRate,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::selectBaudRate);

    // setup parity selection buttons
    parityButtons.addButton(ui->rbNoParity, (int) QSerialPort::NoParity);
    parityButtons.addButton(ui->rbEvenParity, (int) QSerialPort::EvenParity);
    parityButtons.addButton(ui->rbOddParity, (int) QSerialPort::OddParity);

    QObject::connect(&parityButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &PortControl::selectParity);

    // setup data bits selection buttons
    dataBitsButtons.addButton(ui->rb8Bits, (int) QSerialPort::Data8);
    dataBitsButtons.addButton(ui->rb7Bits, (int) QSerialPort::Data7);
    dataBitsButtons.addButton(ui->rb6Bits, (int) QSerialPort::Data6);
    dataBitsButtons.addButton(ui->rb5Bits, (int) QSerialPort::Data5);

    QObject::connect(&dataBitsButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &PortControl::selectDataBits);

    // setup stop bits selection buttons
    stopBitsButtons.addButton(ui->rb1StopBit, (int) QSerialPort::OneStop);
    stopBitsButtons.addButton(ui->rb2StopBit, (int) QSerialPort::TwoStop);

    QObject::connect(&stopBitsButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &PortControl::selectStopBits);

    // setup flow control selection buttons
    flowControlButtons.addButton(ui->rbNoFlowControl,
                                 (int) QSerialPort::NoFlowControl);
    flowControlButtons.addButton(ui->rbHardwareControl,
                                 (int) QSerialPort::HardwareControl);
    flowControlButtons.addButton(ui->rbSoftwareControl,
                                 (int) QSerialPort::SoftwareControl);

    QObject::connect(&flowControlButtons,
                     SELECT<int>::OVERLOAD_OF(&QButtonGroup::buttonClicked),
                     this, &PortControl::selectFlowControl);

    // init skip byte button
    QObject::connect(ui->pbSkipByte, &QPushButton::clicked,
                     [this](){emit skipByteRequested();});

    loadPortList();
    loadBaudRateList();
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText("9600"));
}

PortControl::~PortControl()
{
    delete ui;
}

void PortControl::loadPortList()
{
    QString currentSelection = ui->cbPortList->currentText();

    ui->cbPortList->clear();

    discoveredPorts.clear();
    for (auto port : QSerialPortInfo::availablePorts())
    {
        QString pName = port.portName();
        if (!port.description().isEmpty()) pName += QString(" ") + port.description();
        if (port.hasProductIdentifier())
        {
            QString vID = QString("%1").arg(port.vendorIdentifier(), 4, 16, QChar('0'));
            QString pID = QString("%1").arg(port.productIdentifier(), 4, 16, QChar('0'));
            pName = pName + " [" + vID + ":" + pID + "]";
        }
        ui->cbPortList->addItem(pName);
        discoveredPorts << port.portName();
    }

    ui->cbPortList->addItems(userEnteredPorts);

    // find current selection in the new list, maybe it doesn't exist anymore?
    int currentSelectionIndex = ui->cbPortList->findText(currentSelection);
    if (currentSelectionIndex >= 0)
    {
        ui->cbPortList->setCurrentIndex(currentSelectionIndex);
    }
    else // our port doesn't exist anymore, close port if it's open
    {
        if (serialPort->isOpen()) togglePort();
    }
}

void PortControl::loadBaudRateList()
{
    ui->cbBaudRate->clear();

    for (auto baudRate : QSerialPortInfo::standardBaudRates())
    {
        ui->cbBaudRate->addItem(QString::number(baudRate));
    }
}

void PortControl::selectBaudRate(QString baudRate)
{
    if (serialPort->isOpen())
    {
        if (!serialPort->setBaudRate(baudRate.toInt()))
        {
            qCritical() << "Can't set baud rate!";
        }
        else
        {
            qDebug() << "Baud rate changed to" << serialPort->baudRate();
        }
    }
}

void PortControl::selectParity(int parity)
{
    if (serialPort->isOpen())
    {
        if(!serialPort->setParity((QSerialPort::Parity) parity))
        {
            qCritical() << "Can't set parity option!";
        }
    }
}

void PortControl::selectDataBits(int dataBits)
{
    if (serialPort->isOpen())
    {
        if(!serialPort->setDataBits((QSerialPort::DataBits) dataBits))
        {
            qCritical() << "Can't set numer of data bits!";
        }
    }
}

void PortControl::selectStopBits(int stopBits)
{
    if (serialPort->isOpen())
    {
        if(!serialPort->setStopBits((QSerialPort::StopBits) stopBits))
        {
            qCritical() << "Can't set number of stop bits!";
        }
    }
}

void PortControl::selectFlowControl(int flowControl)
{
    if (serialPort->isOpen())
    {
        if(!serialPort->setFlowControl((QSerialPort::FlowControl) flowControl))
        {
            qCritical() << "Can't set flow control option!";
        }
    }
}

void PortControl::togglePort()
{
    if (serialPort->isOpen())
    {
        serialPort->close();
        qDebug() << "Closed port:" << serialPort->portName();
        emit portToggled(false);
    }
    else
    {
        // port name may contain description
        QString portName = ui->cbPortList->currentText().split(" ")[0];
        keepPortName(portName);

        serialPort->setPortName(portName);

        // open port
        if (serialPort->open(QIODevice::ReadWrite))
        {
            // set port settings
            selectBaudRate(ui->cbBaudRate->currentText());
            selectParity((QSerialPort::Parity) parityButtons.checkedId());
            selectDataBits((QSerialPort::DataBits) dataBitsButtons.checkedId());
            selectStopBits((QSerialPort::StopBits) stopBitsButtons.checkedId());
            selectFlowControl((QSerialPort::FlowControl) flowControlButtons.checkedId());

            qDebug() << "Opened port:" << serialPort->portName();
            emit portToggled(true);
        }
    }
    ui->pbOpenPort->setChecked(serialPort->isOpen());
}

void PortControl::selectPort(QString portName)
{
    // portName may be coming from combobox
    portName = portName.split(" ")[0];
    // has selection actually changed
    if (portName != serialPort->portName())
    {
        // if another port is already open, close it by toggling
        if (serialPort->isOpen())
        {
            togglePort();

            // open new selection by toggling
            togglePort();
        }
    }
}

void PortControl::enableSkipByte(bool enabled)
{
    ui->pbSkipByte->setDisabled(enabled);
}

void PortControl::keepPortName(QString portName)
{
    if(!discoveredPorts.contains(portName) &&
       !userEnteredPorts.contains(portName))
    {
        userEnteredPorts << portName;
    }
    if(ui->cbPortList->findText(portName) < 0)
    {
        ui->cbPortList->addItem(portName);
    }
}

void PortControl::onPortNameChanged(QString portName)
{
    keepPortName(portName);
}
