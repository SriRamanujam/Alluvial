#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T19:00:11
#
#-------------------------------------------------

QT       += core network multimedia websockets

QT       -= gui

TARGET = DemoClient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    client.cpp \
    communicationhandler.cpp

HEADERS += \
    client.h \
    communicationhandler.h
