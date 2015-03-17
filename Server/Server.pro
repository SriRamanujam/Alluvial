#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T17:49:43
#
#-------------------------------------------------

QT       += core network websockets

QT       -= gui

TARGET = Server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    activesockets.cpp \
    clientconnection.cpp

HEADERS += \
    server.h \
    activesockets.h \
    clientconnection.h
