FROM debian:11.7

ADD toolchain_openwrt_mips.tar.gz /opt/

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
    python3-distutils \
    pkg-config \
    ncurses-dev \
    texinfo \
    upx \
    git

RUN curl https://bootstrap.pypa.io/get-pip.py -o /tmp/get-pip.py && \
    python3 /tmp/get-pip.py && \
    pip3 install conan==1.60.1

RUN conan remote add conan_server http://conan.elear.solutions && \
    conan profile new --detect default

COPY ./profiles/ /root/.conan/profiles/

ENV COCO_BIT_ALIGNMENT 13
ENV CONAN_DEFAULT_PROFILE_PATH /root/.conan/profiles/openwrt_mips
