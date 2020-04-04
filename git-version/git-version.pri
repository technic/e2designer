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

# qmake rules to generate gitversion.hpp
versiontarget.depends = FORCE
versiontarget.output = $$VERSION_FILE
win32 {
    # Change directory to QTDIR where required dll's are located
    versiontarget.commands += $$QMAKE_CD "$(QTDIR)\bin" &&
}
versiontarget.commands += $${TOOLFILE} -C "$$_PRO_FILE_PWD_" "${QMAKE_FILE_OUT}"
# link to qmake variables
versiontarget.input = VERSION_FILE
versiontarget.variable_out = HEADERS

QMAKE_EXTRA_COMPILERS += versiontarget
