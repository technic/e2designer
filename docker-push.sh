#!/bin/bash
set -e
NAME="registry.gitlab.com/technic93/e2designer:latest"
docker build docker/qt -t "$NAME"
docker push "$NAME"
