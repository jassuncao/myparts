#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T12:38:40
#
#-------------------------------------------------

#include(../QProgressIndicator/qprogressindicator.pri)

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = octopart-test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qtableviewwithprogress.cpp \
    octopartimportdialog.cpp \
    qsearchlineedit2.cpp \
    checkboxheaderview.cpp

HEADERS  += mainwindow.h \
    qtableviewwithprogress.h \
    octopartimportdialog.h \
    qsearchlineedit2.h \
    checkboxheaderview.h

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

lessThan(QT_MAJOR_VERSION, 5){

 INCLUDEPATH += $$PWD/../qjson-backport
 DEPENDPATH += $$PWD/../qjson-backport

 win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/release/ -lqjson-backport
 else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qjson-backport/debug/ -lqjson-backport
 else:unix: LIBS += -L$$OUT_PWD/../qjson-backport/ -lqjson-backport

}

FORMS += \
    octopartimportdialog.ui

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../widgets/release/ -lwidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../widgets/debug/ -lwidgets
else:unix:!macx: LIBS += -L$$OUT_PWD/../widgets/ -lwidgets

INCLUDEPATH += $$PWD/../widgets
DEPENDPATH += $$PWD/../widgets

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/release/libwidgets.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/debug/libwidgets.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/release/widgets.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/debug/widgets.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../widgets/libwidgets.a
