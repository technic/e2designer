QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    $$PWD/tst_attr.cpp \
    $$PWD/tst_widget.cpp
