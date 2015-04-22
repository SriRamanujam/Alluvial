#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T17:49:43
#
#-------------------------------------------------

QT       += core sql network websockets

QT       -= gui

TARGET = Server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    activesockets.cpp \
    clientconnection.cpp \
    databasehandler.cpp \
    jsonformatter.cpp \
    JSONHandler.cpp \
    mediahandler.cpp \
    playlisthandler.cpp \
    qtlibspotify.cpp \
    qtlibspotifyhandler.cpp \
    queryhandler.cpp \
    schandler.cpp \
    searchresult.cpp \
    settings_storage.cpp \
    simplecrypt.cpp \
    songhandler.cpp

HEADERS += \
    server.h \
    activesockets.h \
    clientconnection.h \
    databasehandler.h \
    jsonformatter.h \
    JSONHandler.h \
    mediahandler.h \
    playlisthandler.h \
    qtlibspotify.h \
    qtlibspotifyhandler.h \
    queryhandler.h \
    schandler.h \
    searchresult.h \
    settings_storage.h \
    simplecrypt.h \
    songhandler.h

CONFIG += c++11

DISTFILES +=

unix:!macx: LIBS += -ltag

unix:!macx: LIBS += -lspotify
