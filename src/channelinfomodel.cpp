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

#include "channelinfomodel.h"

#include <QColor>
#include <QtDebug>

enum ChannelInfoColumn
{
    COLUMN_NAME = 0,
    COLUMN_VISIBILITY,
    COLUMN_COLOR,
    COLUMN_COUNT
};

const QColor colors[8] =
{
    QColor(237,97,68),
    QColor(92,200,96),
    QColor(225,98,207),
    QColor(163,195,58),
    QColor(148,123,239),
    QColor(212,182,52),
    QColor(238,82,133),
    QColor(219,136,44)
};

ChannelInfoModel::ChannelInfoModel(unsigned numberOfChannels, QObject* parent) :
    QAbstractTableModel(parent)
{
    _numOfChannels = 0;
    setNumOfChannels(numberOfChannels);
}

int ChannelInfoModel::rowCount(const QModelIndex &parent) const
{
    return _numOfChannels;
}

int ChannelInfoModel::columnCount(const QModelIndex & parent) const
{
    return COLUMN_COUNT;
}

Qt::ItemFlags ChannelInfoModel::flags(const QModelIndex &index) const
{
    if (index.column() == COLUMN_NAME)
    {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    }
    else if (index.column() == COLUMN_VISIBILITY)
    {
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    }
    else if (index.column() == COLUMN_COLOR)
    {
        return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    }

    return Qt::NoItemFlags;
}

QVariant ChannelInfoModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= (int) _numOfChannels)
    {
        return QVariant();
    }

    if (index.column() == COLUMN_NAME)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            return QVariant(infos[index.row()].name);
        }
        else if (role == Qt::ForegroundRole)
        {
            // TODO: support more colors than 8
            return colors[index.row() % 8];
        }
    }
    else if (index.column() == COLUMN_VISIBILITY)
    {
        if (role == Qt::CheckStateRole)
        {
            bool visible = infos[index.row()].visibility;
            return visible ? Qt::Checked : Qt::Unchecked;
        }
    }
    else if (index.column() == COLUMN_COLOR)
    {
        if (role == Qt::ForegroundRole || role == Qt::BackgroundRole)
        {
            return colors[index.row() % 8];
        }
    }

    return QVariant();
}

QVariant ChannelInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == COLUMN_NAME)
            {
                return tr("Channel");
            }
            else if (section == COLUMN_VISIBILITY)
            {
                return tr("Visible");
            }
            else if (section == COLUMN_COLOR)
            {
                return tr("Color");
            }
        }
    }
    else                        // vertical
    {
        if (section < (int) _numOfChannels && role == Qt::DisplayRole)
        {
            return QString::number(section + 1);
        }
    }

    return QVariant();
}

bool ChannelInfoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= (int) _numOfChannels)
    {
        return false;
    }

    if (index.column() == COLUMN_NAME)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            infos[index.row()].name = value.toString();
            return true;
        }
    }
    else if (index.column() == COLUMN_VISIBILITY)
    {
        if (role == Qt::CheckStateRole)
        {
            bool checked = value.toInt() == Qt::Checked;
            infos[index.row()].visibility = checked;
            return true;
        }
    }

    // invalid index/role
    return false;
}

void ChannelInfoModel::setNumOfChannels(unsigned number)
{
    if (number == _numOfChannels) return;

    bool isInserting = number > _numOfChannels;
    if (isInserting)
    {
        beginInsertRows(QModelIndex(), _numOfChannels, number-1);
    }
    else
    {
        beginRemoveRows(QModelIndex(), number, _numOfChannels-1);
    }

    // we create channel info but never remove channel info to
    // remember user entered info
    if ((int) number > infos.length())
    {
        for (unsigned ci = _numOfChannels; ci < number; ci++)
        {
            infos.append({QString("Channel %1").arg(ci+1), true});
        }
    }

    // make sure newly available channels are visible, we don't
    // remember visibility option intentionally so that user doesn't
    // get confused
    if (number > _numOfChannels)
    {
        for (unsigned ci = _numOfChannels; ci < number; ci++)
        {
            infos[ci].visibility = true;
        }
    }

    _numOfChannels = number;

    if (isInserting)
    {
        endInsertRows();
    }
    else
    {
        endRemoveRows();
    }
}
