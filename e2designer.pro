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
    mainwindow.h \
    coordinate.h \
    skinmaps.h \
    skindelegate.h \
    dimension.h \
    attr/enumitem.h \
    attr/fontitem.h \
    attr/integeritem.h \
    attr/pixmapitem.h \
    attr/positionitem.h \
    attr/sizeitem.h \
    attr/coloritem.h \
    attr/textitem.h \
    attr/variantitem.h \
    colorlistwindow.h \
    colorlistbox.h \
    listbox.h \
    editor/xmlhighlighter.h \
    scene/widgetview.h \
#    element/widgetitem.h \
#    model/toplevelfilter.h \
#    model/skinmodel.h \
    scene/screenview.h \
    scene/sceneview.h \
    scene/rectselector.h \
    scene/recthandle.h \
    attr/attritem.h \
#    model/headerfilter.h  \
    scene/backgroundpixmap.h \
#    scene/borderview.h \
    editor/codeeditor.h \
    scene/foregroundwidget.h \
#    element/skinborder.h \
#    element/skinitem.h \
    base/singleton.h \
    repository/xmlnode.h \
    repository/skinrepository.h \
    repository/widgetdata.h \
#    repository/treenode.h \
#    repository/previewsrepository.h \
    repository/videooutputrepository.h \
    base/uniqueid.h \
    repository/pixmapstorage.h \
    base/tree.h \
    attr/attrgroupitem.h \
    adapter/attritemfactory.h \
    commands/attrcommand.h \
    fontlistwindow.h \
    model/namedlist.h \
    model/colorsmodel.h \
    model/fontsmodel.h \
    model/propertiesmodel.h \
    model/screensmodel.h \
    model/borderset.h \
    model/windowstyle.h \
    scene/borderview.h \
    repository/attrcontainer.h

FORMS += \
    mainwindow.ui \
    colorlistwindow.ui \
    fontlistwindow.ui
