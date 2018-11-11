QT += core widgets xml

TARGET = src
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PRECOMPILED_HEADER = stable.hpp
CONFIG += precompile_header warn_on

include(../Qt-Color-Widgets/color_widgets.pri)
DEFINES += QTCOLORWIDGETS_STATICALLY_LINKED

SOURCES += \
    mainwindow.cpp \
    skindelegate.cpp \
    attr/attritem.cpp \
    attr/attrgroupitem.cpp \
    attr/enumitem.cpp \
    attr/integeritem.cpp \
    attr/pixmapitem.cpp \
    attr/positionitem.cpp \
    attr/sizeitem.cpp \
    attr/textitem.cpp \
    attr/variantitem.cpp \
    attr/coloritem.cpp \
    attr/fontitem.cpp \
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
    adapter/attritemfactory.cpp \
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
    skin/converter.cpp

HEADERS += \
    mainwindow.hpp \
    skindelegate.hpp \
    attr/enumitem.hpp \
    attr/fontitem.hpp \
    attr/integeritem.hpp \
    attr/pixmapitem.hpp \
    attr/positionitem.hpp \
    attr/sizeitem.hpp \
    attr/coloritem.hpp \
    attr/textitem.hpp \
    attr/variantitem.hpp \
    colorlistwindow.hpp \
    colorlistbox.hpp \
    listbox.hpp \
    editor/xmlhighlighter.hpp \
    scene/widgetview.hpp \
    scene/screenview.hpp \
    scene/sceneview.hpp \
    scene/rectselector.hpp \
    scene/recthandle.hpp \
    attr/attritem.hpp \
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
    attr/attrgroupitem.hpp \
    adapter/attritemfactory.hpp \
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
    skin/attrcontainer.hpp \
    base/flagsetter.hpp \
    skin/enums.hpp \
    skin/fontattr.hpp \
    skin/colorattr.hpp \
    skin/fontattr.hpp \
    skin/sizeattr.hpp \
    skin/positionattr.hpp \
    skin/enumattr.hpp \
    skin/converter.hpp

FORMS += \
    ui/mainwindow.ui \
    ui/colorlistwindow.ui \
    ui/fontlistwindow.ui

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    src.pri
