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

#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QSerialPortInfo>

enum PortListRoles
{
    PortNameRole = Qt::UserRole  // portName as QString
};

class PortListItem : public QStandardItem
{
public:
    PortListItem(QString name, QString description="", quint16 vid=0, quint16 pid=0);
    PortListItem(QSerialPortInfo* portInfo);

    QString name(); // returns only the port name

private:
    // common constructor
    void construct(QString name, QString description="", quint16 vid=0, quint16 pid=0);
};

class PortList : public QStandardItemModel
{
public:
    PortList(QObject* parent=0);

    void loadPortList();

private:
    QList<PortListItem*> userEnteredPorts;
};
