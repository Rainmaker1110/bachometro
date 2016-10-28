#-------------------------------------------------
#
# Project created by QtCreator 2015-11-20T15:17:35
#
#-------------------------------------------------

QT       += core gui\
	serialport \
	network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Bachometro
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    qcustomplot.cpp \
    sgsmooth.cpp \
    coordinatesregister.cpp \
    sensordatamanager.cpp \
    sensordataprocessor.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    sgsmooth.h \
    coordinatesregister.h \
    sensordatamanager.h \
    arduinodata.h \
    sensordataprocessor.h

FORMS	+= mainwindow.ui

CONFIG	+= c++11
