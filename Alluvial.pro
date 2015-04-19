TEMPLATE = app

QT += qml quick widgets sql

SOURCES += main.cpp \
    mediahandler.cpp \
    searchresult.cpp \
    simplecrypt.cpp \
    JSONhandler.cpp \
    queryhandler.cpp \
    songhandler.cpp \
    databasehandler.cpp

RESOURCES +=

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

CONFIG += c++11

HEADERS += \
    mediahandler.h \
    searchresult.h \
    simplecrypt.h \
    JSONhandler.h \
    queryhandler.h \
    songhandler.h \
    databasehandler.h
