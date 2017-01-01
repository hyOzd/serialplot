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

#include <QApplication>
#include <QtGlobal>

#include "mainwindow.h"
#include "tooltipfilter.h"
#include "version.h"

// test code
#include <QTableView>
#include <QSpinBox>
#include <QColorDialog>
#include "channelinfomodel.h"
#include "utils.h"

#include "color_selector.hpp"
#include "color_dialog.hpp"

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

    ToolTipFilter ttf;
    a.installEventFilter(&ttf);

    // log application information
    qDebug() << "SerialPlot" << VERSION_STRING;
    qDebug() << "Revision" << VERSION_REVISION;

    w.show();

    // test code
    ChannelInfoModel cim(2, &a);
    QTableView tv;
    QSpinBox cb;

    QObject::connect(&cb, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged), [&cim](int value)
                     {
                         cim.setNumOfChannels(value);
                     });

    tv.setModel(&cim);

    tv.show();
    // cb.show();

    cim.setNumOfChannels(3);
    cim.setNumOfChannels(7);
    cim.setNumOfChannels(10);


    color_widgets::ColorSelector cpicker;
    cpicker.setColor(QColor("red"));
    cpicker.show();

    QObject::connect(tv.selectionModel(), &QItemSelectionModel::currentRowChanged,
                     [&cim, &cpicker](const QModelIndex &current, const QModelIndex &previous)
                     {
                         cpicker.setColor(cim.data(current, Qt::ForegroundRole).value<QColor>());
                     });

    QObject::connect(&cpicker, &color_widgets::ColorSelector::colorChanged,
                     [&cim, &tv](QColor color)
                     {
                         auto index = tv.selectionModel()->currentIndex();
                         index = index.sibling(index.row(), 0);
                         cim.setData(index, color, Qt::ForegroundRole);
                     });

    return a.exec();
}
