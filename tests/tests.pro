#-------------------------------------------------
#
# Project created by QtCreator 2017-01-19T16:45:13
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_calculator
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

SOURCES += ../src/smdresistorcalculator.cpp \
    tests.cpp \
    testoctopartapi.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

DISTFILES += \
    octopart_query_response.json

HEADERS += \
    testoctopartapi.h


lessThan(QT_MAJOR_VERSION, 5){

 INCLUDEPATH += $$PWD/../qjson-backport
 DEPENDPATH += $$PWD/../qjson-backport

 win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/release/ -lqjson-backport
 else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/debug/ -lqjson-backport
 else:unix: LIBS += -L$$OUT_PWD/../qjson-backport/ -lqjson-backport

}

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../octopart/release/ -loctopart
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../octopart/debug/ -loctopart
else:unix: LIBS += -L$$OUT_PWD/../octopart/ -loctopart

INCLUDEPATH += $$PWD/../octopart
DEPENDPATH += $$PWD/../octopart

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../octopart/release/liboctopart.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../octopart/debug/liboctopart.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../octopart/release/octopart.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../octopart/debug/octopart.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../octopart/liboctopart.a
