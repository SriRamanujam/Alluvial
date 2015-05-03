#-------------------------------------------------
#
# Project created by QtCreator 2015-05-03T14:09:14
#
#-------------------------------------------------

QT       += network sql testlib websockets

QT       -= gui

TARGET = tst_server
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_server.cpp \
    communicationhandler.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    communicationhandler.h
