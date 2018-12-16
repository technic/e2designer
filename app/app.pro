QT += core gui widgets

TEMPLATE = app

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../src/src.pri)

TARGET = e2designer

SOURCES += \
        main.cpp

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }

    target.path = $$PREFIX/bin

    desktopFiles.files = ../misc/e2designer.desktop
    desktopFiles.path = $$PREFIX/share/applications/

    iconFiles.files = ../misc/e2designer.png
    iconFiles.path = $$PREFIX/share/pixmaps/

    INSTALLS += desktopFiles iconFiles
}

INSTALLS += target

RESOURCES += \
    ../resources.qrc
