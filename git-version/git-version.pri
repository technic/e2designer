INCLUDEPATH += $$PWD

# Sets the win32 output dir WINDIR
CONFIG(debug, debug|release) {
    WINDIR = debug
}
CONFIG(release, debug|release) {
    WINDIR = release
}

# Determine subproject relative path
SRCDIR = $$relative_path($$PWD, $$_PRO_FILE_PWD_)
# Use it to get subproject output directory
OUTDIR = $$clean_path($$OUT_PWD/$${SRCDIR})

GITTOOL = git-version

win32 {
    TOOLFILE = $${OUTDIR}/$${WINDIR}/$${GITTOOL}.exe
}
else {
    TOOLFILE = $${OUTDIR}/$${GITTOOL}
}
message(Using executable $${TOOLFILE})

VERSION_FILE = $$OUT_PWD/gitversion.hpp
message(Store version in $${VERSION_FILE})

# Makefile target required to build gitversion.o
versiontarget.target = $${VERSION_FILE}
win32 {
    # Change directory to QTDIR where required dll's are located
    versiontarget.commands += $$QMAKE_CD "$(QTDIR)\bin" &&
}
versiontarget.commands += $${TOOLFILE} -C "$$_PRO_FILE_PWD_" "$${VERSION_FILE}"
versiontarget.depends = FORCE
QMAKE_EXTRA_TARGETS += versiontarget

HEADERS += $${VERSION_FILE}
PRE_TARGETDEPS += $${VERSION_FILE}
