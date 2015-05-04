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
    communicationhandler.cpp \
    schandler.cpp \
    playlisthandler.cpp \
    JSONhandler.cpp \
    databasehandler.cpp \
    queryhandler.cpp \
    songhandler.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    communicationhandler.h \
    schandler.h \
    playlisthandler.h \
    JSONhandler.h \
    databasehandler.h \
    queryhandler.h \
    songhandler.h

unix:!macx: LIBS += -ltag
