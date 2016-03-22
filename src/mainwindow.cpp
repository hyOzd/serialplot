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
#include <QMenu>
#include <QtDebug>
#include <qwt_plot.h>
#include <limits.h>
#include <cmath>
#include <iostream>

// test code
#include <QListView>

#include <plot.h>

#include "utils.h"
#include "version.h"

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
    aboutDialog(this),
    portControl(&serialPort),
    channelMan(1, 1, this),
    commandPanel(&serialPort),
    dataFormatPanel(&serialPort, &channelMan),
    snapshotMan(this, &channelMan)
{
    ui->setupUi(this);
    ui->tabWidget->insertTab(0, &portControl, "Port");
    ui->tabWidget->insertTab(1, &dataFormatPanel, "Data Format");
    ui->tabWidget->insertTab(3, &commandPanel, "Commands");
    ui->tabWidget->setCurrentIndex(0);
    addToolBar(portControl.toolBar());

    ui->plotToolBar->addAction(snapshotMan.takeSnapshotAction());
    ui->menuBar->insertMenu(ui->menuHelp->menuAction(), snapshotMan.menu());
    ui->menuBar->insertMenu(ui->menuHelp->menuAction(), commandPanel.menu());
    connect(commandPanel.newCommandAction(), &QAction::triggered, [this]()
            {
                this->ui->tabWidget->setCurrentWidget(&commandPanel);
            });

    setupAboutDialog();

    // init view menu
    for (auto a : ui->plot->menuActions())
    {
        ui->menuView->addAction(a);
    }

    ui->menuView->addSeparator();

    QMenu* tbMenu = ui->menuView->addMenu("Toolbars");
    tbMenu->addAction(ui->plotToolBar->toggleViewAction());
    tbMenu->addAction(portControl.toolBar()->toggleViewAction());

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
                     &dataFormatPanel, &DataFormatPanel::requestSkipByte);

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

    QObject::connect(snapshotMan.takeSnapshotAction(), &QAction::triggered,
                     ui->plot, &Plot::flashSnapshotOverlay);

    // init port signals
    QObject::connect(&(this->serialPort), SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(onPortError(QSerialPort::SerialPortError)));

    // set limits for axis limit boxes
    ui->spYmin->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    ui->spYmax->setRange((-1) * std::numeric_limits<double>::max(),
                         std::numeric_limits<double>::max());

    // init data format and reader
    QObject::connect(&dataFormatPanel, &DataFormatPanel::dataAdded,
                     ui->plot, &QwtPlot::replot);

    QObject::connect(ui->actionPause, &QAction::triggered,
                     &dataFormatPanel, &DataFormatPanel::pause);

    // init data arrays and plot
    numOfSamples = ui->spNumOfSamples->value();
    unsigned numOfChannels = dataFormatPanel.numOfChannels();

    channelMan.setNumOfSamples(ui->spNumOfSamples->value());
    channelMan.setNumOfChannels(dataFormatPanel.numOfChannels());

    connect(&dataFormatPanel, &DataFormatPanel::numOfChannelsChanged,
            &channelMan, &ChannelManager::setNumOfChannels);

    connect(&channelMan, &ChannelManager::numOfChannelsChanged,
            this, &MainWindow::onNumOfChannelsChanged);

    connect(&channelMan, &ChannelManager::channelNameChanged,
            this, &MainWindow::onChannelNameChanged);

    ui->lvChannelNames->setModel(channelMan.channelNames());

    // init curve list
    for (unsigned int i = 0; i < numOfChannels; i++)
    {
        curves.append(new QwtPlotCurve(channelMan.channelName(i)));
        curves[i]->setSamples(channelMan.channelBuffer(i));
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

    // Init sps (sample per second) counter
    spsLabel.setText("0sps");
    spsLabel.setToolTip("samples per second (total of all channels)");
    ui->statusBar->addPermanentWidget(&spsLabel);
    QObject::connect(&dataFormatPanel,
                     &DataFormatPanel::samplesPerSecondChanged,
                     this, &MainWindow::onSpsChanged);

    // init demo
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
            qCritical() << "Unknown error! Error: " << serialPort.errorString();
            break;
        default:
            qCritical() << "Unhandled port error: " << error;
            break;
    }
}

void MainWindow::clearPlot()
{
    for (unsigned ci = 0; ci < channelMan.numOfChannels(); ci++)
    {
        channelMan.channelBuffer(ci)->clear();
    }
    ui->plot->replot();
}

void MainWindow::onNumOfSamplesChanged(int value)
{
    numOfSamples = value;
    channelMan.setNumOfSamples(value);
    ui->plot->replot();
}

void MainWindow::onNumOfChannelsChanged(unsigned value)
{
    unsigned int oldNum = curves.size();
    unsigned numOfChannels = value;

    if (numOfChannels > oldNum)
    {
        // add new channels
        for (unsigned int i = oldNum; i < numOfChannels; i++)
        {
            QwtPlotCurve* curve = new QwtPlotCurve(channelMan.channelName(i));
            // TODO: create a wrapper around FrameBuffer that holds a
            // pointer to it and provides the QwtDataSeries interface,
            // that wrapper should be created for and owned by 'curve'
            curve->setSamples(channelMan.channelBuffer(i));
            curve->setPen(Plot::makeColor(i));
            curve->attach(ui->plot);
            curves.append(curve);
        }
    }
    else if(numOfChannels < oldNum)
    {
        // remove channels
        for (unsigned int i = 0; i < oldNum - numOfChannels; i++)
        {
            // also deletes owned FrameBuffer TODO: which souldn't happen
            delete curves.takeLast();
        }
    }

    ui->plot->replot();
}

void MainWindow::onChannelNameChanged(unsigned channel, QString name)
{
    // This slot is triggered also when a new channel is added, in
    // this case curve list doesn't contain said channel. No worries,
    // since `onNumOfChannelsChanged` slot will update curve list.
    if ((int) channel < curves.size()) // check if channel exists in curve list
    {
        curves[channel]->setTitle(name);
        ui->plot->replot();
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

void MainWindow::onSpsChanged(unsigned sps)
{
    spsLabel.setText(QString::number(sps) + "sps");
}

bool MainWindow::isDemoRunning()
{
    return ui->actionDemoMode->isChecked();
}

void MainWindow::enableDemo(bool enabled)
{
    if (enabled)
    {
        if (!serialPort.isOpen())
        {
            dataFormatPanel.enableDemo(true);
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
        dataFormatPanel.enableDemo(false);
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

            unsigned numOfChannels = channelMan.numOfChannels();
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
                    fileStream << channelMan.channelBuffer(ci)->sample(i).y();
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
