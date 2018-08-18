#-------------------------------------------------
#
# Project created by QtCreator 2017-01-12T16:41:43
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#TARGET = e2designer
TEMPLATE = app

CONFIG += c++11
INCLUDEPATH += $${PWD}/src/

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

*msvc* {
    # visual studio parallel compile
    QMAKE_CXXFLAGS += -MP
}

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/skindelegate.cpp \
#    src/headerfilter.cpp \
    src/attr/attritem.cpp \
    src/attr/attrgroupitem.cpp \
    src/attr/enumitem.cpp \
    src/attr/integeritem.cpp \
    src/attr/pixmapitem.cpp \
    src/attr/positionitem.cpp \
    src/attr/sizeitem.cpp \
    src/attr/textitem.cpp \
    src/attr/variantitem.cpp \
    src/attr/coloritem.cpp \
    src/attr/fontitem.cpp \
    src/colorlistwindow.cpp \
    src/colorlistbox.cpp \
    src/listbox.cpp \
    src/scene/sceneview.cpp \
    src/scene/screenview.cpp \
    src/scene/recthandle.cpp \
    src/editor/xmlhighlighter.cpp \
    src/scene/widgetview.cpp \
    src/scene/backgroundpixmap.cpp \
    src/scene/foregroundwidget.cpp \
#    src/scene/borderview.cpp \
    src/editor/codeeditor.cpp \
    src/scene/rectselector.cpp \
    src/base/singleton.cpp \
    src/repository/xmlnode.cpp \
    src/repository/skinrepository.cpp \
    src/skin/widgetdata.cpp \
#    src/repository/treenode.cpp \
#    src/repository/previewsrepository.cpp \
    src/repository/videooutputrepository.cpp \
    src/base/uniqueid.cpp \
    src/repository/pixmapstorage.cpp \
    src/base/tree.cpp \
    src/adapter/attritemfactory.cpp \
    src/commands/attrcommand.cpp \
    src/fontlistwindow.cpp \
    src/model/namedlist.cpp \
    src/model/colorsmodel.cpp \
    src/model/fontsmodel.cpp \
    src/model/propertiesmodel.cpp \
    src/model/screensmodel.cpp \
    src/model/borderset.cpp \
    src/model/windowstyle.cpp \
    src/scene/borderview.cpp \
    src/base/flagsetter.cpp \
    src/skin/attributes.cpp \
    src/skin/colorattr.cpp \
    src/skin/fontattr.cpp \
    src/skin/sizeattr.cpp \
    src/skin/positionattr.cpp \
    src/skin/enumattr.cpp \
    src/skin/converter.cpp

HEADERS += \
    src/mainwindow.hpp \
    src/skindelegate.hpp \
    src/attr/enumitem.hpp \
    src/attr/fontitem.hpp \
    src/attr/integeritem.hpp \
    src/attr/pixmapitem.hpp \
    src/attr/positionitem.hpp \
    src/attr/sizeitem.hpp \
    src/attr/coloritem.hpp \
    src/attr/textitem.hpp \
    src/attr/variantitem.hpp \
    src/colorlistwindow.hpp \
    src/colorlistbox.hpp \
    src/listbox.hpp \
    src/editor/xmlhighlighter.hpp \
    src/scene/widgetview.hpp \
#    src/element/widgetitem.hpp \
#    src/model/toplevelfilter.hpp \
#    src/model/skinmodel.hpp \
    src/scene/screenview.hpp \
    src/scene/sceneview.hpp \
    src/scene/rectselector.hpp \
    src/scene/recthandle.hpp \
    src/attr/attritem.hpp \
#    src/model/headerfilter.hpp  \
    src/scene/backgroundpixmap.hpp \
#    src/scene/borderview.hpp \
    src/editor/codeeditor.hpp \
    src/scene/foregroundwidget.hpp \
#    src/element/skinborder.hpp \
#    src/element/skinitem.hpp \
    src/base/singleton.hpp \
    src/repository/xmlnode.hpp \
    src/repository/skinrepository.hpp \
    src/skin/widgetdata.hpp \
#    src/repository/treenode.hpp \
#    src/repository/previewsrepository.hpp \
    src/repository/videooutputrepository.hpp \
    src/base/uniqueid.hpp \
    src/repository/pixmapstorage.hpp \
    src/base/tree.hpp \
    src/attr/attrgroupitem.hpp \
    src/adapter/attritemfactory.hpp \
    src/commands/attrcommand.hpp \
    src/fontlistwindow.hpp \
    src/model/namedlist.hpp \
    src/model/colorsmodel.hpp \
    src/model/fontsmodel.hpp \
    src/model/propertiesmodel.hpp \
    src/model/screensmodel.hpp \
    src/model/borderset.hpp \
    src/model/windowstyle.hpp \
    src/scene/borderview.hpp \
    src/skin/attrcontainer.hpp \
    src/base/flagsetter.hpp \
    src/skin/enums.hpp \
    src/skin/fontattr.hpp \
    src/skin/colorattr.hpp \
    src/skin/fontattr.hpp \
    src/skin/sizeattr.hpp \
    src/skin/positionattr.hpp \
    src/skin/enumattr.hpp \
    src/skin/converter.hpp

FORMS += \
    mainwindow.ui \
    colorlistwindow.ui \
    fontlistwindow.ui

test {
    message(Configure test build)
    SOURCES -= src/main.cpp
    include(tests/tests.pri)

    TARGET = tests
} else {
    TARGET = e2designer
}

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }

    target.path = $$PREFIX/bin

    desktopFiles.files = misc/e2designer.desktop
    desktopFiles.path = $$PREFIX/share/applications/

    iconFiles.files = misc/e2designer.png
    iconFiles.path = $$PREFIX/share/pixmaps/

    INSTALLS += desktopFiles iconFiles
}

INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += LICENSE README.md .gitlab-ci.yml .appveyor.yml
