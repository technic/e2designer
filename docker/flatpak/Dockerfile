FROM ubuntu:18.04

RUN apt-get update \
 && apt-get install -y flatpak-builder python-yaml

ARG kde_v=5.14
RUN flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo \
 && flatpak install -y flathub org.kde.Platform//$kde_v \
 && flatpak install -y flathub org.kde.Sdk//$kde_v

COPY yaml2json.py /usr/local/bin/yaml2json.py
RUN chmod 755 /usr/local/bin/yaml2json.py
