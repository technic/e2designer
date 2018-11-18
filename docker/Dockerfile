FROM ubuntu:16.04

RUN apt-get update \
 && apt-get install -y curl git g++ make software-properties-common

RUN apt-add-repository -y ppa:beineri/opt-qt-5.11.1-xenial \
 && apt-get update && apt-get install -y qt511base qt511tools libgl1-mesa-dev

RUN apt-get install -y fuse ccache
RUN echo "source /opt/qt511/bin/qt511-env.sh" >> $HOME/.bashrc
