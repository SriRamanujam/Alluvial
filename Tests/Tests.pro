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
    songhandler.cpp \
    Spotify/qtspotifysession.cpp \
    Spotify/qtspotifywrapper.cpp \
    Settings/settings_storage.cpp \
    Settings/jsonformatter.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    communicationhandler.h \
    schandler.h \
    playlisthandler.h \
    JSONhandler.h \
    databasehandler.h \
    queryhandler.h \
    songhandler.h \
    Spotify/qtspotifysession.h \
    Spotify/qtspotifywrapper.h \
    Settings/settings_storage.h \
    Settings/jsonformatter.h

unix:!macx: LIBS += -ltag

unix:!macx: LIBS += -lspotify
