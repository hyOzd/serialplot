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
#include <QApplication>
#include <QtGlobal>

#include "version.h"

MainWindow* pMainWindow;

void messageHandler(QtMsgType type, const QMessageLogContext &context,
                    const QString &msg)
{
    // TODO: don't call MainWindow::messageHandler if window is destroyed
    pMainWindow->messageHandler(type, context, msg);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    pMainWindow = &w;

    qInstallMessageHandler(messageHandler);

    // log application information
    qDebug() << "SerialPlot" << VERSION_STRING;
    qDebug() << "Revision" << VERSION_REVISION;

    w.show();
    return a.exec();
}
