TEMPLATE = subdirs

SUBDIRS += \
    Server

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    Doxyfile
