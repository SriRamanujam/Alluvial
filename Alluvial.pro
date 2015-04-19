TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    mediahandler.cpp \
    searchresult.cpp \
    simplecrypt.cpp

RESOURCES +=

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

CONFIG += c++11

HEADERS += \
    mediahandler.h \
    searchresult.h \
    simplecrypt.h
