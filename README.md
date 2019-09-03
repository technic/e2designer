# e2designer

Skin editor for enigma2 written in C++/Qt5 framework.

[ ![Download](https://api.bintray.com/packages/technic/e2designer/e2designer/images/download.svg) ](https://bintray.com/technic/e2designer/e2designer/_latestVersion#files)

# Screenshot

![Application window.](https://gitlab.com/technic93/e2designer/raw/master/screenshot.png)

# Insallation

Portable builds:

- Windows - download & extract.
- Portable Linux version - download, allow to execute and run.

# Controls

- Ctrl + Mouse wheel - Zoom In/Out
- Arrows - Move widget
- Shift + Arrows - Precise move widget

# Build dependencies

System with C++17 support and Qt version 5.11.

# Compile from sources

```
qmake
make
```

# Contributing

1. Use \*.hpp and \*.cpp for file extensions.
2. Use style defined by clang-format in the project root directory, stay with clang-8.
4. Report issues in GitLab [bugtracker](https://gitlab.com/technic93/e2designer/issues)
3. Fork and send GitLab pull requests.
4. Join discussion on the [openpli forum](https://forums.openpli.org/topic/61204-e2designer-alpha/)

# License
GPLv3

# Updating subtrees
We need to pull upstream from time to time:
```
git subtree pull -P Qt-Color-Widgets/ --squash git@gitlab.com:mattia.basaglia/Qt-Color-Widgets.git master
git subtree pull -P AppImageUpdaterBridge --squash https://github.com/antony-jr/AppImageUpdaterBridge.git master
```

# Develop notes
To start linux docker image with qt and compiler
```bash
docker-compose run e2designer bash
```

# Credits
Libraries:
- [Qt-Color-Widgets](https://github.com/mbasaglia/Qt-Color-Widgets)
- [AppImageUpdaterBridge](https://github.com/antony-jr/AppImageUpdaterBridge)

Application icon made by [Picol](https://www.flaticon.com/authors/picol)
from [Flaticon](https://www.flaticon.com/)
and is licensed by [Creative Commons BY 3.0](http://creativecommons.org/licenses/by/3.0/).

Editor background image is taken from [wallpaper.wiki](https://wallpaper.wiki)
and it is licensed by [CC0 Creative Commons](https://creativecommons.org/publicdomain/zero/1.0/).
