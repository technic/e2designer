QT += core widgets xml svg

TARGET = src
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(pch) {
    PRECOMPILED_HEADER = stable.hpp
    CONFIG += precompile_header
}

*msvc* { # Visual studio spec filter
    QMAKE_CXXFLAGS += -MP
}

CONFIG += warn_on

include(../Qt-Color-Widgets/color_widgets.pri)
DEFINES += QTCOLORWIDGETS_STATICALLY_LINKED

gcc | clang {
    include(../AppImageUpdaterBridge/AppImageUpdaterBridge.pri)
    DEFINES += APPIMAGE_UPDATE
}

SOURCES += \
    mainwindow.cpp \
    skindelegate.cpp \
    colorlistwindow.cpp \
    colorlistbox.cpp \
    listbox.cpp \
    scene/sceneview.cpp \
    scene/screenview.cpp \
    scene/recthandle.cpp \
    editor/xmlhighlighter.cpp \
    scene/widgetview.cpp \
    scene/backgroundpixmap.cpp \
    scene/foregroundwidget.cpp \
    editor/codeeditor.cpp \
    scene/rectselector.cpp \
    base/singleton.cpp \
    repository/xmlnode.cpp \
    repository/skinrepository.cpp \
    skin/widgetdata.cpp \
    repository/videooutputrepository.cpp \
    base/uniqueid.cpp \
    repository/pixmapstorage.cpp \
    base/tree.cpp \
    commands/attrcommand.cpp \
    fontlistwindow.cpp \
    model/namedlist.cpp \
    model/colorsmodel.cpp \
    model/fontsmodel.cpp \
    model/propertiesmodel.cpp \
    model/screensmodel.cpp \
    model/borderset.cpp \
    model/windowstyle.cpp \
    scene/borderview.cpp \
    base/flagsetter.cpp \
    skin/attributes.cpp \
    skin/colorattr.cpp \
    skin/fontattr.cpp \
    skin/sizeattr.cpp \
    skin/positionattr.cpp \
    skin/enumattr.cpp \
    skin/converter.cpp \
    model/propertytree.cpp \
    skin/offsetattr.cpp \
    model/outputsmodel.cpp \
    model/movablelistmodel.cpp \
    outputslistwindow.cpp \
    base/xmlstreamwriter.cpp

HEADERS += \
    mainwindow.hpp \
    skindelegate.hpp \
    colorlistwindow.hpp \
    colorlistbox.hpp \
    listbox.hpp \
    editor/xmlhighlighter.hpp \
    scene/widgetview.hpp \
    scene/screenview.hpp \
    scene/sceneview.hpp \
    scene/rectselector.hpp \
    scene/recthandle.hpp \
    scene/backgroundpixmap.hpp \
    editor/codeeditor.hpp \
    scene/foregroundwidget.hpp \
    base/singleton.hpp \
    repository/xmlnode.hpp \
    repository/skinrepository.hpp \
    skin/widgetdata.hpp \
    repository/videooutputrepository.hpp \
    base/uniqueid.hpp \
    repository/pixmapstorage.hpp \
    base/tree.hpp \
    commands/attrcommand.hpp \
    fontlistwindow.hpp \
    model/namedlist.hpp \
    model/colorsmodel.hpp \
    model/fontsmodel.hpp \
    model/propertiesmodel.hpp \
    model/screensmodel.hpp \
    model/borderset.hpp \
    model/windowstyle.hpp \
    scene/borderview.hpp \
    base/flagsetter.hpp \
    skin/attributes.hpp \
    skin/enums.hpp \
    skin/fontattr.hpp \
    skin/colorattr.hpp \
    skin/fontattr.hpp \
    skin/sizeattr.hpp \
    skin/positionattr.hpp \
    skin/enumattr.hpp \
    skin/converter.hpp \
    model/propertytree.hpp \
    skin/offsetattr.hpp \
    base/meta.hpp \
    base/typelist.hpp \
    model/outputsmodel.hpp \
    model/movablelistmodel.hpp \
    outputslistwindow.hpp \
    base/xmlstreamwriter.hpp

FORMS += \
    ui/mainwindow.ui \
    ui/colorlistwindow.ui \
    ui/fontlistwindow.ui \
    ui/outputslistwindow.ui

RESOURCES += \
    data.qrc

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    target.path = $$PREFIX/lib
    INSTALLS += target
}

DISTFILES += \
    src.pri

win32 {
    DOXYGEN_BIN = $$system(where doxygen)
}
unix {
    DOXYGEN_BIN = $$system(which doxygen)
}
isEmpty(DOXYGEN_BIN) {
    message("Doxygen not found")
} else {
    message("Doxygen found in" $$DOXYGEN_BIN)
    qtPrepareTool(QHELPGENERATOR, qhelpgenerator)
    win32 {
        docs.commands += set QHG_LOCATION="$$QHELPGENERATOR" && doxygen $$PWD/../Doxyfile
    } unix {
        docs.commands += QHG_LOCATION="$$QHELPGENERATOR" doxygen $$PWD/../Doxyfile
    }
    QMAKE_EXTRA_TARGETS += docs
    POST_TARGETDEPS += docs
}
