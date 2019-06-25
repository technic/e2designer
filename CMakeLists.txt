cmake_minimum_required(VERSION 3.9)
project(e2designer)

set(CMAKE_CXX_STANDARD 14)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt
find_package(Qt5 COMPONENTS Core Widgets Gui Test REQUIRED)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build QtColorWidgets shared library")
set(BUILD_STATIC_LIBS ON  CACHE BOOL "Build QtColorWidgets static library")
add_subdirectory(Qt-Color-Widgets)

# Application core
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
    resources.qrc
    src/data.qrc
    src/skin/enums.hpp
)

include_directories(src)
add_library(srclib ${SOURCE})
target_link_libraries(srclib Qt5::Core Qt5::Widgets Qt5::Gui QtColorWidgets)

# Application executable
add_executable(${PROJECT_NAME} app/main.cpp)
target_link_libraries(${PROJECT_NAME} srclib)
#target_include_directories(e2designer PRIVATE src Qt-Color-Widgets/include)

enable_testing()
add_subdirectory(tests)

# git version
find_package(Git)
add_custom_target(
    version ${CMAKE_COMMAND} 
    -D local_dir=${CMAKE_SOURCE_DIR} -D output_dir=${CMAKE_BINARY_DIR}
    -P ${CMAKE_SOURCE_DIR}/git-version/git-version.cmake
)
add_dependencies(${PROJECT_NAME} version)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/generated)
