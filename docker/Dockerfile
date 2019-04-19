FROM ubuntu:16.04

RUN apt-get update \
 && apt-get install -y curl git g++ make software-properties-common

ARG Qt=512
RUN apt-add-repository -y ppa:beineri/opt-qt-5.12.2-xenial \
 && apt-get update && apt-get install -y qt${Qt}base qt${Qt}tools qt${Qt}svg libgl1-mesa-dev
# disable grep and exec part of the script
RUN sed -i '/^TEST/d' /opt/qt${Qt}/bin/qt${Qt}-env.sh
RUN echo "source /opt/qt${Qt}/bin/qt${Qt}-env.sh" >> $HOME/.bashrc

RUN apt-get install -y fuse ccache zsync gcovr lcov doxygen graphviz

ARG v=9
RUN apt-add-repository 'deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main' \
    && curl https://apt.llvm.org/llvm-snapshot.gpg.key |apt-key add - \
    && apt-get update

RUN apt-get install -y clang-$v clang-format-$v clang-tidy-$v clang-tools-$v bear python-yaml \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-$v 100 \
 && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-$v 100 \
 && update-alternatives --install /usr/bin/run-clang-tidy.py run-clang-tidy.py /usr/bin/run-clang-tidy-$v.py 100 \
 && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-$v 100 \
 && update-alternatives --install /usr/bin/scan-build scan-build /usr/bin/scan-build-$v 100
