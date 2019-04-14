#!/bin/bash
# According to https://gitlab.com/gitlab-org/gitlab-runner/issues/2438#note_34431388
# More complicated command is required on Windows

if test -z "$1"; then
    echo "Usage: $0 <job>"
    exit 1
fi

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
