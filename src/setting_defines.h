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

#ifndef SETTING_DEFINES_H
#define SETTING_DEFINES_H

const char SettingGroup_MainWindow[] = "MainWindow";
const char SettingGroup_Port[] = "Port";
const char SettingGroup_DataFormat[] = "DataFormat";
const char SettingGroup_Binary[] = "DataFormat_Binary";
const char SettingGroup_ASCII[] = "DataFormat_ASCII";
const char SettingGroup_CustomFrame[] = "DataFormat_CustomFrame";
const char SettingGroup_Channels[] = "Channels";
const char SettingGroup_Plot[] = "Plot";
const char SettingGroup_Commands[] = "Commands";
const char SettingGroup_Record[] = "Record";

// mainwindow setting keys
const char SG_MainWindow_Size[] = "size";
const char SG_MainWindow_Pos[] = "pos";
const char SG_MainWindow_ActivePanel[] = "activePanel";
const char SG_MainWindow_HidePanels[] = "hidePanels";
const char SG_MainWindow_Maximized[] = "maximized";
const char SG_MainWindow_State[] = "state";

// port setting keys
const char SG_Port_SelectedPort[] = "selectedPort";
const char SG_Port_BaudRate[] = "baudRate";
const char SG_Port_Parity[] = "parity";
const char SG_Port_DataBits[] = "dataBits";
const char SG_Port_StopBits[] = "stopBits";
const char SG_Port_FlowControl[] = "flowControl";

// data format panel keys
const char SG_DataFormat_Format[] = "format";

// binary stream reader keys
const char SG_Binary_NumOfChannels[] = "numOfChannels";
const char SG_Binary_NumberFormat[] = "numberFormat";
const char SG_Binary_Endianness[] = "endianness";

// ascii reader keys
const char SG_ASCII_NumOfChannels[] = "numOfChannels";

// framed reader keys
const char SG_CustomFrame_NumOfChannels[] = "numOfChannels";
const char SG_CustomFrame_FrameStart[] = "frameStart";
const char SG_CustomFrame_FixedSize[] = "fixedSize";
const char SG_CustomFrame_FrameSize[] = "frameSize";
const char SG_CustomFrame_NumberFormat[] = "numberFormat";
const char SG_CustomFrame_Endianness[] = "endianness";
const char SG_CustomFrame_Checksum[] = "checksum";
const char SG_CustomFrame_DebugMode[] = "debugMode";

// channel manager keys
const char SG_Channels_Channel[] = "channel";
const char SG_Channels_Name[] = "name";
const char SG_Channels_Color[] = "color";
const char SG_Channels_Visible[] = "visible";

// plot settings keys
const char SG_Plot_NumOfSamples[] = "numOfSamples";
const char SG_Plot_AutoScale[] = "autoScale";
const char SG_Plot_YMax[] = "yMax";
const char SG_Plot_YMin[] = "yMin";
const char SG_Plot_DarkBackground[] = "darkBackground";
const char SG_Plot_Grid[] = "grid";
const char SG_Plot_MinorGrid[] = "minorGrid";
const char SG_Plot_Legend[] = "legend";
const char SG_Plot_MultiPlot[] = "multiPlot";

// command setting keys
const char SG_Commands_Command[] = "command";
const char SG_Commands_Name[] = "name";
const char SG_Commands_Type[] = "type";
const char SG_Commands_Data[] = "data";

// record panel settings keys
const char SG_Record_AutoIncrement[]    = "autoIncrement";
const char SG_Record_RecordPaused[]     = "recordPaused";
const char SG_Record_StopOnClose[]      = "stopOnClose";
const char SG_Record_Header[]           = "header";
const char SG_Record_Separator[]        = "separator";
const char SG_Record_DisableBuffering[] = "disableBuffering";

#endif // SETTING_DEFINES_H
