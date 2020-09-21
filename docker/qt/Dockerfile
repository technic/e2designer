FROM ubuntu:16.04

RUN apt-get update \
 && apt-get install -y curl git make software-properties-common

ARG v=7
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test \
 && apt-get update && apt-get install -y gcc-$v g++-$v gcc-$v-base \
 && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-$v 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-$v 100 

ENV Qt=512
RUN apt-add-repository -y ppa:beineri/opt-qt-5.12.7-xenial \
 && apt-get update && apt-get install -y qt${Qt}base qt${Qt}tools qt${Qt}svg libgl1-mesa-dev
# disable grep and exec part of the script
RUN sed -i '/^TEST/d' /opt/qt${Qt}/bin/qt${Qt}-env.sh
RUN echo "source /opt/qt${Qt}/bin/qt${Qt}-env.sh" >> $HOME/.bashrc

RUN apt-get install -y fuse ccache zsync gcovr lcov doxygen graphviz python3-requests \
    apt-transport-https ca-certificates

ARG v=8
RUN apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-$v main" \
    && curl https://apt.llvm.org/llvm-snapshot.gpg.key |apt-key add - \
    && apt-get update

RUN apt-get install -y clang-$v clang-format-$v clang-tidy-$v clang-tools-$v bear python-yaml \
 && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-$v 100 \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-$v 100 \
 && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-$v 100 \
 && update-alternatives --install /usr/bin/run-clang-tidy.py run-clang-tidy.py /usr/bin/run-clang-tidy-$v.py 100 \
 && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-$v 100 \
 && update-alternatives --install /usr/bin/scan-build scan-build /usr/bin/scan-build-$v 100

RUN curl https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add - \
 && apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main' \
 && apt-get update && apt-get install -y cmake

RUN apt-get install -y apt-transport-https ca-certificates \
 && curl https://files.viva64.com/etc/pubkey.txt | apt-key add - \
 && curl https://files.viva64.com/etc/viva64.list -o /etc/apt/sources.list.d/viva64.list \
 && apt-get update && apt-get -y install pvs-studio
