#
# Copyright © 2015 Hasan Yavuz Özderya
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

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serialplot
TEMPLATE = app

CONFIG += qwt


SOURCES += main.cpp\
        mainwindow.cpp \
    portcontrol.cpp \
    plot.cpp \
    zoomer.cpp \
    hidabletabwidget.cpp \
    framebuffer.cpp \
    scalepicker.cpp \
    scalezoomer.cpp \
    portlist.cpp \
    snapshotview.cpp \
    snapshotmanager.cpp \
    snapshot.cpp \
    plotsnapshotoverlay.cpp

HEADERS  += mainwindow.h \
    utils.h \
    portcontrol.h \
    floatswap.h \
    plot.h \
    zoomer.h \
    hidabletabwidget.h \
    framebuffer.h \
    scalepicker.h \
    scalezoomer.h \
    portlist.h \
    snapshotview.h \
    snapshotmanager.h \
    snapshot.h \
    plotsnapshotoverlay.h

FORMS    += mainwindow.ui \
    about_dialog.ui \
    portcontrol.ui \
    snapshotview.ui

INCLUDEPATH += qmake/

CONFIG += c++11

RESOURCES += \
    misc/icons.qrc
