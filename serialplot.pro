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
    customcheckablebutton.cpp

HEADERS  += mainwindow.h \
    utils.h \
    customcheckablebutton.h

FORMS    += mainwindow.ui

CONFIG += c++11
