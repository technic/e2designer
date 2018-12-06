#!/bin/bash
set -e -x
make
make -C app INSTALL_ROOT=$(pwd)/appdir install
linuxdeployqt appdir/usr/share/applications/e2designer.desktop -bundle-non-qt-libs
appimagetool -u "bintray-zsync|technic|e2designer|e2designer|e2designer-_latestVersion-x86_64.AppImage.zsync" appdir
./e2designer-x86_64.AppImage
