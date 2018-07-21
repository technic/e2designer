#-------------------------------------------------
#
# Project created by QtCreator 2017-01-12T16:41:43
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = e2designer
TEMPLATE = app

CONFIG += c++11
#INCLUDEPATH +=

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(Qt-Color-Widgets/color_widgets.pri)
DEFINES += QTCOLORWIDGETS_STATICALLY_LINKED

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    coordinate.cpp \
    skinmaps.cpp \
    skindelegate.cpp \
    dimension.cpp \
#    headerfilter.cpp \
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
#    scene/borderview.cpp \
    editor/codeeditor.cpp \
    scene/rectselector.cpp \
    base/singleton.cpp \
    repository/xmlnode.cpp \
    repository/skinrepository.cpp \
    repository/widgetdata.cpp \
#    repository/treenode.cpp \
#    repository/previewsrepository.cpp \
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
    repository/attrcontainer.cpp

HEADERS += \
    mainwindow.hpp \
    coordinate.hpp \
    skinmaps.hpp \
    skindelegate.hpp \
    dimension.hpp \
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
#    element/widgetitem.hpp \
#    model/toplevelfilter.hpp \
#    model/skinmodel.hpp \
    scene/screenview.hpp \
    scene/sceneview.hpp \
    scene/rectselector.hpp \
    scene/recthandle.hpp \
    attr/attritem.hpp \
#    model/headerfilter.hpp  \
    scene/backgroundpixmap.hpp \
#    scene/borderview.hpp \
    editor/codeeditor.hpp \
    scene/foregroundwidget.hpp \
#    element/skinborder.hpp \
#    element/skinitem.hpp \
    base/singleton.hpp \
    repository/xmlnode.hpp \
    repository/skinrepository.hpp \
    repository/widgetdata.hpp \
#    repository/treenode.hpp \
#    repository/previewsrepository.hpp \
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
    repository/attrcontainer.hpp

FORMS += \
    mainwindow.ui \
    colorlistwindow.ui \
    fontlistwindow.ui
