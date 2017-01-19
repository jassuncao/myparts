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

INCLUDEPATH += ../src
DEPENDPATH += ../src

SOURCES += smdresistorcalculator.cpp \
    tests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
