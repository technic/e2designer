cmake_minimum_required(VERSION 3.9)
project(e2designer)

set(CMAKE_CXX_STANDARD 14)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(SOURCE
    src/base/flagsetter.cpp
    src/base/singleton.cpp
    src/base/tree.cpp
    src/base/uniqueid.cpp
    src/base/xmlstreamwriter.cpp
    src/colorlistbox.cpp
    src/colorlistwindow.cpp
    src/commands/attrcommand.cpp
    src/editor/codeeditor.cpp
    src/editor/xmlhighlighter.cpp
    src/fontlistwindow.cpp
    src/listbox.cpp
    src/mainwindow.cpp
    src/model/borderset.cpp
    src/model/colorsmodel.cpp
    src/model/fontsmodel.cpp
    src/model/movablelistmodel.cpp
    src/model/namedlist.cpp
    src/model/outputsmodel.cpp
    src/model/propertiesmodel.cpp
    src/model/propertytree.cpp
    src/model/screensmodel.cpp
    src/model/windowstyle.cpp
    src/outputslistwindow.cpp
    src/repository/pixmapstorage.cpp
    src/repository/skinrepository.cpp
    src/repository/videooutputrepository.cpp
    src/repository/xmlnode.cpp
    src/scene/backgroundpixmap.cpp
    src/scene/borderview.cpp
    src/scene/foregroundwidget.cpp
    src/scene/recthandle.cpp
    src/scene/rectselector.cpp
    src/scene/sceneview.cpp
    src/scene/screenview.cpp
    src/scene/widgetview.cpp
    src/skin/attributes.cpp
    src/skin/colorattr.cpp
    src/skin/converter.cpp
    src/skin/enumattr.cpp
    src/skin/fontattr.cpp
    src/skin/offsetattr.cpp
    src/skin/positionattr.cpp
    src/skin/sizeattr.cpp
    src/skin/widgetdata.cpp
    src/skindelegate.cpp
    app/main.cpp
    resources.qrc
    src/data.qrc
    src/skin/enums.hpp
)

add_executable(${PROJECT_NAME} ${SOURCE})

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build QtColorWidgets shared library")
set(BUILD_STATIC_LIBS ON  CACHE BOOL "Build QtColorWidgets static library")
add_subdirectory(Qt-Color-Widgets)

#qt5_use_modules(e2designer Core Widgets)

target_link_libraries(e2designer Qt5::Core Qt5::Widgets QtColorWidgets)
target_include_directories(e2designer PRIVATE src Qt-Color-Widgets/include)
