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

#include <QtDebug>

/// name + visibility
enum ChannelInfoColumn
{
    COLUMN_NAME = 0,
    COLUMN_VISIBILITY,
    COLUMN_COUNT
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
    }
    else if (index.column() == COLUMN_VISIBILITY)
    {
        if (role == Qt::CheckStateRole)
        {
            bool visible = infos[index.row()].visibility;
            return visible ? Qt::Checked : Qt::Unchecked;
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

    // we create channel info but never remove channel info to
    // remember user entered info
    if ((int) number > infos.length())
    {
        for (unsigned ci = _numOfChannels; ci < number; ci++)
        {
            infos.append({QString("Channel %1").arg(ci), true});
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
}
