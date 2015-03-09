#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QtDebug>
#include <qwt_plot.h>
#include <limits.h>
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // init UI signals
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

    // init port signals
    QObject::connect(&(this->serialPort), SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(onPortError(QSerialPort::SerialPortError)));

    QObject::connect(&(this->serialPort), &QSerialPort::readyRead,
                     this, &MainWindow::onDataReady);

    loadPortList();
    loadBaudRateList();
    ui->cbBaudRate->setCurrentIndex(ui->cbBaudRate->findText("9600"));

    // set limits for axis limit boxes
    ui->spYmin->setRange(std::numeric_limits<double>::min(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange(std::numeric_limits<double>::min(),
                         std::numeric_limits<double>::max());

    // init plot
    numOfSamples = ui->spNumOfSamples->value();
    dataArray.resize(numOfSamples);
    dataX.resize(numOfSamples);
    for (int i = 0; i < dataX.size(); i++)
    {
        dataX[i] = i;
    }
    curve.setSamples(dataX, dataArray);
    curve.attach(ui->plot);
}

MainWindow::~MainWindow()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
    }
    delete ui;
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

void MainWindow::onPortToggled(bool open)
{
    ui->pbOpenPort->setChecked(open);
}

void MainWindow::onDataReady()
{
    if (!ui->actionPause->isChecked())
    {
        QByteArray data = serialPort.readAll();
        addData((unsigned char)(data[0]));
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

void MainWindow::addData(double data)
{
    // shift data array and place new data at the end
    for (int i = 0; i < dataArray.size()-1; i++)
    {
        dataArray[i] = dataArray[i+1];
    }
    dataArray.last() = data;

    // update plot
    curve.setSamples(dataX, dataArray);
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
        dataArray.remove(0, oldNum - numOfSamples);
    }
    else if(numOfSamples > oldNum)
    {
        dataX.resize(numOfSamples);
        for (unsigned int i = oldNum; i < numOfSamples; i++)
        {
            dataX[i] = i;
            dataArray.prepend(0);
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
