#-------------------------------------------------
#
# Project created by QtCreator 2018-10-28T18:55:50
#
#-------------------------------------------------

TARGET = widgets
TEMPLATE = lib
CONFIG += lib

DEFINES += WIDGETS_LIBRARY

SOURCES += qprogressindicator.cpp \
    qsearchlineedit.cpp \
    checkboxheaderview.cpp

HEADERS += qprogressindicator.h \
    qsearchlineedit.h \
    checkboxheaderview.h \
    QSearchLineEdit \
    QProgressIndicator \
    widgets_global.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
