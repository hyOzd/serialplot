/*
  Copyright © 2020 Hasan Yavuz Özderya

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
#include <QLineEdit>
#include <QMap>
#include <QtDebug>

#include "setting_defines.h"
#include "utils.h"

#define TBPORTLIST_MINWIDTH (200)

// setting mappings
const QMap<QSerialPort::Parity, QString> paritySettingMap({
        {QSerialPort::NoParity, "none"},
        {QSerialPort::OddParity, "odd"},
        {QSerialPort::EvenParity, "even"},
    });

PortControl::PortControl(QSerialPort* port, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PortControl),
    portToolBar("Port Toolbar"),
    openAction("Open", this),
    loadPortListAction("↺", this)
{
    ui->setupUi(this);

    serialPort = port;
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)),
            this, SLOT(onPortError(QSerialPort::SerialPortError)));

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
                     this, &PortControl::selectListedPort);
    QObject::connect(&tbPortList,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::selectListedPort);

    // setup buttons
    ui->pbOpenPort->setDefaultAction(&openAction);
    ui->pbReloadPorts->setDefaultAction(&loadPortListAction);

    // setup baud rate selection widget
    QObject::connect(ui->cbBaudRate,
                     SELECT<const QString&>::OVERLOAD_OF(&QComboBox::activated),
                     this, &PortControl::_selectBaudRate);

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

    // initialize signal leds
    ui->ledDTR->setOn(true);
    ui->ledRTS->setOn(true);

    // connect output signals
    connect(ui->pbDTR, &QPushButton::clicked, [this]()
            {
                // toggle DTR
                ui->ledDTR->toggle();
                if (serialPort->isOpen())
                {
                    serialPort->setDataTerminalReady(ui->ledDTR->isOn());
                }
            });

    connect(ui->pbRTS, &QPushButton::clicked, [this]()
            {
                // toggle RTS
                ui->ledRTS->toggle();
                if (serialPort->isOpen())
                {
                    serialPort->setRequestToSend(ui->ledRTS->isOn());
                }
            });

    // setup pin update leds
    ui->ledDCD->setColor(Qt::yellow);
    ui->ledDSR->setColor(Qt::yellow);
    ui->ledRI->setColor(Qt::yellow);
    ui->ledCTS->setColor(Qt::yellow);

    pinUpdateTimer.setInterval(1000); // ms
    connect(&pinUpdateTimer, &QTimer::timeout, this, &PortControl::updatePinLeds);

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

    if (portList.rowCount() == 0)
    {
        ui->cbPortList->lineEdit()->setPlaceholderText(tr("No port found - enter name"));
    }
    else
    {
        ui->cbPortList->lineEdit()->setPlaceholderText(tr("Select port or enter name"));
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

void PortControl::_selectBaudRate(QString baudRate)
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
        pinUpdateTimer.stop();
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
            _selectBaudRate(ui->cbBaudRate->currentText());
            selectParity((QSerialPort::Parity) parityButtons.checkedId());
            selectDataBits((QSerialPort::DataBits) dataBitsButtons.checkedId());
            selectStopBits((QSerialPort::StopBits) stopBitsButtons.checkedId());
            selectFlowControl((QSerialPort::FlowControl) flowControlButtons.checkedId());

            // set output signals
            serialPort->setDataTerminalReady(ui->ledDTR->isOn());
            serialPort->setRequestToSend(ui->ledRTS->isOn());

            // update pin signals
            updatePinLeds();
            pinUpdateTimer.start();

            qDebug() << "Opened port:" << serialPort->portName();
            emit portToggled(true);
        }
    }
    openAction.setChecked(serialPort->isOpen());
}

void PortControl::selectListedPort(QString portName)
{
    // portName may be coming from combobox
    portName = portName.split(" ")[0];

    QSerialPortInfo portInfo(portName);
    if (portInfo.isNull())
    {
        qWarning() << "Device doesn't exists:" << portName;
    }

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

QString PortControl::selectedPortName()
{
    QString portText = ui->cbPortList->currentText();
    int portIndex = portList.indexOf(portText);
    if (portIndex < 0) // not in the list yet
    {
        // return the displayed name as port name
        return portText;
    }
    else
    {
        // get the port name from the 'port list'
        return static_cast<PortListItem*>(portList.item(portIndex))->portName();
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

void PortControl::onPortError(QSerialPort::SerialPortError error)
{
#ifdef Q_OS_UNIX
    // For suppressing "Invalid argument" errors that happens with pseudo terminals
    auto isPtsInvalidArgErr = [this] () -> bool {
        return serialPort->portName().contains("pts/") && serialPort->errorString().contains("Invalid argument");
    };
#endif

    switch(error)
    {
        case QSerialPort::NoError :
            break;
        case QSerialPort::ResourceError :
            qWarning() << "Port error: resource unavaliable; most likely device removed.";
            if (serialPort->isOpen())
            {
                qWarning() << "Closing port on resource error: " << serialPort->portName();
                togglePort();
            }
            loadPortList();
            break;
        case QSerialPort::DeviceNotFoundError:
            qCritical() << "Device doesn't exists: " << serialPort->portName();
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
#ifdef Q_OS_UNIX
            // Qt 5.5 gives "Invalid argument" with 'UnsupportedOperationError'
            if (isPtsInvalidArgErr())
                break;
#endif
            qCritical() << "Operation is not supported.";
            break;
        case QSerialPort::TimeoutError:
            qCritical() << "A timeout error occurred.";
            break;
        case QSerialPort::UnknownError:
#ifdef Q_OS_UNIX
            // Qt 5.2 gives "Invalid argument" with 'UnknownError'
            if (isPtsInvalidArgErr())
                break;
#endif
            qCritical() << "Unknown error! Error: " << serialPort->errorString();
            break;
        default:
            qCritical() << "Unhandled port error: " << error;
            break;
    }
}

void PortControl::updatePinLeds(void)
{
    auto pins = serialPort->pinoutSignals();
    ui->ledDCD->setOn(pins & QSerialPort::DataCarrierDetectSignal);
    ui->ledDSR->setOn(pins & QSerialPort::DataSetReadySignal);
    ui->ledRI->setOn(pins & QSerialPort::RingIndicatorSignal);
    ui->ledCTS->setOn(pins & QSerialPort::ClearToSendSignal);
}

QString PortControl::currentParityText()
{
    return paritySettingMap.value(
        (QSerialPort::Parity) parityButtons.checkedId());
}

QString PortControl::currentFlowControlText()
{
    if (flowControlButtons.checkedId() == QSerialPort::HardwareControl)
    {
        return "hardware";
    }
    else if (flowControlButtons.checkedId() == QSerialPort::SoftwareControl)
    {
        return "software";
    }
    else // no parity
    {
        return "none";
    }
}

void PortControl::selectPort(QString portName)
{
    int portIndex = portList.indexOfName(portName);
    if (portIndex < 0) // not in list, add to model and update the selections
    {
        portList.appendRow(new PortListItem(portName));
        portIndex = portList.rowCount()-1;
    }

    ui->cbPortList->setCurrentIndex(portIndex);
    tbPortList.setCurrentIndex(portIndex);

    selectListedPort(portName);
}

void PortControl::selectBaudrate(QString baudRate)
{
    int baudRateIndex = ui->cbBaudRate->findText(baudRate);
    if (baudRateIndex < 0)
    {
        ui->cbBaudRate->setCurrentText(baudRate);
    }
    else
    {
        ui->cbBaudRate->setCurrentIndex(baudRateIndex);
    }
    _selectBaudRate(baudRate);
}

void PortControl::openPort()
{
    if (!serialPort->isOpen())
    {
        openAction.trigger();
    }
}

unsigned PortControl::maxBitRate() const
{
    float baud = serialPort->baudRate();
    float dataBits = serialPort->dataBits();
    float parityBits = serialPort->parity() == QSerialPort::NoParity ? 0 : 1;

    float stopBits;
    if (serialPort->stopBits() == QSerialPort::OneAndHalfStop)
    {
        stopBits = 1.5;
    }
    else
    {
        stopBits = serialPort->stopBits();
    }

    float frame_size = 1 /* start bit */ + dataBits + parityBits + stopBits;

    return float(baud) / frame_size;
}

