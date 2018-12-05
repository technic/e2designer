# Some parts of this qmake pri file
# were copied from https://gitlab.com/mxklb/cuteproject
# Here is the author's copyright note
# Copyright (c) 2017 mxklb

INCLUDEPATH += $$PWD
CONFIG += static
QT += widgets network

# Sets the win32 output dir WINDIR
CONFIG(debug, debug|release) {
    WINDIR = debug
}
CONFIG(release, debug|release) {
    WINDIR = release
}

LIB_EXTENSION = $$QMAKE_EXTENSION_STATICLIB
isEmpty(LIB_EXTENSION) {
    error("cannot determine lib extension")
}

LIBNAME = src
# Determine subproject relative path
LIBDIR = $$relative_path($$PWD, $$_PRO_FILE_PWD_)
# Use it to get library's binary directory
OUTDIR = $$clean_path($$OUT_PWD/$${LIBDIR})
#message("Binary dir: $${OUTDIR}")

macx {
    frameworks += $$files($${OUTDIR}/$${LIBNAME}.framework)
    LIBS += -F$${OUTDIR}/ -framework $${LIBNAME}
    PRE_TARGETDEPS += $${OUTDIR}/$${LIBNAME}.framework
    INCLUDEPATH += -F$${LIBDIR}
}
win32 {
    *msvc* {
        LIBS += $${OUTDIR}/$${WINDIR}/$${LIBNAME}.$${LIB_EXTENSION}
        PRE_TARGETDEPS += $${OUTDIR}/$${WINDIR}/$${LIBNAME}.$${LIB_EXTENSION}
    } else {
        LIBS += $${OUTDIR}/$${WINDIR}/lib$${LIBNAME}.$${LIB_EXTENSION}
        PRE_TARGETDEPS += $${OUTDIR}/$${WINDIR}/lib$${LIBNAME}.$${LIB_EXTENSION}
    }
}
unix:!macx {
    LIBS += -L$${OUTDIR}/ -l$${LIBNAME}
    PRE_TARGETDEPS += $${OUTDIR}/lib$${LIBNAME}.$${LIB_EXTENSION}
}

#message("flags: $${LIBS}")
#message("deps: $${PRE_TARGETDEPS}")
