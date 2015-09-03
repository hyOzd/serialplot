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

#include <QVariant>
#include <QtDebug>

#include "portlist.h"

PortListItem::PortListItem(QString name, QString description, quint16 vid, quint16 pid)
{
    construct(name, description, vid, pid);
}

PortListItem::PortListItem(QSerialPortInfo* portInfo)
{
    if (portInfo->hasProductIdentifier())
    {
        construct(portInfo->portName(),
                  portInfo->description(),
                  portInfo->vendorIdentifier(),
                  portInfo->productIdentifier());
    }
    else
    {
        construct(portInfo->portName());
    }
}

void PortListItem::construct(QString name, QString description, quint16 vid, quint16 pid)
{
    QString text = name;
    if (!description.isEmpty())
    {
        text += QString(" ") + description;
    }
    if (vid && pid)
    {
        text += QString("[%1:").arg(vid, 4, 16, QChar('0'));
        text += QString("%1]").arg(pid, 4, 16, QChar('0'));
    }
    setText(text);
    setData(name, PortNameRole);
}

QString PortListItem::name()
{
    return data(PortNameRole).toString();
}

PortList::PortList(QObject* parent) :
    QStandardItemModel(parent)
{
    loadPortList();
}

void PortList::loadPortList()
{
    clear();

    for (auto portInfo : QSerialPortInfo::availablePorts())
    {
        appendRow(new PortListItem(&portInfo));
    }
}
