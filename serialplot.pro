#
# Copyright © 2019 Hasan Yavuz Özderya
#
# This file is part of serialplot.
#
# serialplot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# serialplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
#

#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T08:20:06
#
#-------------------------------------------------

QT       += core gui serialport network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialplot
TEMPLATE = app

CONFIG += qwt
# LIBS += -lqwt # enable this line if qwt pri files aren't installed

DEFINES += PROGRAM_NAME="\\\"serialplot\\\""

DEFINES += VERSION_MAJOR=10 VERSION_MINOR=0 VERSION_PATCH=0 VERSION_STRING=\\\"10.0.0\\\"

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/portcontrol.cpp \
    src/plot.cpp \
    src/zoomer.cpp \
    src/scrollzoomer.cpp \
    src/scrollbar.cpp \
    src/hidabletabwidget.cpp \
    src/scalepicker.cpp \
    src/scalezoomer.cpp \
    src/portlist.cpp \
    src/snapshot.cpp \
    src/snapshotview.cpp \
    src/snapshotmanager.cpp \
    src/plotsnapshotoverlay.cpp \
    src/commandpanel.cpp \
    src/commandwidget.cpp \
    src/commandedit.cpp \
    src/dataformatpanel.cpp \
    src/plotcontrolpanel.cpp \
    src/recordpanel.cpp \
    src/datarecorder.cpp \
    src/tooltipfilter.cpp \
    src/sneakylineedit.cpp \
    src/stream.cpp \
    src/streamchannel.cpp \
    src/channelinfomodel.cpp \
    src/ringbuffer.cpp \
    src/indexbuffer.cpp \
    src/linindexbuffer.cpp \
    src/readonlybuffer.cpp \
    src/framebufferseries.cpp \
    src/numberformatbox.cpp \
    src/endiannessbox.cpp \
    src/abstractreader.cpp \
    src/binarystreamreader.cpp \
    src/binarystreamreadersettings.cpp \
    src/asciireader.cpp \
    src/asciireadersettings.cpp \
    src/demoreader.cpp \
    src/demoreadersettings.cpp \
    src/framedreader.cpp \
    src/framedreadersettings.cpp \
    src/plotmanager.cpp \
    src/plotmenu.cpp \
    src/barplot.cpp \
    src/barchart.cpp \
    src/barscaledraw.cpp \
    src/numberformat.cpp \
    src/updatechecker.cpp \
    src/versionnumber.cpp \
    src/updatecheckdialog.cpp \
    src/samplepack.cpp \
    src/source.cpp \
    src/sink.cpp \
    src/samplecounter.cpp \
    src/ledwidget.cpp \
    src/datatextview.cpp \
    src/bpslabel.cpp

HEADERS += \
    src/mainwindow.h \
    src/utils.h \
    src/portcontrol.h \
    src/byteswap.h \
    src/plot.h \
    src/hidabletabwidget.h \
    src/framebuffer.h \
    src/scalepicker.h \
    src/scalezoomer.h \
    src/portlist.h \
    src/snapshotview.h \
    src/snapshotmanager.h \
    src/snapshot.h \
    src/plotsnapshotoverlay.h \
    src/commandpanel.h \
    src/commandwidget.h \
    src/commandedit.h \
    src/dataformatpanel.h \
    src/tooltipfilter.h \
    src/sneakylineedit.h \
    src/framebufferseries.h \
    src/plotcontrolpanel.h \
    src/numberformatbox.h \
    src/endiannessbox.h \
    src/framedreadersettings.h \
    src/abstractreader.h \
    src/binarystreamreader.h \
    src/binarystreamreadersettings.h \
    src/asciireadersettings.h \
    src/asciireader.h \
    src/demoreader.h \
    src/framedreader.h \
    src/plotmanager.h \
    src/setting_defines.h \
    src/numberformat.h \
    src/recordpanel.h \
    src/updatechecker.h \
    src/updatecheckdialog.h \
    src/demoreadersettings.h \
    src/datatextview.h \
    src/bpslabel.h \
    src/barchart.h \
    src/barplot.h \
    src/barscaledraw.h \
    src/channelinfomodel.h \
    src/datarecorder.h \
    src/defines.h \
    src/indexbuffer.h \
    src/ledwidget.h \
    src/linindexbuffer.h \
    src/plotmenu.h \
    src/readonlybuffer.h \
    src/ringbuffer.h \
    src/samplecounter.h \
    src/samplepack.h \
    src/scrollbar.h \
    src/scrollzoomer.h \
    src/sink.h \
    src/source.h \
    src/streamchannel.h \
    src/stream.h \
    src/version.h \
    src/versionnumber.h \
    src/zoomer.h

FORMS += \
    src/mainwindow.ui \
    src/about_dialog.ui \
    src/portcontrol.ui \
    src/snapshotview.ui \
    src/commandpanel.ui \
    src/commandwidget.ui \
    src/dataformatpanel.ui \
    src/plotcontrolpanel.ui \
    src/numberformatbox.ui \
    src/endiannessbox.ui \
    src/framedreadersettings.ui \
    src/binarystreamreadersettings.ui \
    src/asciireadersettings.ui \
    src/recordpanel.ui \
    src/updatecheckdialog.ui \
    src/demoreadersettings.ui \
    src/datatextview.ui

INCLUDEPATH += qmake/ src/

CONFIG += c++11

RESOURCES += misc/icons.qrc

win32 {
    RESOURCES += misc/winicons.qrc
}
