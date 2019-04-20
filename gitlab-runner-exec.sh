#!/bin/bash
# Script to use gitlab-runner under git for Windows.
# According to https://gitlab.com/gitlab-org/gitlab-runner/issues/2438#note_34431388
# more complicated docker command is required.

if test -z "$1"; then
    echo "Usage: $0 <job>"
    exit 1
fi

export MSYS_NO_PATHCONV=1

docker run --rm \
    --volume /var/run/docker.sock:/var/run/docker.sock \
    --volume $PWD:$PWD \
    --workdir $PWD \
    gitlab/gitlab-runner:latest \
    exec docker \
        --docker-volumes "/var/run/docker.sock:/var/run/docker.sock" \
        --docker-privileged \
        --docker-pull-policy if-not-present \
        "$1"
