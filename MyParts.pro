TEMPLATE = subdirs

CONFIG+=ordered

lessThan(QT_MAJOR_VERSION, 5): SUBDIRS += qjson-backport

SUBDIRS += \
    qt-manhattan-style \
    octopart \
    src \
    tests
