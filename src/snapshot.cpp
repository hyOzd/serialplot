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

#include <stddef.h>
#include <QSaveFile>
#include <QTextStream>

#include "mainwindow.h"
#include "snapshot.h"
#include "snapshotview.h"

Snapshot::Snapshot(MainWindow* parent, QString name, ChannelInfoModel infoModel, bool saved) :
    QObject(parent),
    cInfoModel(infoModel),
    _showAction(this),
    _deleteAction("&Delete", this)
{
    _name = name;
    _saved = saved;

    view = NULL;
    mainWindow = parent;
    _showAction.setText(displayName());
    connect(&_showAction, &QAction::triggered, this, &Snapshot::show);

    _deleteAction.setToolTip(QString("Delete ") + _name);
    connect(&_deleteAction, &QAction::triggered, this, &Snapshot::onDeleteTriggered);
}

Snapshot::~Snapshot()
{
    if (view != NULL)
    {
        delete view;
    }
}

QAction* Snapshot::showAction()
{
    return &_showAction;
}

QAction* Snapshot::deleteAction()
{
    return &_deleteAction;
}

void Snapshot::show()
{
    if (view == NULL)
    {
        view = new SnapshotView(mainWindow, this);
        connect(view, &SnapshotView::closed, this, &Snapshot::viewClosed);
    }
    view->show();
    view->activateWindow();
    view->raise();
}

void Snapshot::viewClosed()
{
    view->deleteLater();
    view = NULL;
}

void Snapshot::onDeleteTriggered()
{
    emit deleteRequested(this);
}

QString Snapshot::name()
{
    return _name;
}

QString Snapshot::displayName()
{
    if (_saved)
    {
        return name();
    }
    else
    {
        return name() + "*";
    }
}

void Snapshot::setName(QString name)
{
    _name = name;
    _showAction.setText(_name);
    emit nameChanged(this);
}

ChannelInfoModel* Snapshot::infoModel()
{
    return &cInfoModel;
}

QString Snapshot::channelName(unsigned channel)
{
    return cInfoModel.name(channel);
}

void Snapshot::save(QString fileName)
{
    // TODO: remove code duplication (MainWindow::onExportCsv)
    QSaveFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);

        unsigned numOfChannels = data.size();
        unsigned numOfSamples = data[0].size();

        // print header
        for (unsigned int ci = 0; ci < numOfChannels; ci++)
        {
            fileStream << channelName(ci);
            if (ci != numOfChannels-1) fileStream << ",";
        }
        fileStream << '\n';

        // print rows
        for (unsigned int i = 0; i < numOfSamples; i++)
        {
            for (unsigned int ci = 0; ci < numOfChannels; ci++)
            {
                fileStream << data[ci][i].y();
                if (ci != numOfChannels-1) fileStream << ",";
            }
            fileStream << '\n';
        }

        if (!file.commit())
        {
            qCritical() << "File save error during snapshot save: " << file.error();
        }
        else
        {
            _saved = true;
            _showAction.setText(displayName());
        }
    }
    else
    {
        qCritical() << "File open error during snapshot save: " << file.error();
    }
}

bool Snapshot::isSaved()
{
    return _saved;
}
