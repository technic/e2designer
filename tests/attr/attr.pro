QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

include(../../src/src.pri)

SOURCES +=  tst_testattr.cpp
