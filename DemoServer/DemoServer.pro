#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T17:49:43
#
#-------------------------------------------------

QT       += core network websockets

QT       -= gui

TARGET = DemoServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp

HEADERS += \
    server.h
