TEMPLATE = subdirs
SUBDIRS = \
    tests \
    src \
    app

app.depends += src
tests.depends += src

DISTFILES += LICENSE README.md .gitlab-ci.yml .appveyor.yml .gitignore
