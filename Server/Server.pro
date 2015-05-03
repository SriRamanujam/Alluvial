TEMPLATE = app

QT += websockets sql

CONFIG += c++11

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

unix:!macx: LIBS += -ltag

unix:!macx: LIBS += -lspotify

HEADERS += \
    Local/databasehandler.h \
    Local/JSONhandler.h \
    Local/playlisthandler.h \
    Local/queryhandler.h \
    Local/songhandler.h \
    Settings/jsonformatter.h \
    Settings/settings_storage.h \
    Soundcloud/schandler.h \
    Spotify/qtlibspotify.h \
    Spotify/qtlibspotifyhandler.h \
    activesockets.h \
    clientconnection.h \
    mediahandler.h \
    searchresult.h \
    server.h \
    simplecrypt.h

SOURCES += \
    main.cpp \
    Local/databasehandler.cpp \
    Local/JSONhandler.cpp \
    Local/playlisthandler.cpp \
    Local/queryhandler.cpp \
    Local/songhandler.cpp \
    Settings/jsonformatter.cpp \
    Settings/settings_storage.cpp \
    Soundcloud/schandler.cpp \
    Spotify/qtlibspotify.cpp \
    Spotify/qtlibspotifyhandler.cpp \
    activesockets.cpp \
    clientconnection.cpp \
    mediahandler.cpp \
    searchresult.cpp \
    server.cpp \
    simplecrypt.cpp

