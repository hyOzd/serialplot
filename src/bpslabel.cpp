/*
  Copyright © 2019 Hasan Yavuz Özderya

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

#include "bpslabel.h"

const char* BPS_TOOLTIP = "bits per second";
const char* BPS_TOOLTIP_ERR = "Maximum baud rate may be reached!";

BPSLabel::BPSLabel(PortControl* portControl,
                   DataFormatPanel* dataFormatPanel,
                   QWidget *parent) :
    QLabel(parent)
{
    _portControl = portControl;
    _dataFormatPanel = dataFormatPanel;
    prevBytesRead = 0;

    setText("0bps");
    setToolTip(tr(BPS_TOOLTIP));

    connect(&bpsTimer, &QTimer::timeout,
            this, &BPSLabel::onBpsTimeout);

    connect(portControl, &PortControl::portToggled,
            this, &BPSLabel::onPortToggled);
}

void BPSLabel::onBpsTimeout()
{
    uint64_t curBytesRead = _dataFormatPanel->bytesRead();
    uint64_t bytesRead = curBytesRead - prevBytesRead;
    prevBytesRead = curBytesRead;

    unsigned bits = bytesRead * 8;
    unsigned maxBps = _portControl->maxBitRate();
    QString str;
    if (bits >= maxBps)
    {
        // TODO: an icon for bps warning
        str = QString(tr("!%1/%2bps")).arg(bits).arg(maxBps);
        setToolTip(tr(BPS_TOOLTIP_ERR));
    }
    else
    {
        str = QString(tr("%1bps")).arg(bits);
        setToolTip(tr(BPS_TOOLTIP));
    }
    setText(str);
}

void BPSLabel::onPortToggled(bool open)
{
    if (open)
    {
        bpsTimer.start(1000);
    }
    else
    {
        bpsTimer.stop();
        // if not cleared last displayed value is stuck
        setText("0bps");
        setToolTip(tr(BPS_TOOLTIP));
    }
}
