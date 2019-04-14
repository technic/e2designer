TEMPLATE = subdirs
SUBDIRS = \
    tests \
    src \
    app \
    git-version

app.depends += src git-version
tests.depends += src

DISTFILES += LICENSE README.md .gitlab-ci.yml .appveyor.yml .gitignore \
    .clang-format \
    misc/e2designer.desktop \
    docker/Dockerfile docker-push.sh gitlab-runner-exec.sh
