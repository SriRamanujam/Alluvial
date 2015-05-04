TEMPLATE = app

QT += qml quick widgets multimedia websockets

RESOURCES += qml.qrc

CONFIG += c++11

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += ../appkey.h \
    communication/communicationhandler.h \
    dataobject.h \
    jsonformatter.h \
    playlist.h \
    playlist_handler.h \
    playlist_item.h \
    settings_storage.h

DISTFILES += \
    qmldir.txt \
    Doxyfile
    qmldir.txt

#unix:!macx: LIBS += -lspotify

unix:!macx: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lspotify

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include

SOURCES += \
    communication/communicationhandler.cpp \
    dataobject.cpp \
    jsonformatter.cpp \
    main.cpp \
    playlist.cpp \
    playlist_handler.cpp \
    playlist_item.cpp \
    settings_storage.cpp
