TEMPLATE = app

QT += qml quick widgets websockets sql

CONFIG += c++11

SOURCES += main.cpp \
    Server/server.cpp \
    Server/activesockets.cpp \
    Server/mediahandler.cpp \
    Server/searchresult.cpp \
    Server/simplecrypt.cpp \
    Server/clientconnection.cpp \
    Server/Soundcloud/schandler.cpp \
    Server/Local/databasehandler.cpp \
    Server/Local/JSONhandler.cpp \
    Server/Local/playlisthandler.cpp \
    Server/Local/queryhandler.cpp \
    Server/Local/songhandler.cpp \
    Server/Settings/settings_storage.cpp \
    Server/Settings/jsonformatter.cpp \
    Server/Spotify/qtspotifysession.cpp \
    Server/Spotify/qtspotifywrapper.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Server/activesockets.h \
    Server/mediahandler.h \
    Server/searchresult.h \
    Server/server.h \
    Server/simplecrypt.h \
    Server/clientconnection.h \
    Server/Soundcloud/schandler.h \
    Server/Local/databasehandler.h \
    Server/Local/JSONhandler.h \
    Server/Local/playlisthandler.h \
    Server/Local/queryhandler.h \
    Server/Local/songhandler.h \
    Server/Settings/settings_storage.h \
    Server/Settings/jsonformatter.h \
    Server/Spotify/qtspotifysession.h \
    Server/Spotify/qtspotifywrapper.h

unix:!macx: LIBS += -ltag

unix:!macx: LIBS += -lspotify

DISTFILES += \
    Doxyfile
