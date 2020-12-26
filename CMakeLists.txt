cmake_minimum_required(VERSION 3.9)
project(e2designer)

set(CMAKE_CXX_STANDARD 17)

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

# PEGTL
set(PEGTL_BUILD_TESTS OFF CACHE BOOL "Build PEGTL tests")
set(PEGTL_BUILD_EXAMPLES OFF CACHE BOOL "Build PEGTL examples")
add_subdirectory(pegtl)

# Application core
set(SOURCE
    src/base/flagsetter.hpp
    src/base/xmlstreamwriter.hpp
    src/parser/parser.cpp
    src/colorlistbox.cpp
    src/skindelegate.cpp
    src/colorlistwindow.cpp
    src/customtreeview.hpp
    src/commands/attrcommand.cpp
    src/editor/codeeditor.cpp
    src/editor/xmlhighlighter.cpp
    src/fontlistwindow.cpp
    src/listbox.cpp
    src/mainwindow.cpp
    src/model/bordersmodel.cpp
    src/model/colorsmodel.cpp
    src/model/fontsmodel.cpp
    src/model/movablelistmodel.cpp
    src/model/outputsmodel.cpp
    src/model/propertiesmodel.cpp
    src/model/propertytree.cpp
    src/model/screensmodel.cpp
    src/model/windowstyle.cpp
    src/outputslistwindow.cpp
    src/repository/pixmapstorage.cpp
    src/repository/skinrepository.cpp
    src/repository/xmlnode.cpp
    src/scene/backgroundpixmap.cpp
    src/scene/borderview.cpp
    src/scene/foregroundwidget.cpp
    src/scene/recthandle.cpp
    src/scene/rectselector.cpp
    src/scene/sceneview.cpp
    src/scene/screenview.cpp
    src/scene/widgetview.cpp
    src/scene/entrypainter.cpp
    src/skin/borders.cpp
    src/skin/colorattr.cpp
    src/skin/converter.cpp
    src/skin/enumattr.cpp
    src/skin/fontattr.cpp
    src/skin/offsetattr.cpp
    src/skin/positionattr.cpp
    src/skin/sizeattr.cpp
    src/skin/widgetdata.cpp
    src/skin/includefile.cpp
    resources.qrc
    src/skin/enums.hpp
)

include_directories(src)
add_library(srclib ${SOURCE})
target_link_libraries(srclib Qt5::Core Qt5::Widgets Qt5::Gui QtColorWidgets taocpp::pegtl)

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

# installation
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if(UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT application)
    install(FILES misc/e2designer.desktop DESTINATION share/applications)
    install(FILES misc/e2designer.png DESTINATION share/icons/hicolor/256x256/app)
    install(FILES misc/e2designer.appdata.xml DESTINATION share/metainfo)
endif()

if(WIN32)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT application)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    # At build stage we copy required files to temporary folder
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" 
        ARGS --verbose 1 --dir "${CMAKE_CURRENT_BINARY_DIR}/deployqt" "$<TARGET_FILE:${PROJECT_NAME}>"
        DEPENDS ${PROJECT_NAME}
        COMMENT "Running windeployqt..."
    )
    # At intall stage we copy contents of this folder to destination
    install(DIRECTORY "${CMAKE_BINARY_DIR}/deployqt/" DESTINATION bin)
endif()

if(APPLE)
    install(TARGETS ${PROJECT_NAME} RUNTIME COMPONENT application)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif()
