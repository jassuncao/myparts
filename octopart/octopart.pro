#-------------------------------------------------
#
# Project created by QtCreator 2017-01-20T17:56:03
#
#-------------------------------------------------

QT       -= gui
QT       += network

TARGET = octopart
TEMPLATE = lib
CONFIG += staticlib

lessThan(QT_MAJOR_VERSION, 5){

 INCLUDEPATH += $$PWD/../qjson-backport
 DEPENDPATH += $$PWD/../qjson-backport

 win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/release/ -lqjson-backport
 else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/debug/ -lqjson-backport
 else:unix: LIBS += -L$$OUT_PWD/../qjson-backport/ -lqjson-backport

}

SOURCES += octopartapi.cpp \
    octopartpartsearchmodel.cpp

HEADERS += octopartapi.h \
    octopartpartsearchmodel.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}


