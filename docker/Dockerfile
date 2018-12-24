FROM ubuntu:16.04

RUN apt-get update \
 && apt-get install -y curl git g++ make software-properties-common

RUN apt-add-repository -y ppa:beineri/opt-qt-5.11.1-xenial \
 && apt-get update && apt-get install -y qt511base qt511tools libgl1-mesa-dev
# disable grep and exec part of the script
RUN sed -i '/^TEST/d' /opt/qt511/bin/qt511-env.sh
RUN echo "source /opt/qt511/bin/qt511-env.sh" >> $HOME/.bashrc

RUN apt-get install -y fuse ccache zsync

RUN apt-get install -y clang-6.0 clang-format-6.0 clang-tidy-6.0 bear python-yaml \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-6.0 100 \
 && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-6.0 100 \
 && update-alternatives --install /usr/bin/run-clang-tidy.py run-clang-tidy.py /usr/bin/run-clang-tidy-6.0.py 100 \
 && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-6.0 100 \
 && update-alternatives --install /usr/bin/scan-build scan-build /usr/bin/scan-build-6.0 100

