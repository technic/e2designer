# e2designer

Skin editor for enigma2 written in C++/Qt5 framework.

[ ![](https://gitlab.com/technic93/e2designer/badges/master/pipeline.svg) ](https://gitlab.com/technic93/e2designer/pipelines/master/latest)
[ ![](https://ci.appveyor.com/api/projects/status/9t4t1mlsohkxec6o/branch/master?svg=true) ](https://ci.appveyor.com/project/technic/e2designer/branch/master)
[ ![Download](https://img.shields.io/bintray/v/technic/e2designer/e2designer?label=Download) ](https://bintray.com/technic/e2designer/e2designer/_latestVersion#files)

[ ![Coverage](https://gitlab.com/technic93/e2designer/badges/master/coverage.svg) ](https://gitlab.com/technic93/e2designer/builds/artifacts/master/file/coverage-report/index.html?job=coverage)
[ ![codecov](https://codecov.io/gl/technic93/e2designer/branch/master/graph/badge.svg) ](https://codecov.io/gl/technic93/e2designer)
[ ![CodeChecker](https://img.shields.io/badge/CodeChecker-results-yellow) ](https://gitlab.com/technic93/e2designer/builds/artifacts/master/file/reports_html/index.html?job=CodeChecker)
[ ![PVS-Studio](https://img.shields.io/badge/PVS-report-yellow) ](https://gitlab.com/technic93/e2designer/builds/artifacts/master/file/PVS-Studio.html/index.html?job=PVS-Studio)

# Screenshot

![Application window.](https://gitlab.com/technic93/e2designer/raw/master/screenshot.png)

# Insallation

Portable builds:

- Windows - download & extract, then run
- Portable Linux version - download, allow to execute and run.
- MacOS - download dmg file, allow not trusted apps, install and run. This file is not signed 

# Controls

- Ctrl + Mouse wheel - Zoom In/Out
- Arrows - Move widget
- Shift + Arrows - Precise move widget

# Compile from sources

Navigate to the e2designer sources and run

```
qmake
make
```

The executable will be named ./app/e2designer or similar

## Build dependencies

System with C++17 support and Qt version 5.11. or later

# Contributing

1. Use \*.hpp and \*.cpp for file extensions.
2. Use style defined by clang-format in the project root directory, stay with clang-8.
4. Report issues in GitLab [bugtracker](https://gitlab.com/technic93/e2designer/issues)
3. Fork and send GitLab pull requests.
4. Join discussion on the [openpli forum](https://forums.openpli.org/topic/61204-e2designer-alpha/)

## Code style
- Use `camelCase` for functions and methods 
- Use `CapitalizedNames` for classes and types
- Use `m_fooBar` style for private class members

# License
GPLv3

# Develop notes
## Testing builds in docker
To start linux docker image with qt and compiler
```bash
docker-compose run e2designer bash
```

## Updating subtrees
We need to pull upstream from time to time:
```
git subtree pull -P Qt-Color-Widgets --squash git@gitlab.com:mattia.basaglia/Qt-Color-Widgets.git master
git subtree pull -P AppImageUpdaterBridge --squash https://github.com/antony-jr/AppImageUpdaterBridge.git master
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
