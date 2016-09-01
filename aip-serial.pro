#-------------------------------------------------
#
# Project created by QtCreator 2016-08-27T16:20:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aip-serial
TEMPLATE = app


SOURCES += main.cpp \
    CWinSerial.cpp \
    qextserialbase.cpp \
    posix_qextserialport.cpp

HEADERS  += \
    CWinSerial.h \
    qextserialbase.h \
    posix_qextserialport.h

FORMS    += \
    CWinSerial.ui

RESOURCES += \
    image.qrc
