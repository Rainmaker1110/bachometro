#-------------------------------------------------
#
# Project created by QtCreator 2015-11-20T15:17:35
#
#-------------------------------------------------

QT       += core gui\
	serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network

TARGET = Bachometro
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    sgsmooth.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    sgsmooth.h

FORMS    += mainwindow.ui

CONFIG += c++11
