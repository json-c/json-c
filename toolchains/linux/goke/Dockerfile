FROM ubuntu:22.04

ADD toolchain_goke_armv6.tar.gz /opt/

RUN apt update && \
    apt install -y \
    build-essential \
    make \
    cmake \
    autoconf \
    automake \
    libtool \
    curl \
    python3 \
    pkg-config \
    ncurses-dev \
    texinfo \
    upx \
    git

RUN dpkg --add-architecture i386

RUN apt update && \
    apt install -y \
    libc6:i386 \
    libncurses5:i386 \
    libstdc++6:i386 \
    zlib1g:i386

RUN curl https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py && \
    python3 /tmp/get-pip.py && \
    pip3 install conan==1.60.1

RUN conan remote add conan_server http://conan.elear.solutions && \
    conan profile new --detect default

COPY ./profiles/ /root/.conan/profiles/

ENV COCO_BIT_ALIGNMENT 14
ENV CONAN_DEFAULT_PROFILE_PATH /root/.conan/profiles/goke_armv6
