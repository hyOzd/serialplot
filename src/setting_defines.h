/*
  Copyright © 2016 Hasan Yavuz Özderya

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

#ifndef SETTING_DEFINES_H
#define SETTING_DEFINES_H

const char SettingGroup_MainWindow[] = "MainWindow";
const char SettingGroup_Port[] = "Port";
const char SettingGroup_DataFormat[] = "DataFormat";
const char SettingGroup_Channels[] = "Channels";
const char SettingGroup_Plot[] = "Plot";
const char SettingGroup_Commands[] = "Commands";

// mainwindow setting keys
const char SG_MainWindow_Size[] = "size";
const char SG_MainWindow_Pos[] = "pos";
const char SG_MainWindow_ActivePanel[] = "activePanel";

// port setting keys
const char SG_Port_SelectedPort[] = "selectedPort";
const char SG_Port_BaudRate[] = "baudRate";
const char SG_Port_Parity[] = "parity";
const char SG_Port_DataBits[] = "dataBits";
const char SG_Port_StopBits[] = "stopBits";
const char SG_Port_FlowControl[] = "flowControl";

#endif // SETTING_DEFINES_H
