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
#include <QKeySequence>
#include <QLabel>
#include <QtDebug>
#include "utils.h"

#define TBPORTLIST_MINWIDTH (200)

PortControl::PortControl(QSerialPort* port, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PortControl),
    portToolBar("Port Toolbar"),
    openAction("Open", this),
    loadPortListAction("↺", this)
{
    ui->setupUi(this);

    serialPort = port;

    // setup actions
    openAction.setCheckable(true);
    openAction.setShortcut(QKeySequence("F12"));
    openAction.setToolTip("Open Port");
    QObject::connect(&openAction, &QAction::triggered,
                     this, &PortControl::openActionTriggered);

    loadPortListAction.setToolTip("Reload port list");
    QObject::connect(&loadPortListAction, &QAction::triggered,
                     [this](bool checked){loadPortList();});

    // setup toolbar
    portToolBar.addWidget(&tbPortList);
    portToolBar.addAction(&loadPortListAction);
    portToolBar.addAction(&openAction);

    // setup port selection widgets
    tbPortList.setMinimumWidth(TBPORTLIST_MINWIDTH);
    tbPortList.setModel(&portList);
    ui->cbPortList->setModel(&portList);
    QObject::connect(ui->cbPortList,
                     SELECT<int>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::onCbPortListActivated);
    QObject::connect(&tbPortList,
                     SELECT<int>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::onTbPortListActivated);
    QObject::connect(ui->cbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::selectPort);
    QObject::connect(&tbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::selectPort);

    // setup buttons
    ui->pbOpenPort->setDefaultAction(&openAction);
    ui->pbReloadPorts->setDefaultAction(&loadPortListAction);

    // setup baud rate selection widget
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
    QString currentSelection = ui->cbPortList->currentData(PortNameRole).toString();
    portList.loadPortList();
    int index = portList.indexOf(currentSelection);
    if (index >= 0)
    {
        ui->cbPortList->setCurrentIndex(index);
        tbPortList.setCurrentIndex(index);
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
        // we get the port name from the edit text, which may not be
        // in the portList if user hasn't pressed Enter
        // Also note that, portText may not be the `portName`
        QString portText = ui->cbPortList->currentText();
        QString portName;
        int portIndex = portList.indexOf(portText);
        if (portIndex < 0) // not in list, add to model and update the selections
        {
            portList.appendRow(new PortListItem(portText));
            ui->cbPortList->setCurrentIndex(portList.rowCount()-1);
            tbPortList.setCurrentIndex(portList.rowCount()-1);
            portName = portText;
        }
        else
        {
            // get the port name from the data field
            portName = static_cast<PortListItem*>(portList.item(portIndex))->portName();
        }

        serialPort->setPortName(ui->cbPortList->currentData(PortNameRole).toString());

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
    openAction.setChecked(serialPort->isOpen());
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

QToolBar* PortControl::toolBar()
{
    return &portToolBar;
}

void PortControl::openActionTriggered(bool checked)
{
    togglePort();
}

void PortControl::onCbPortListActivated(int index)
{
    tbPortList.setCurrentIndex(index);
}

void PortControl::onTbPortListActivated(int index)
{
    ui->cbPortList->setCurrentIndex(index);
}