void PortControl::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Port);
    settings->setValue(SG_Port_SelectedPort, selectedPortName());
    settings->setValue(SG_Port_BaudRate, ui->cbBaudRate->currentText());
    settings->setValue(SG_Port_Parity, currentParityText());
    settings->setValue(SG_Port_DataBits, dataBitsButtons.checkedId());
    settings->setValue(SG_Port_StopBits, stopBitsButtons.checkedId());
    settings->setValue(SG_Port_FlowControl, currentFlowControlText());
    settings->endGroup();
}

void PortControl::loadSettings(QSettings* settings)
{
    // make sure the port is closed
    if (serialPort->isOpen()) togglePort();

    settings->beginGroup(SettingGroup_Port);

    // set port name if it exists in the current list otherwise ignore
    QString portName = settings->value(SG_Port_SelectedPort, QString()).toString();
    if (!portName.isEmpty())
    {
        int index = portList.indexOfName(portName);
        if (index > -1) ui->cbPortList->setCurrentIndex(index);
    }

    // load baud rate setting if it exists in baud rate list
    QString baudSetting = settings->value(
        SG_Port_BaudRate, ui->cbBaudRate->currentText()).toString();
    int baudIndex = ui->cbBaudRate->findText(baudSetting);
    if (baudIndex > -1) ui->cbBaudRate->setCurrentIndex(baudIndex);

    // load parity setting
    QString parityText =
        settings->value(SG_Port_Parity, currentParityText()).toString();
    QSerialPort::Parity paritySetting = paritySettingMap.key(
        parityText, (QSerialPort::Parity) parityButtons.checkedId());
    parityButtons.button(paritySetting)->setChecked(true);

    // load number of bits
    int dataBits = settings->value(SG_Port_DataBits, dataBitsButtons.checkedId()).toInt();
    if (dataBits >=5 && dataBits <= 8)
    {
        dataBitsButtons.button((QSerialPort::DataBits) dataBits)->setChecked(true);
    }

    // load stop bits
    int stopBits = settings->value(SG_Port_StopBits, stopBitsButtons.checkedId()).toInt();
    if (stopBits == QSerialPort::OneStop)
    {
        ui->rb1StopBit->setChecked(true);
    }
    else if (stopBits == QSerialPort::TwoStop)
    {
        ui->rb2StopBit->setChecked(true);
    }

    // load flow control
    QString flowControlSetting =
        settings->value(SG_Port_FlowControl, currentFlowControlText()).toString();
    if (flowControlSetting == "hardware")
    {
        ui->rbHardwareControl->setChecked(true);
    }
    else if (flowControlSetting == "software")
    {
        ui->rbSoftwareControl->setChecked(true);
    }
    else
    {
        ui->rbNoFlowControl->setChecked(true);
    }

    settings->endGroup();
}
