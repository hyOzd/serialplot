/*
  Copyright © 2017 Hasan Yavuz Özderya

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

#include <QApplication>
#include <QtGlobal>
#include <iostream>

#include "mainwindow.h"
#include "tooltipfilter.h"
#include "version.h"

MainWindow* pMainWindow = nullptr;

void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg)
{
    QString logString;

    switch (type)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            logString = "[Info] " + msg;
            break;
#endif
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

    std::cerr << logString.toStdString() << std::endl;

    if (pMainWindow != nullptr)
    {
        // TODO: don't call MainWindow::messageHandler if window is destroyed
        pMainWindow->messageHandler(type, logString, msg);
    }

    if (type == QtFatalMsg)
    {
        __builtin_trap();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName(PROGRAM_NAME);
    QApplication::setApplicationVersion(VERSION_STRING);

    qInstallMessageHandler(messageHandler);
    MainWindow w;
    pMainWindow = &w;

    ToolTipFilter ttf;
    a.installEventFilter(&ttf);

    // log application information
    qDebug() << "SerialPlot" << VERSION_STRING;
    qDebug() << "Revision" << VERSION_REVISION;

    w.show();

    return a.exec();
}
